#include "db.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "storage.h"
#include <sqlite3.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static const char *TAG = "db";
static sqlite3 *db_handle = NULL;
static char db_key[64];

static bool open_db(const char *path) {
  if (sqlite3_open(path, &db_handle) != SQLITE_OK) {
    ESP_LOGE(TAG, "Impossible d'ouvrir la base de données: %s",
             sqlite3_errmsg(db_handle));
    return false;
  }
#ifdef SQLITE_HAS_CODEC
  if (db_key[0] != '\0') {
    sqlite3_key(db_handle, db_key, strlen(db_key));
  }
#endif
  return true;
}

static void create_tables(void) {
  exec_simple("CREATE TABLE IF NOT EXISTS elevages("
              "
              "id INTEGER PRIMARY KEY,"
              "
              "name TEXT,"
              "
              "description TEXT);");

  exec_simple("CREATE TABLE IF NOT EXISTS animals("
              "
              "id INTEGER PRIMARY KEY,"
              "
              "elevage_id INTEGER,"
              "
              "name TEXT,"
              "
              "species TEXT,"
              "
              "sex TEXT,"
              "
              "birth_date TEXT,"
              "
              "health TEXT,"
              "
              "breeding_cycle TEXT,"
              "
              "cdc_number TEXT,"
              "
              "aoe_number TEXT,"
              "
              "ifap_id TEXT,"
              "
              "quota_limit INTEGER,"
              "
              "quota_used INTEGER,"
              "
              "cerfa_valid_until INTEGER,"
              "
              "cites_valid_until INTEGER);");

  exec_simple("CREATE TABLE IF NOT EXISTS terrariums("
              "
              "id INTEGER PRIMARY KEY,"
              "
              "elevage_id INTEGER,"
              "
              "name TEXT,"
              "
              "capacity INTEGER,"
              "
              "inventory TEXT,"
              "
              "notes TEXT);");

  exec_simple("CREATE TABLE IF NOT EXISTS terrarium_logs("
              "
              "id INTEGER PRIMARY KEY AUTOINCREMENT,"
              "
              "terrarium_id INTEGER,"
              "
              "message TEXT,"
              "
              "timestamp INTEGER DEFAULT (strftime('%s','now')));");

  exec_simple("CREATE TABLE IF NOT EXISTS stocks("
              "
              "id INTEGER PRIMARY KEY,"
              "
              "name TEXT,"
              "
              "quantity INTEGER,"
              "
              "min_quantity INTEGER);");

  exec_simple("CREATE TABLE IF NOT EXISTS transactions("
              "
              "id INTEGER PRIMARY KEY AUTOINCREMENT,"
              "
              "stock_id INTEGER,"
              "
              "quantity INTEGER,"
              "
              "type TEXT,"
              "
              "timestamp INTEGER DEFAULT (strftime('%s','now')));");
  exec_simple("CREATE TABLE IF NOT EXISTS users("
              "
              "username TEXT PRIMARY KEY,"
              "
              "hash TEXT,"
              "
              "role INTEGER);");

  exec_simple("CREATE TABLE IF NOT EXISTS user_elevages("
              "
              "username TEXT,"
              "
              "elevage_id INTEGER);");
  exec_simple("CREATE TABLE IF NOT EXISTS health_records("
              ""
              "id INTEGER PRIMARY KEY,"
              ""
              "animal_id INTEGER,"
              ""
              "description TEXT,"
              ""
              "date INTEGER);");

  exec_simple("CREATE TABLE IF NOT EXISTS breeding_events("
              ""
              "id INTEGER PRIMARY KEY,"
              ""
              "animal_id INTEGER,"
              ""
              "description TEXT,"
              ""
              "date INTEGER);");

  exec_simple("CREATE TABLE IF NOT EXISTS cdc_aoe_numbers("
              ""
              "id INTEGER PRIMARY KEY,"
              ""
              "username TEXT,"
              ""
              "elevage_id INTEGER,"
              ""
              "type TEXT,"
              ""
              "number TEXT);");
}

static void load_db_key(void) {
  nvs_handle_t h;
  size_t len = sizeof(db_key);
  if (nvs_open("db", NVS_READONLY, &h) == ESP_OK) {
    if (nvs_get_str(h, "key", db_key, &len) != ESP_OK) {
      db_key[0] = '\0';
    }
    nvs_close(h);
  } else {
    db_key[0] = '\0';
  }

#ifdef CONFIG_DB_DEFAULT_KEY
  if (db_key[0] == '\0' && strlen(CONFIG_DB_DEFAULT_KEY) > 0) {
    strncpy(db_key, CONFIG_DB_DEFAULT_KEY, sizeof(db_key) - 1);
    db_key[sizeof(db_key) - 1] = '\0';
  }
#endif
}

