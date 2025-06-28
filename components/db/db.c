#include "db.h"
#include "esp_log.h"
#include "storage.h"
#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

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

bool db_exec(const char *format, ...)
{
    char sql[256];
    va_list args;
    va_start(args, format);
    vsnprintf(sql, sizeof(sql), format, args);
    va_end(args);

    char *err = NULL;
    if (sqlite3_exec(db_handle, sql, NULL, NULL, &err) != SQLITE_OK) {
        ESP_LOGE(TAG, "SQL exec error: %s", err ? err : "unknown");
        sqlite3_free(err);
        return false;
    }
    return true;
}

sqlite3_stmt *db_query(const char *format, ...)
{
    char sql[256];
    va_list args;
    va_start(args, format);
    vsnprintf(sql, sizeof(sql), format, args);
    va_end(args);

    sqlite3_stmt *stmt = NULL;
    if (sqlite3_prepare_v2(db_handle, sql, -1, &stmt, NULL) != SQLITE_OK) {
        ESP_LOGE(TAG, "SQL query error: %s", sqlite3_errmsg(db_handle));
        return NULL;
    }
    return stmt;
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
                "elevage_id INTEGER,"
                "name TEXT,"
                "species TEXT,"
                "sex TEXT,"
                "birth_date TEXT,"
                "health TEXT,"
                "breeding_cycle TEXT);");

    exec_simple("CREATE TABLE IF NOT EXISTS terrariums("
                "id INTEGER PRIMARY KEY,"
                "elevage_id INTEGER,"
                "name TEXT,"
                "capacity INTEGER,"
                "inventory TEXT,"
                "notes TEXT);");

    exec_simple("CREATE TABLE IF NOT EXISTS terrarium_logs("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "terrarium_id INTEGER,"
                "message TEXT,"
                "timestamp INTEGER DEFAULT (strftime('%s','now')));");

    exec_simple("CREATE TABLE IF NOT EXISTS stocks(""
                "id INTEGER PRIMARY KEY,""
                "name TEXT,""
                "quantity INTEGER,""
                "min_quantity INTEGER);");

    exec_simple("CREATE TABLE IF NOT EXISTS transactions(""
                "id INTEGER PRIMARY KEY AUTOINCREMENT,""
                "stock_id INTEGER,""
                "quantity INTEGER,""
                "type TEXT,""
                "timestamp INTEGER DEFAULT (strftime('%s','now')));");
}

void db_backup(void)
{
    ESP_LOGI(TAG, "Sauvegarde de la base de données");

    if (!db_handle)
        return;

    sqlite3 *backup_db = NULL;
    if (sqlite3_open("/sdcard/lizard_backup.db", &backup_db) != SQLITE_OK) {
        ESP_LOGE(TAG, "Erreur ouverture fichier backup");
        return;
    }

    sqlite3_backup *backup = sqlite3_backup_init(backup_db, "main", db_handle, "main");
    if (backup) {
        sqlite3_backup_step(backup, -1);
        sqlite3_backup_finish(backup);
    }
    sqlite3_close(backup_db);
    storage_encrypt_file("/sdcard/lizard_backup.db");
}

static void export_animals_csv(FILE *f)
{
    fprintf(f, "animals\n");
    fprintf(f, "id,elevage_id,name,species,sex,birth_date,health,breeding_cycle\n");

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db_handle,
                           "SELECT id,elevage_id,name,species,sex,birth_date,health,breeding_cycle FROM animals;",
                           -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            fprintf(f, "%d,%d,%s,%s,%s,%s,%s,%s\n",
                    sqlite3_column_int(stmt, 0),
                    sqlite3_column_int(stmt, 1),
                    sqlite3_column_text(stmt, 2),
                    sqlite3_column_text(stmt, 3),
                    sqlite3_column_text(stmt, 4),
                    sqlite3_column_text(stmt, 5),
                    sqlite3_column_text(stmt, 6),
                    sqlite3_column_text(stmt, 7));
        }
        sqlite3_finalize(stmt);
    }
    fprintf(f, "\n");
}

static void export_terrariums_csv(FILE *f)
{
    fprintf(f, "terrariums\n");
    fprintf(f, "id,elevage_id,name,capacity,inventory,notes\n");

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db_handle,
                           "SELECT id,elevage_id,name,capacity,inventory,notes FROM terrariums;",
                           -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            fprintf(f, "%d,%d,%s,%d,%s,%s\n",
                    sqlite3_column_int(stmt, 0),
                    sqlite3_column_int(stmt, 1),
                    sqlite3_column_text(stmt, 2),
                    sqlite3_column_int(stmt, 3),
                    sqlite3_column_text(stmt, 4),
                    sqlite3_column_text(stmt, 5));
        }
        sqlite3_finalize(stmt);
    }
    fprintf(f, "\n");
}

