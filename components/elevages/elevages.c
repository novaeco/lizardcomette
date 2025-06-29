#include "elevages.h"
#include "esp_log.h"
#include "db.h"
#include <sqlite3.h>
#include <string.h>

static const char *TAG = "elevages";

static Elevage elevages[ELEVAGES_MAX];
static int elevage_count = 0;

void elevages_init(void)
{
    elevage_count = 0;
    memset(elevages, 0, sizeof(elevages));

    sqlite3_stmt *stmt = db_query("SELECT id,name,description FROM elevages;");
    if (stmt) {
        while (sqlite3_step(stmt) == SQLITE_ROW && elevage_count < ELEVAGES_MAX) {
            Elevage *e = &elevages[elevage_count++];
            e->id = sqlite3_column_int(stmt, 0);
            strncpy(e->name, (const char *)sqlite3_column_text(stmt, 1), sizeof(e->name) - 1);
            strncpy(e->description, (const char *)sqlite3_column_text(stmt, 2), sizeof(e->description) - 1);
        }
        sqlite3_finalize(stmt);
    }

    ESP_LOGI(TAG, "Initialisation des elevages");
}

static int find_index(int id)
{
    for (int i = 0; i < elevage_count; ++i) {
        if (elevages[i].id == id)
            return i;
    }
    return -1;
}

bool elevages_add(const Elevage *e)
{
    if (elevage_count >= ELEVAGES_MAX || !e)
        return false;
    sqlite3_stmt *stmt =
        db_prepare("INSERT INTO elevages(id,name,description) VALUES(?,?,?);");
    if (!stmt)
        return false;
    sqlite3_bind_int(stmt, 1, e->id);
    sqlite3_bind_text(stmt, 2, e->name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, e->description, -1, SQLITE_TRANSIENT);
    if (!db_step_finalize(stmt))
        return false;
    elevages[elevage_count] = *e;
    elevage_count++;
    ESP_LOGI(TAG, "Ajout de l'elevage %d", e->id);
    return true;
}

const Elevage *elevages_get(int id)
{
    int idx = find_index(id);
    if (idx >= 0)
        return &elevages[idx];
    return NULL;
}

bool elevages_update(int id, const Elevage *e)
{
    int idx = find_index(id);
    if (idx < 0 || !e)
        return false;
    sqlite3_stmt *stmt =
        db_prepare("UPDATE elevages SET name=?,description=? WHERE id=?;");
    if (!stmt)
        return false;
    sqlite3_bind_text(stmt, 1, e->name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, e->description, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, id);
    if (!db_step_finalize(stmt))
        return false;
    elevages[idx] = *e;
    ESP_LOGI(TAG, "Mise a jour de l'elevage %d", id);
    return true;
}

bool elevages_delete(int id)
{
    int idx = find_index(id);
    if (idx < 0)
        return false;
    sqlite3_stmt *stmt = db_prepare("DELETE FROM elevages WHERE id=?;");
    if (!stmt)
        return false;
    sqlite3_bind_int(stmt, 1, id);
    if (!db_step_finalize(stmt))
        return false;
    for (int i = idx; i < elevage_count - 1; ++i) {
        elevages[i] = elevages[i + 1];
    }
    elevage_count--;
    ESP_LOGI(TAG, "Suppression de l'elevage %d", id);
    return true;
}

int elevages_count(void)
{
    return elevage_count;
}

const Elevage *elevages_get_by_index(int index)
{
    if (index < 0 || index >= elevage_count)
        return NULL;
    return &elevages[index];
}