bool db_set_key(const char *key) {
  if (!key)
    return false;
  strncpy(db_key, key, sizeof(db_key) - 1);
  db_key[sizeof(db_key) - 1] = '\0';
  nvs_handle_t h;
  if (nvs_open("db", NVS_READWRITE, &h) != ESP_OK)
    return false;
  esp_err_t err = nvs_set_str(h, "key", db_key);
  if (err == ESP_OK)
    err = nvs_commit(h);
  nvs_close(h);
  return err == ESP_OK;
}

static void exec_simple(const char *sql) {
  char *err = NULL;
  if (sqlite3_exec(db_handle, sql, NULL, NULL, &err) != SQLITE_OK) {
    ESP_LOGE(TAG, "SQL error: %s", err ? err : "unknown");
    sqlite3_free(err);
  }
}

bool db_exec(const char *format, ...) {
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

sqlite3_stmt *db_query(const char *format, ...) {
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

bool db_open_custom(const char *path) {
  if (!open_db(path))
    return false;
  create_tables();
  return true;
}

void db_close(void) {
  if (db_handle) {
    sqlite3_close(db_handle);
    db_handle = NULL;
  }
}

void db_init(void) {
  ESP_LOGI(TAG, "Initialisation de la base de données");

  load_db_key();
  if (db_key[0] == '\0') {
    char buf[64];
    printf("Mot de passe BD: ");
    fflush(stdout);
    if (fgets(buf, sizeof(buf), stdin)) {
      buf[strcspn(buf, "\r\n")] = '\0';
      db_set_key(buf);
    }
  }

  if (sqlite3_open("/spiffs/lizard.db", &db_handle) != SQLITE_OK) {
    ESP_LOGE(TAG, "Impossible d'ouvrir la base de données: %s",
             sqlite3_errmsg(db_handle));
    return;
  }
#ifdef SQLITE_HAS_CODEC
  if (db_key[0] != '\0') {
    sqlite3_key(db_handle, db_key, strlen(db_key));
  }
#endif

  create_tables();
}

void db_backup(void) {
  ESP_LOGI(TAG, "Sauvegarde de la base de données");

  if (!db_handle)
    return;

  sqlite3 *backup_db = NULL;
  if (sqlite3_open("/sdcard/lizard_backup.db", &backup_db) != SQLITE_OK) {
    ESP_LOGE(TAG, "Erreur ouverture fichier backup");
    return;
  }

  sqlite3_backup *backup =
      sqlite3_backup_init(backup_db, "main", db_handle, "main");
  if (backup) {
    sqlite3_backup_step(backup, -1);
    sqlite3_backup_finish(backup);
  }
  sqlite3_close(backup_db);
  storage_encrypt_file("/sdcard/lizard_backup.db");
}

static void export_elevages_csv(FILE *f) {
  fprintf(f, "elevages\n");
  fprintf(f, "id,name,description\n");

  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db_handle, "SELECT id,name,description FROM elevages;",
                         -1, &stmt, NULL) == SQLITE_OK) {
    while (sqlite3_step(stmt) == SQLITE_ROW) {
      fprintf(f, "%d,%s,%s\n", sqlite3_column_int(stmt, 0),
              sqlite3_column_text(stmt, 1), sqlite3_column_text(stmt, 2));
    }
    sqlite3_finalize(stmt);
  }
  fprintf(f, "\n");
}

static void export_animals_csv(FILE *f) {
  fprintf(f, "animals\n");
  fprintf(f, "id,elevage_id,name,species,sex,birth_date,health,breeding_cycle,"
             "cdc_number,aoe_number,ifap_id,quota_limit,quota_used,cerfa_valid_"
             "until,cites_valid_until\n");

  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(
          db_handle,
          "SELECT "
          "id,elevage_id,name,species,sex,birth_date,health,breeding_cycle,cdc_"
          "number,aoe_number,ifap_id,quota_limit,quota_used,cerfa_valid_until,"
          "cites_valid_until FROM animals;",
          -1, &stmt, NULL) == SQLITE_OK) {
    while (sqlite3_step(stmt) == SQLITE_ROW) {
      fprintf(f, "%d,%d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d,%d,%d,%d\n",
              sqlite3_column_int(stmt, 0), sqlite3_column_int(stmt, 1),
              sqlite3_column_text(stmt, 2), sqlite3_column_text(stmt, 3),
              sqlite3_column_text(stmt, 4), sqlite3_column_text(stmt, 5),
              sqlite3_column_text(stmt, 6), sqlite3_column_text(stmt, 7),
              sqlite3_column_text(stmt, 8), sqlite3_column_text(stmt, 9),
              sqlite3_column_text(stmt, 10), sqlite3_column_int(stmt, 11),
              sqlite3_column_int(stmt, 12), sqlite3_column_int(stmt, 13),
              sqlite3_column_int(stmt, 14));
    }
    sqlite3_finalize(stmt);
  }
  fprintf(f, "\n");
}

