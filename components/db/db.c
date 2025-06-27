#include "db.h"
#include "esp_log.h"
#include <sqlite3.h>
#include <stdio.h>
#include <string.h>

static const char *TAG = "db";
static sqlite3 *db_handle = NULL;

static void exec_simple(const char *sql)
{
    char *err = NULL;
    if (sqlite3_exec(db_handle, sql, NULL, NULL, &err) != SQLITE_OK) {
        ESP_LOGE(TAG, "SQL error: %s", err ? err : "unknown");
        sqlite3_free(err);
    }
}

void db_init(void)
{
    ESP_LOGI(TAG, "Initialisation de la base de données");

    if (sqlite3_open("/spiffs/lizard.db", &db_handle) != SQLITE_OK) {
        ESP_LOGE(TAG, "Impossible d'ouvrir la base de données: %s", sqlite3_errmsg(db_handle));
        return;
    }

    exec_simple("CREATE TABLE IF NOT EXISTS animals("
                "id INTEGER PRIMARY KEY,"
                "name TEXT,"
                "species TEXT,"
                "sex TEXT,"
                "birth_date TEXT,"
                "health TEXT,"
                "breeding_cycle TEXT);");

    exec_simple("CREATE TABLE IF NOT EXISTS terrariums("
                "id INTEGER PRIMARY KEY,"
                "name TEXT,"
                "capacity INTEGER,"
                "inventory TEXT,"
                "notes TEXT);");

    exec_simple("CREATE TABLE IF NOT EXISTS terrarium_logs("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "terrarium_id INTEGER,"
                "message TEXT,"
                "timestamp INTEGER DEFAULT (strftime('%s','now')));");
}

void db_backup(void)
{
    ESP_LOGI(TAG, "Sauvegarde de la base de données");

    if (!db_handle)
        return;

    sqlite3 *backup_db = NULL;
    if (sqlite3_open("/spiffs/lizard_backup.db", &backup_db) != SQLITE_OK) {
        ESP_LOGE(TAG, "Erreur ouverture fichier backup");
        return;
    }

    sqlite3_backup *backup = sqlite3_backup_init(backup_db, "main", db_handle, "main");
    if (backup) {
        sqlite3_backup_step(backup, -1);
        sqlite3_backup_finish(backup);
    }
    sqlite3_close(backup_db);
}

static void export_animals_csv(FILE *f)
{
    fprintf(f, "animals\n");
    fprintf(f, "id,name,species,sex,birth_date,health,breeding_cycle\n");

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db_handle,
                           "SELECT id,name,species,sex,birth_date,health,breeding_cycle FROM animals;",
                           -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            fprintf(f, "%d,%s,%s,%s,%s,%s,%s\n",
                    sqlite3_column_int(stmt, 0),
                    sqlite3_column_text(stmt, 1),
                    sqlite3_column_text(stmt, 2),
                    sqlite3_column_text(stmt, 3),
                    sqlite3_column_text(stmt, 4),
                    sqlite3_column_text(stmt, 5),
                    sqlite3_column_text(stmt, 6));
        }
        sqlite3_finalize(stmt);
    }
    fprintf(f, "\n");
}

static void export_terrariums_csv(FILE *f)
{
    fprintf(f, "terrariums\n");
    fprintf(f, "id,name,capacity,inventory,notes\n");

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db_handle,
                           "SELECT id,name,capacity,inventory,notes FROM terrariums;",
                           -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            fprintf(f, "%d,%s,%d,%s,%s\n",
                    sqlite3_column_int(stmt, 0),
                    sqlite3_column_text(stmt, 1),
                    sqlite3_column_int(stmt, 2),
                    sqlite3_column_text(stmt, 3),
                    sqlite3_column_text(stmt, 4));
        }
        sqlite3_finalize(stmt);
    }
    fprintf(f, "\n");
}

void db_export_csv(const char *path)
{
    if (!db_handle || !path)
        return;

    FILE *f = fopen(path, "w");
    if (!f) {
        ESP_LOGE(TAG, "Impossible d'ouvrir %s", path);
        return;
    }

    export_animals_csv(f);
    export_terrariums_csv(f);

    fclose(f);
    ESP_LOGI(TAG, "Export CSV vers %s termine", path);
}

static void export_animals_json(FILE *f)
{
    fprintf(f, "  \"animals\": [\n");
    sqlite3_stmt *stmt;
    bool first = true;
    if (sqlite3_prepare_v2(db_handle,
                           "SELECT id,name,species,sex,birth_date,health,breeding_cycle FROM animals;",
                           -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            if (!first) fprintf(f, ",\n");
            first = false;
            fprintf(f, "    {\"id\":%d,\"name\":\"%s\",\"species\":\"%s\",\"sex\":\"%s\",\"birth_date\":\"%s\",\"health\":\"%s\",\"breeding_cycle\":\"%s\"}",
                    sqlite3_column_int(stmt, 0),
                    sqlite3_column_text(stmt, 1),
                    sqlite3_column_text(stmt, 2),
                    sqlite3_column_text(stmt, 3),
                    sqlite3_column_text(stmt, 4),
                    sqlite3_column_text(stmt, 5),
                    sqlite3_column_text(stmt, 6));
        }
        sqlite3_finalize(stmt);
    }
    fprintf(f, "\n  ],\n");
}

static void export_terrariums_json(FILE *f)
{
    fprintf(f, "  \"terrariums\": [\n");
    sqlite3_stmt *stmt;
    bool first = true;
    if (sqlite3_prepare_v2(db_handle,
                           "SELECT id,name,capacity,inventory,notes FROM terrariums;",
                           -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            if (!first) fprintf(f, ",\n");
            first = false;
            fprintf(f, "    {\"id\":%d,\"name\":\"%s\",\"capacity\":%d,\"inventory\":\"%s\",\"notes\":\"%s\"}",
                    sqlite3_column_int(stmt, 0),
                    sqlite3_column_text(stmt, 1),
                    sqlite3_column_int(stmt, 2),
                    sqlite3_column_text(stmt, 3),
                    sqlite3_column_text(stmt, 4));
        }
        sqlite3_finalize(stmt);
    }
    fprintf(f, "\n  ]\n");
}

void db_export_json(const char *path)
{
    if (!db_handle || !path)
        return;

    FILE *f = fopen(path, "w");
    if (!f) {
        ESP_LOGE(TAG, "Impossible d'ouvrir %s", path);
        return;
    }

    fprintf(f, "{\n");
    export_animals_json(f);
    fprintf(f, ",\n");
    export_terrariums_json(f);
    fprintf(f, "}\n");

    fclose(f);
    ESP_LOGI(TAG, "Export JSON vers %s termine", path);
}
