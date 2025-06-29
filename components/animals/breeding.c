#include "breeding.h"
#include "esp_log.h"
#include "db.h"
#include <sqlite3.h>
#include <string.h>

static const char *TAG = "breeding";

static BreedingEvent events[BREEDING_MAX];
static int event_count = 0;

static int find_index(int id)
{
    for (int i = 0; i < event_count; ++i) {
        if (events[i].id == id)
            return i;
    }
    return -1;
}

void breeding_init(void)
{
    event_count = 0;
    memset(events, 0, sizeof(events));

    sqlite3_stmt *stmt = db_query("SELECT id,animal_id,description,date FROM breeding_events;");
    if (stmt) {
        while (sqlite3_step(stmt) == SQLITE_ROW && event_count < BREEDING_MAX) {
            BreedingEvent *e = &events[event_count++];
            e->id = sqlite3_column_int(stmt, 0);
            e->animal_id = sqlite3_column_int(stmt, 1);
            strncpy(e->description, (const char *)sqlite3_column_text(stmt, 2), sizeof(e->description) - 1);
            e->date = sqlite3_column_int(stmt, 3);
        }
        sqlite3_finalize(stmt);
    }
    ESP_LOGI(TAG, "Initialisation des evenements reproduction");
}

bool breeding_add(const BreedingEvent *ev)
{
    if (event_count >= BREEDING_MAX || !ev)
        return false;
    sqlite3_stmt *stmt = db_query(
        "INSERT INTO breeding_events(id,animal_id,description,date) VALUES(?,?,?,?);");
    if (!stmt)
        return false;
    sqlite3_bind_int(stmt, 1, ev->id);
    sqlite3_bind_int(stmt, 2, ev->animal_id);
    sqlite3_bind_text(stmt, 3, ev->description, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, ev->date);
    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    if (!ok)
        return false;
    events[event_count] = *ev;
    event_count++;
    ESP_LOGI(TAG, "Ajout evenement reproduction %d", ev->id);
    return true;
}

const BreedingEvent *breeding_get(int id)
{
    int idx = find_index(id);
    if (idx >= 0)
        return &events[idx];
    return NULL;
}

bool breeding_update(int id, const BreedingEvent *ev)
{
    int idx = find_index(id);
    if (idx < 0 || !ev)
        return false;
    sqlite3_stmt *stmt = db_query(
        "UPDATE breeding_events SET animal_id=?,description=?,date=? WHERE id=?;");
    if (!stmt)
        return false;
    sqlite3_bind_int(stmt, 1, ev->animal_id);
    sqlite3_bind_text(stmt, 2, ev->description, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, ev->date);
    sqlite3_bind_int(stmt, 4, id);
    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    if (!ok)
        return false;
    events[idx] = *ev;
    events[idx].id = id;
    ESP_LOGI(TAG, "Mise a jour evenement reproduction %d", id);
    return true;
}

bool breeding_delete(int id)
{
    int idx = find_index(id);
    if (idx < 0)
        return false;
    sqlite3_stmt *stmt = db_query("DELETE FROM breeding_events WHERE id=?;");
    if (!stmt)
        return false;
    sqlite3_bind_int(stmt, 1, id);
    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    if (!ok)
        return false;
    for (int i = idx; i < event_count - 1; ++i)
        events[i] = events[i + 1];
    event_count--;
    ESP_LOGI(TAG, "Suppression evenement reproduction %d", id);
    return true;
}

int breeding_count(void)
{
    return event_count;
}

const BreedingEvent *breeding_get_by_index(int index)
{
    if (index < 0 || index >= event_count)
        return NULL;
    return &events[index];
}

int breeding_count_for_animal(int animal_id)
{
    int cnt = 0;
    for (int i = 0; i < event_count; ++i)
        if (events[i].animal_id == animal_id)
            cnt++;
    return cnt;
}

const BreedingEvent *breeding_get_by_index_for_animal(int index, int animal_id)
{
    int current = 0;
    for (int i = 0; i < event_count; ++i) {
        if (events[i].animal_id == animal_id) {
            if (current == index)
                return &events[i];
            current++;
        }
    }
    return NULL;
}