static void export_terrariums_csv(FILE *f) {
  fprintf(f, "terrariums\n");
  fprintf(f, "id,elevage_id,name,capacity,inventory,notes\n");

  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(
          db_handle,
          "SELECT id,elevage_id,name,capacity,inventory,notes FROM terrariums;",
          -1, &stmt, NULL) == SQLITE_OK) {
    while (sqlite3_step(stmt) == SQLITE_ROW) {
      fprintf(f, "%d,%d,%s,%d,%s,%s\n", sqlite3_column_int(stmt, 0),
              sqlite3_column_int(stmt, 1), sqlite3_column_text(stmt, 2),
              sqlite3_column_int(stmt, 3), sqlite3_column_text(stmt, 4),
              sqlite3_column_text(stmt, 5));
    }
    sqlite3_finalize(stmt);
  }
  fprintf(f, "\n");
}

static void export_stocks_csv(FILE *f) {
  fprintf(f, "stocks\n");
  fprintf(f, "id,name,quantity,min_quantity\n");

  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db_handle,
                         "SELECT id,name,quantity,min_quantity FROM stocks;",
                         -1, &stmt, NULL) == SQLITE_OK) {
    while (sqlite3_step(stmt) == SQLITE_ROW) {
      fprintf(f, "%d,%s,%d,%d\n", sqlite3_column_int(stmt, 0),
              sqlite3_column_text(stmt, 1), sqlite3_column_int(stmt, 2),
              sqlite3_column_int(stmt, 3));
    }
    sqlite3_finalize(stmt);
  }
  fprintf(f, "\n");
}

static void export_transactions_csv(FILE *f) {
  fprintf(f, "transactions\n");
  fprintf(f, "id,stock_id,quantity,type,timestamp\n");

  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(
          db_handle,
          "SELECT id,stock_id,quantity,type,timestamp FROM transactions;", -1,
          &stmt, NULL) == SQLITE_OK) {
    while (sqlite3_step(stmt) == SQLITE_ROW) {
      fprintf(f, "%d,%d,%d,%s,%d\n", sqlite3_column_int(stmt, 0),
              sqlite3_column_int(stmt, 1), sqlite3_column_int(stmt, 2),
              sqlite3_column_text(stmt, 3), sqlite3_column_int(stmt, 4));
    }
    sqlite3_finalize(stmt);
  }
  fprintf(f, "\n");
}

static void export_elevages_json(FILE *f) {
  fprintf(f, "  \"elevages\": [\n");
  sqlite3_stmt *stmt;
  bool first = true;
  if (sqlite3_prepare_v2(db_handle, "SELECT id,name,description FROM elevages;",
                         -1, &stmt, NULL) == SQLITE_OK) {
    while (sqlite3_step(stmt) == SQLITE_ROW) {
      if (!first)
        fprintf(f, ",\n");
      first = false;
      fprintf(f, "    {\"id\":%d,\"name\":\"%s\",\"description\":\"%s\"}",
              sqlite3_column_int(stmt, 0), sqlite3_column_text(stmt, 1),
              sqlite3_column_text(stmt, 2));
    }
    sqlite3_finalize(stmt);
  }
  fprintf(f, "\n  ],\n");
}

void db_export_csv(const char *path) {
  if (!db_handle || !path)
    return;

  FILE *f = fopen(path, "w");
  if (!f) {
    ESP_LOGE(TAG, "Impossible d'ouvrir %s", path);
    return;
  }

  export_elevages_csv(f);
  export_animals_csv(f);
  export_terrariums_csv(f);
  export_stocks_csv(f);
  export_transactions_csv(f);

  fclose(f);
  ESP_LOGI(TAG, "Export CSV vers %s termine", path);
}

