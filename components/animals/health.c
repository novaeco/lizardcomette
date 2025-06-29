#include "health.h"
#include "esp_log.h"
#include "db.h"
#include <sqlite3.h>
#include <string.h>

static const char *TAG = "health";

static HealthRecord records[HEALTH_MAX];
static int record_count = 0;

static int find_index(int id)
{
    for (int i = 0; i < record_count; ++i) {
        if (records[i].id == id)
            return i;
    }
    return -1;
}

void health_init(void)
{
    record_count = 0;
    memset(records, 0, sizeof(records));

    sqlite3_stmt *stmt = db_query("SELECT id,animal_id,description,date FROM health_records;");
    if (stmt) {
        while (sqlite3_step(stmt) == SQLITE_ROW && record_count < HEALTH_MAX) {
            HealthRecord *r = &records[record_count++];
            r->id = sqlite3_column_int(stmt, 0);
            r->animal_id = sqlite3_column_int(stmt, 1);
            strncpy(r->description, (const char *)sqlite3_column_text(stmt, 2), sizeof(r->description) - 1);
            r->date = sqlite3_column_int(stmt, 3);
        }
        sqlite3_finalize(stmt);
    }
    ESP_LOGI(TAG, "Initialisation des fiches sante");
}

bool health_add(const HealthRecord *rec)
{
    if (record_count >= HEALTH_MAX || !rec)
        return false;
    sqlite3_stmt *stmt = db_prepare(
        "INSERT INTO health_records(id,animal_id,description,date) VALUES(?,?,?,?);");
    if (!stmt)
        return false;
    sqlite3_bind_int(stmt, 1, rec->id);
    sqlite3_bind_int(stmt, 2, rec->animal_id);
    sqlite3_bind_text(stmt, 3, rec->description, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, rec->date);
    if (!db_step_finalize(stmt))
        return false;
    records[record_count] = *rec;
    record_count++;
    ESP_LOGI(TAG, "Ajout fiche sante %d", rec->id);
    return true;
}

const HealthRecord *health_get(int id)
{
    int idx = find_index(id);
    if (idx >= 0)
        return &records[idx];
    return NULL;
}

bool health_update(int id, const HealthRecord *rec)
{
    int idx = find_index(id);
    if (idx < 0 || !rec)
        return false;
    sqlite3_stmt *stmt = db_prepare(
        "UPDATE health_records SET animal_id=?,description=?,date=? WHERE id=?;");
    if (!stmt)
        return false;
    sqlite3_bind_int(stmt, 1, rec->animal_id);
    sqlite3_bind_text(stmt, 2, rec->description, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, rec->date);
    sqlite3_bind_int(stmt, 4, id);
    if (!db_step_finalize(stmt))
        return false;
    records[idx] = *rec;
    records[idx].id = id;
    ESP_LOGI(TAG, "Mise a jour fiche sante %d", id);
    return true;
}

bool health_delete(int id)
{
    int idx = find_index(id);
    if (idx < 0)
        return false;
    sqlite3_stmt *stmt = db_prepare("DELETE FROM health_records WHERE id=?;");
    if (!stmt)
        return false;
    sqlite3_bind_int(stmt, 1, id);
    if (!db_step_finalize(stmt))
        return false;
    for (int i = idx; i < record_count - 1; ++i)
        records[i] = records[i + 1];
    record_count--;
    ESP_LOGI(TAG, "Suppression fiche sante %d", id);
    return true;
}

int health_count(void)
{
    return record_count;
}

const HealthRecord *health_get_by_index(int index)
{
    if (index < 0 || index >= record_count)
        return NULL;
    return &records[index];
}

int health_count_for_animal(int animal_id)
{
    int cnt = 0;
    for (int i = 0; i < record_count; ++i)
        if (records[i].animal_id == animal_id)
            cnt++;
    return cnt;
}

const HealthRecord *health_get_by_index_for_animal(int index, int animal_id)
{
    int current = 0;
    for (int i = 0; i < record_count; ++i) {
        if (records[i].animal_id == animal_id) {
            if (current == index)
                return &records[i];
            current++;
        }
    }
    return NULL;
}

