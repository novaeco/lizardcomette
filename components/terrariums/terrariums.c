#include "terrariums.h"
#include "esp_log.h"
#include "db.h"
#include <sqlite3.h>
#include <string.h>

static const char *TAG = "terrariums";

static Terrarium terrariums[TERRARIUMS_MAX];
static int terrarium_count = 0;
static char logs[LOGS_MAX][64];
static int log_count = 0;

void terrariums_init(int log_offset)
{
    terrarium_count = 0;
    log_count = 0;
    memset(terrariums, 0, sizeof(terrariums));

    sqlite3_stmt *stmt = db_query("SELECT id,elevage_id,name,capacity,inventory,notes FROM terrariums;");
    if (stmt) {
        while (sqlite3_step(stmt) == SQLITE_ROW && terrarium_count < TERRARIUMS_MAX) {
            Terrarium *t = &terrariums[terrarium_count++];
            t->id = sqlite3_column_int(stmt, 0);
            t->elevage_id = sqlite3_column_int(stmt, 1);
            strncpy(t->name, (const char *)sqlite3_column_text(stmt, 2), sizeof(t->name) - 1);
            t->capacity = sqlite3_column_int(stmt, 3);
            strncpy(t->inventory, (const char *)sqlite3_column_text(stmt, 4), sizeof(t->inventory) - 1);
            strncpy(t->notes, (const char *)sqlite3_column_text(stmt, 5), sizeof(t->notes) - 1);
        }
        sqlite3_finalize(stmt);
    }

    // Chargement des logs existants avec pagination
    sqlite3_stmt *log_stmt = db_query(
        "SELECT message FROM terrarium_logs ORDER BY id DESC LIMIT %d OFFSET %d;",
        LOGS_MAX, log_offset);
    if (log_stmt) {
        while (sqlite3_step(log_stmt) == SQLITE_ROW && log_count < LOGS_MAX) {
            const unsigned char *msg = sqlite3_column_text(log_stmt, 0);
            if (msg) {
                strncpy(logs[log_count], (const char *)msg, sizeof(logs[0]) - 1);
                logs[log_count][sizeof(logs[0]) - 1] = '\0';
                log_count++;
            }
        }
        sqlite3_finalize(log_stmt);
    }

    ESP_LOGI(TAG, "Initialisation des terrariums");
}

bool terrariums_add(const Terrarium *t)
{
    if (terrarium_count >= TERRARIUMS_MAX || !t)
        return false;
    if (!db_exec("INSERT INTO terrariums(id,elevage_id,name,capacity,inventory,notes) "
                 "VALUES(%d,%d,'%s',%d,'%s','%s');",
                 t->id, t->elevage_id, t->name, t->capacity, t->inventory, t->notes))
        return false;
    terrariums[terrarium_count] = *t;
    terrarium_count++;
    ESP_LOGI(TAG, "Ajout du terrarium %d", t->id);
    return true;
}

static int find_index(int id)
{
    for (int i = 0; i < terrarium_count; ++i) {
        if (terrariums[i].id == id)
            return i;
    }
    return -1;
}

const Terrarium *terrariums_get(int id)
{
    int idx = find_index(id);
    if (idx >= 0)
        return &terrariums[idx];
    return NULL;
}

bool terrariums_update(int id, const Terrarium *t)
{
    int idx = find_index(id);
    if (idx < 0 || !t)
        return false;
    if (!db_exec("UPDATE terrariums SET elevage_id=%d,name='%s',capacity=%d,inventory='%s',notes='%s' WHERE id=%d;",
                 t->elevage_id, t->name, t->capacity, t->inventory, t->notes, id))
        return false;
    terrariums[idx] = *t;
    ESP_LOGI(TAG, "Mise a jour du terrarium %d", id);
    return true;
}

bool terrariums_delete(int id)
{
    int idx = find_index(id);
    if (idx < 0)
        return false;
    if (!db_exec("DELETE FROM terrariums WHERE id=%d;", id))
        return false;
    for (int i = idx; i < terrarium_count - 1; ++i) {
        terrariums[i] = terrariums[i + 1];
    }
    terrarium_count--;
    ESP_LOGI(TAG, "Suppression du terrarium %d", id);
    return true;
}

void terrariums_log_transaction(const char *msg)
{
    if (log_count >= LOGS_MAX || !msg)
        return;
    db_exec("INSERT INTO terrarium_logs(message,terrarium_id) VALUES('%s',0);", msg);
    strncpy(logs[log_count], msg, sizeof(logs[0]) - 1);
    logs[log_count][sizeof(logs[0]) - 1] = '\0';
    ESP_LOGI(TAG, "Log: %s", logs[log_count]);
    log_count++;
}

int terrariums_count_for_elevage(int elevage_id)
{
    int count = 0;
    for (int i = 0; i < terrarium_count; ++i) {
        if (terrariums[i].elevage_id == elevage_id)
            count++;
    }
    return count;
}

const Terrarium *terrariums_get_by_index_for_elevage(int index, int elevage_id)
{
    int current = 0;
    for (int i = 0; i < terrarium_count; ++i) {
        if (terrariums[i].elevage_id == elevage_id) {
            if (current == index)
                return &terrariums[i];
            current++;
        }
    }
    return NULL;
}