static void export_animals_json(FILE *f) {
  fprintf(f, "  \"animals\": [\n");
  sqlite3_stmt *stmt;
  bool first = true;
  if (sqlite3_prepare_v2(
          db_handle,
          "SELECT "
          "id,elevage_id,name,species,sex,birth_date,health,breeding_cycle,cdc_"
          "number,aoe_number,ifap_id,quota_limit,quota_used,cerfa_valid_until,"
          "cites_valid_until FROM animals;",
          -1, &stmt, NULL) == SQLITE_OK) {
    while (sqlite3_step(stmt) == SQLITE_ROW) {
      if (!first)
        fprintf(f, ",\n");
      first = false;
      fprintf(f,
              "    "
              "{\"id\":%d,\"elevage_id\":%d,\"name\":\"%s\",\"species\":\"%s\","
              "\"sex\":\"%s\",\"birth_date\":\"%s\",\"health\":\"%s\","
              "\"breeding_cycle\":\"%s\",\"cdc_number\":\"%s\",\"aoe_number\":"
              "\"%s\",\"ifap_id\":\"%s\",\"quota_limit\":%d,\"quota_used\":%d,"
              "\"cerfa_valid_until\":%d,\"cites_valid_until\":%d}",
              sqlite3_column_int(stmt, 0), sqlite3_column_int(stmt, 1),
              sqlite3_column_text(stmt, 2), sqlite3_column_text(stmt, 3),
              sqlite3_column_text(stmt, 4), sqlite3_column_text(stmt, 5),
              sqlite3_column_text(stmt, 6), sqlite3_column_text(stmt, 7),
              sqlite3_column_text(stmt, 8), sqlite3_column_text(stmt, 9),
              sqlite3_column_text(stmt, 10), sqlite3_column_int(stmt, 11),
              sqlite3_column_int(stmt, 12), sqlite3_column_int(stmt, 13),
              sqlite3_column_int(stmt, 14));
    }
    sqlite3_finalize(stmt);
  }
  fprintf(f, "\n  ],\n");
}

static void export_terrariums_json(FILE *f) {
  fprintf(f, "  \"terrariums\": [\n");
  sqlite3_stmt *stmt;
  bool first = true;
  if (sqlite3_prepare_v2(
          db_handle,
          "SELECT id,elevage_id,name,capacity,inventory,notes FROM terrariums;",
          -1, &stmt, NULL) == SQLITE_OK) {
    while (sqlite3_step(stmt) == SQLITE_ROW) {
      if (!first)
        fprintf(f, ",\n");
      first = false;
      fprintf(f,
              "    "
              "{\"id\":%d,\"elevage_id\":%d,\"name\":\"%s\",\"capacity\":%d,"
              "\"inventory\":\"%s\",\"notes\":\"%s\"}",
              sqlite3_column_int(stmt, 0), sqlite3_column_int(stmt, 1),
              sqlite3_column_text(stmt, 2), sqlite3_column_int(stmt, 3),
              sqlite3_column_text(stmt, 4), sqlite3_column_text(stmt, 5));
    }
    sqlite3_finalize(stmt);
  }
  fprintf(f, "\n  ]\n");
}

static void export_stocks_json(FILE *f) {
  fprintf(f, "  \"stocks\": [\n");
  sqlite3_stmt *stmt;
  bool first = true;
  if (sqlite3_prepare_v2(db_handle,
                         "SELECT id,name,quantity,min_quantity FROM stocks;",
                         -1, &stmt, NULL) == SQLITE_OK) {
    while (sqlite3_step(stmt) == SQLITE_ROW) {
      if (!first)
        fprintf(f, ",\n");
      first = false;
      fprintf(
          f,
          "    {\"id\":%d,\"name\":\"%s\",\"quantity\":%d,\"min_quantity\":%d}",
          sqlite3_column_int(stmt, 0), sqlite3_column_text(stmt, 1),
          sqlite3_column_int(stmt, 2), sqlite3_column_int(stmt, 3));
    }
    sqlite3_finalize(stmt);
  }
  fprintf(f, "\n  ],\n");
}

static void export_transactions_json(FILE *f) {
  fprintf(f, "  \"transactions\": [\n");
  sqlite3_stmt *stmt;
  bool first = true;
  if (sqlite3_prepare_v2(
          db_handle,
          "SELECT id,stock_id,quantity,type,timestamp FROM transactions;", -1,
          &stmt, NULL) == SQLITE_OK) {
    while (sqlite3_step(stmt) == SQLITE_ROW) {
      if (!first)
        fprintf(f, ",\n");
      first = false;
      fprintf(f,
              "    "
              "{\"id\":%d,\"stock_id\":%d,\"quantity\":%d,\"type\":\"%s\","
              "\"timestamp\":%d}",
              sqlite3_column_int(stmt, 0), sqlite3_column_int(stmt, 1),
              sqlite3_column_int(stmt, 2), sqlite3_column_text(stmt, 3),
              sqlite3_column_int(stmt, 4));
    }
    sqlite3_finalize(stmt);
  }
  fprintf(f, "\n  ]\n");
}