static void export_stocks_csv(FILE *f)
{
    fprintf(f, "stocks\n");
    fprintf(f, "id,name,quantity,min_quantity\n");

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db_handle,
                           "SELECT id,name,quantity,min_quantity FROM stocks;",
                           -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            fprintf(f, "%d,%s,%d,%d\n",
                    sqlite3_column_int(stmt, 0),
                    sqlite3_column_text(stmt, 1),
                    sqlite3_column_int(stmt, 2),
                    sqlite3_column_int(stmt, 3));
        }
        sqlite3_finalize(stmt);
    }
    fprintf(f, "\n");
}

static void export_transactions_csv(FILE *f)
{
    fprintf(f, "transactions\n");
    fprintf(f, "id,stock_id,quantity,type,timestamp\n");

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db_handle,
                           "SELECT id,stock_id,quantity,type,timestamp FROM transactions;",
                           -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            fprintf(f, "%d,%d,%d,%s,%d\n",
                    sqlite3_column_int(stmt, 0),
                    sqlite3_column_int(stmt, 1),
                    sqlite3_column_int(stmt, 2),
                    sqlite3_column_text(stmt, 3),
                    sqlite3_column_int(stmt, 4));
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
    export_stocks_csv(f);
    export_transactions_csv(f);

    fclose(f);
    ESP_LOGI(TAG, "Export CSV vers %s termine", path);
}

static void export_animals_json(FILE *f)
{
    fprintf(f, "  \"animals\": [\n");
    sqlite3_stmt *stmt;
    bool first = true;
    if (sqlite3_prepare_v2(db_handle,
                           "SELECT id,elevage_id,name,species,sex,birth_date,health,breeding_cycle FROM animals;",
                           -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            if (!first) fprintf(f, ",\n");
            first = false;
            fprintf(f, "    {\"id\":%d,\"elevage_id\":%d,\"name\":\"%s\",\"species\":\"%s\",\"sex\":\"%s\",\"birth_date\":\"%s\",\"health\":\"%s\",\"breeding_cycle\":\"%s\"}",
                    sqlite3_column_int(stmt, 0),
                    sqlite3_column_int(stmt, 1),
                    sqlite3_column_text(stmt, 2),
                    sqlite3_column_text(stmt, 3),
                    sqlite3_column_text(stmt, 4),
                    sqlite3_column_text(stmt, 5),
                    sqlite3_column_text(stmt, 6),
                    sqlite3_column_text(stmt, 7));
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
                           "SELECT id,elevage_id,name,capacity,inventory,notes FROM terrariums;",
                           -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            if (!first) fprintf(f, ",\n");
            first = false;
            fprintf(f, "    {\"id\":%d,\"elevage_id\":%d,\"name\":\"%s\",\"capacity\":%d,\"inventory\":\"%s\",\"notes\":\"%s\"}",
                    sqlite3_column_int(stmt, 0),
                    sqlite3_column_int(stmt, 1),
                    sqlite3_column_text(stmt, 2),
                    sqlite3_column_int(stmt, 3),
                    sqlite3_column_text(stmt, 4),
                    sqlite3_column_text(stmt, 5));
        }
        sqlite3_finalize(stmt);
    }
    fprintf(f, "\n  ]\n");
}

static void export_stocks_json(FILE *f)
{
    fprintf(f, "  \"stocks\": [\n");
    sqlite3_stmt *stmt;
    bool first = true;
    if (sqlite3_prepare_v2(db_handle,
                           "SELECT id,name,quantity,min_quantity FROM stocks;",
                           -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            if (!first) fprintf(f, ",\n");
            first = false;
            fprintf(f, "    {\"id\":%d,\"name\":\"%s\",\"quantity\":%d,\"min_quantity\":%d}",
                    sqlite3_column_int(stmt, 0),
                    sqlite3_column_text(stmt, 1),
                    sqlite3_column_int(stmt, 2),
                    sqlite3_column_int(stmt, 3));
        }
        sqlite3_finalize(stmt);
    }
    fprintf(f, "\n  ],\n");
}

static void export_transactions_json(FILE *f)
{
    fprintf(f, "  \"transactions\": [\n");
    sqlite3_stmt *stmt;
    bool first = true;
    if (sqlite3_prepare_v2(db_handle,
                           "SELECT id,stock_id,quantity,type,timestamp FROM transactions;",
                           -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            if (!first) fprintf(f, ",\n");
            first = false;
            fprintf(f, "    {\"id\":%d,\"stock_id\":%d,\"quantity\":%d,\"type\":\"%s\",\"timestamp\":%d}",
                    sqlite3_column_int(stmt, 0),
                    sqlite3_column_int(stmt, 1),
                    sqlite3_column_int(stmt, 2),
                    sqlite3_column_text(stmt, 3),
                    sqlite3_column_int(stmt, 4));
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
    fprintf(f, ",\n");
    export_stocks_json(f);
    fprintf(f, ",\n");
    export_transactions_json(f);
    fprintf(f, "}\n");

    fclose(f);
    ESP_LOGI(TAG, "Export JSON vers %s termine", path);
}
