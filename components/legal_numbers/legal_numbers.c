#include "legal_numbers.h"
#include "db.h"
#include "esp_log.h"
#include <sqlite3.h>
#include <string.h>

static const char *TAG = "legal_numbers";

static LegalNumber numbers[LEGAL_NUMBERS_MAX];
static int number_count = 0;

static int find_index(int id)
{
    for (int i = 0; i < number_count; ++i) {
        if (numbers[i].id == id)
            return i;
    }
    return -1;
}

void legal_numbers_init(void)
{
    number_count = 0;
    memset(numbers, 0, sizeof(numbers));

    sqlite3_stmt *stmt = db_query("SELECT id,username,elevage_id,type,number FROM cdc_aoe_numbers;");
    if (stmt) {
        while (sqlite3_step(stmt) == SQLITE_ROW && number_count < LEGAL_NUMBERS_MAX) {
            LegalNumber *n = &numbers[number_count++];
            n->id = sqlite3_column_int(stmt, 0);
            const unsigned char *u = sqlite3_column_text(stmt, 1);
            if (u)
                strncpy(n->username, (const char *)u, sizeof(n->username) - 1);
            n->elevage_id = sqlite3_column_int(stmt, 2);
            const unsigned char *t = sqlite3_column_text(stmt, 3);
            if (t)
                strncpy(n->type, (const char *)t, sizeof(n->type) - 1);
            const unsigned char *num = sqlite3_column_text(stmt, 4);
            if (num)
                strncpy(n->number, (const char *)num, sizeof(n->number) - 1);
        }
        sqlite3_finalize(stmt);
    }

    ESP_LOGI(TAG, "Initialisation des numeros CDC/AOE");
}

bool legal_numbers_add(const LegalNumber *n)
{
    if (number_count >= LEGAL_NUMBERS_MAX || !n)
        return false;
    sqlite3_stmt *stmt = db_query(
        "INSERT INTO cdc_aoe_numbers(id,username,elevage_id,type,number) VALUES(?,?,?,?,?);");
    if (!stmt)
        return false;
    sqlite3_bind_int(stmt, 1, n->id);
    sqlite3_bind_text(stmt, 2, n->username, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, n->elevage_id);
    sqlite3_bind_text(stmt, 4, n->type, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, n->number, -1, SQLITE_TRANSIENT);
    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    if (!ok)
        return false;
    numbers[number_count] = *n;
    number_count++;
    ESP_LOGI(TAG, "Ajout numero %d", n->id);
    return true;
}

const LegalNumber *legal_numbers_get(int id)
{
    int idx = find_index(id);
    if (idx >= 0)
        return &numbers[idx];
    return NULL;
}

bool legal_numbers_update(int id, const LegalNumber *n)
{
    int idx = find_index(id);
    if (idx < 0 || !n)
        return false;
    sqlite3_stmt *stmt =
        db_query("UPDATE cdc_aoe_numbers SET username=?,elevage_id=?,type=?,number=? WHERE id=?;");
    if (!stmt)
        return false;
    sqlite3_bind_text(stmt, 1, n->username, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, n->elevage_id);
    sqlite3_bind_text(stmt, 3, n->type, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, n->number, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 5, id);
    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    if (!ok)
        return false;
    numbers[idx] = *n;
    ESP_LOGI(TAG, "Mise a jour numero %d", id);
    return true;
}

bool legal_numbers_delete(int id)
{
    int idx = find_index(id);
    if (idx < 0)
        return false;
    sqlite3_stmt *stmt = db_query("DELETE FROM cdc_aoe_numbers WHERE id=?;");
    if (!stmt)
        return false;
    sqlite3_bind_int(stmt, 1, id);
    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    if (!ok)
        return false;
    for (int i = idx; i < number_count - 1; ++i)
        numbers[i] = numbers[i + 1];
    number_count--;
    ESP_LOGI(TAG, "Suppression numero %d", id);
    return true;
}

int legal_numbers_count(void)
{
    return number_count;
}

const LegalNumber *legal_numbers_get_by_index(int index)
{
    if (index < 0 || index >= number_count)
        return NULL;
    return &numbers[index];
}

static bool number_matches(const LegalNumber *n, const char *number, const char *type, const char *username, int elevage_id)
{
    if (strcmp(n->type, type) != 0)
        return false;
    if (strcmp(n->number, number) != 0)
        return false;
    if (n->username[0] != '\0' && (!username || strcmp(n->username, username) != 0))
        return false;
    if (n->elevage_id != 0 && n->elevage_id != elevage_id)
        return false;
    return true;
}

static bool is_valid(const char *number, const char *type, const char *username, int elevage_id)
{
    if (!number)
        return false;
    for (int i = 0; i < number_count; ++i) {
        if (number_matches(&numbers[i], number, type, username, elevage_id))
            return true;
    }
    return false;
}

bool legal_numbers_is_cdc_valid(const char *number, const char *username, int elevage_id)
{
    return is_valid(number, "CDC", username, elevage_id);
}

bool legal_numbers_is_aoe_valid(const char *number, const char *username, int elevage_id)
{
    return is_valid(number, "AOE", username, elevage_id);
}