void db_export_json(const char *path) {
  if (!db_handle || !path)
    return;

  FILE *f = fopen(path, "w");
  if (!f) {
    ESP_LOGE(TAG, "Impossible d'ouvrir %s", path);
    return;
  }

  fprintf(f, "{\n");
  export_elevages_json(f);
  fprintf(f, ",\n");
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

static char *next_token(char **line)
{
  char *start = *line;
  char *comma = strchr(start, ',');
  if (comma) {
    *comma = '\0';
    *line = comma + 1;
  } else {
    *line = start + strlen(start);
  }
  return start;
}

static void clear_tables(void)
{
  exec_simple("DELETE FROM transactions;");
  exec_simple("DELETE FROM stocks;");
  exec_simple("DELETE FROM terrariums;");
  exec_simple("DELETE FROM animals;");
  exec_simple("DELETE FROM elevages;");
}

void db_import_csv(const char *path)
{
  if (!db_handle || !path)
    return;

  FILE *f = fopen(path, "r");
  if (!f) {
    ESP_LOGE(TAG, "Impossible d'ouvrir %s", path);
    return;
  }

  sqlite3_exec(db_handle, "BEGIN TRANSACTION;", NULL, NULL, NULL);
  clear_tables();

  char line[512];
  char table[32] = "";
  while (fgets(line, sizeof(line), f)) {
    line[strcspn(line, "\r\n")] = '\0';
    if (line[0] == '\0') {
      table[0] = '\0';
      continue;
    }

    if (table[0] == '\0') {
      strncpy(table, line, sizeof(table) - 1);
      table[sizeof(table) - 1] = '\0';
      fgets(line, sizeof(line), f); /* skip header */
      continue;
    }

    char *ptr = line;
    if (strcmp(table, "elevages") == 0) {
      char *tok_id = next_token(&ptr);
      char *name = next_token(&ptr);
      char *desc = ptr;
      db_exec("INSERT INTO elevages(id,name,description) VALUES(%d,'%s','%s');",
              atoi(tok_id), name, desc);
    } else if (strcmp(table, "animals") == 0) {
      char *tok[15];
      for (int i = 0; i < 14; ++i)
        tok[i] = next_token(&ptr);
      tok[14] = ptr;
      db_exec("INSERT INTO animals(id,elevage_id,name,species,sex,birth_date,health,breeding_cycle,cdc_number,aoe_number,ifap_id,quota_limit,quota_used,cerfa_valid_until,cites_valid_until) "
              "VALUES(%d,%d,'%s','%s','%s','%s','%s','%s','%s','%s','%s',%d,%d,%d,%d);",
              atoi(tok[0]), atoi(tok[1]), tok[2], tok[3], tok[4], tok[5], tok[6],
              tok[7], tok[8], tok[9], tok[10], atoi(tok[11]), atoi(tok[12]),
              atoi(tok[13]), atoi(tok[14]));
    } else if (strcmp(table, "terrariums") == 0) {
      char *tok_id = next_token(&ptr);
      char *tok_elev = next_token(&ptr);
      char *name = next_token(&ptr);
      char *cap = next_token(&ptr);
      char *inv = next_token(&ptr);
      char *notes = ptr;
      db_exec(
          "INSERT INTO terrariums(id,elevage_id,name,capacity,inventory,notes) "
          "VALUES(%d,%d,'%s',%d,'%s','%s');",
          atoi(tok_id), atoi(tok_elev), name, atoi(cap), inv, notes);
    } else if (strcmp(table, "stocks") == 0) {
      char *tok_id = next_token(&ptr);
      char *name = next_token(&ptr);
      char *qty = next_token(&ptr);
      char *min = ptr;
      db_exec("INSERT INTO stocks(id,name,quantity,min_quantity) VALUES(%d,'%s',%d,%d);",
              atoi(tok_id), name, atoi(qty), atoi(min));
    } else if (strcmp(table, "transactions") == 0) {
      char *tok_id = next_token(&ptr);
      char *stock_id = next_token(&ptr);
      char *qty = next_token(&ptr);
      char *type = next_token(&ptr);
      char *timestamp = ptr;
      db_exec(
          "INSERT INTO transactions(id,stock_id,quantity,type,timestamp) VALUES(%d,%d,%d,'%s',%d);",
          atoi(tok_id), atoi(stock_id), atoi(qty), type, atoi(timestamp));
    }
  }

  sqlite3_exec(db_handle, "COMMIT;", NULL, NULL, NULL);
  fclose(f);
  ESP_LOGI(TAG, "Import CSV depuis %s termine", path);
}
