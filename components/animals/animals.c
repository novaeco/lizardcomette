#include "animals.h"
#include "db.h"
#include "esp_log.h"
#include "legal_numbers.h"
#include "ui.h"
#include <sqlite3.h>
#include <string.h>

static const char *TAG = "animals";

static Reptile animals[ANIMALS_MAX];
static int animal_count = 0;

void animals_init(void) {
  animal_count = 0;
  memset(animals, 0, sizeof(animals));

  sqlite3_stmt *stmt =
      db_query("SELECT "
               "id,elevage_id,name,species,sex,birth_date,health,breeding_"
               "cycle,cdc_number,aoe_number,ifap_id,quota_limit,quota_used,"
               "cerfa_valid_until,cites_valid_until FROM animals;");
  if (stmt) {
    while (sqlite3_step(stmt) == SQLITE_ROW && animal_count < ANIMALS_MAX) {
      Reptile *r = &animals[animal_count++];
      r->id = sqlite3_column_int(stmt, 0);
      r->elevage_id = sqlite3_column_int(stmt, 1);
      strncpy(r->name, (const char *)sqlite3_column_text(stmt, 2),
              sizeof(r->name) - 1);
      strncpy(r->species, (const char *)sqlite3_column_text(stmt, 3),
              sizeof(r->species) - 1);
      strncpy(r->sex, (const char *)sqlite3_column_text(stmt, 4),
              sizeof(r->sex) - 1);
      strncpy(r->birth_date, (const char *)sqlite3_column_text(stmt, 5),
              sizeof(r->birth_date) - 1);
      strncpy(r->health, (const char *)sqlite3_column_text(stmt, 6),
              sizeof(r->health) - 1);
      strncpy(r->breeding_cycle, (const char *)sqlite3_column_text(stmt, 7),
              sizeof(r->breeding_cycle) - 1);
      strncpy(r->cdc_number, (const char *)sqlite3_column_text(stmt, 8),
              sizeof(r->cdc_number) - 1);
      strncpy(r->aoe_number, (const char *)sqlite3_column_text(stmt, 9),
              sizeof(r->aoe_number) - 1);
      strncpy(r->ifap_id, (const char *)sqlite3_column_text(stmt, 10),
              sizeof(r->ifap_id) - 1);
      r->quota_limit = sqlite3_column_int(stmt, 11);
      r->quota_used = sqlite3_column_int(stmt, 12);
      r->cerfa_valid_until = sqlite3_column_int(stmt, 13);
      r->cites_valid_until = sqlite3_column_int(stmt, 14);
    }
    sqlite3_finalize(stmt);
  }

  ESP_LOGI(TAG, "Initialisation des donnees animaux");
}

bool animals_add(const Reptile *r) {
  if (animal_count >= ANIMALS_MAX || !r)
    return false;
  if (!legal_numbers_is_cdc_valid(r->cdc_number, NULL, r->elevage_id)) {
    ESP_LOGW(TAG, "CDC invalide pour %s", r->name);
    ui_notify("CDC invalide");
    return false;
  }
  if (!legal_numbers_is_aoe_valid(r->aoe_number, NULL, r->elevage_id)) {
    ESP_LOGW(TAG, "AOE invalide pour %s", r->name);
    ui_notify("AOE invalide");
    return false;
  }
  sqlite3_stmt *stmt = db_prepare(
      "INSERT INTO animals(id,elevage_id,name,species,sex,birth_date,health,"
      "breeding_cycle,cdc_number,aoe_number,ifap_id,quota_limit,quota_used,"
      "cerfa_valid_until,cites_valid_until) VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);");
  if (!stmt)
    return false;
  sqlite3_bind_int(stmt, 1, r->id);
  sqlite3_bind_int(stmt, 2, r->elevage_id);
  sqlite3_bind_text(stmt, 3, r->name, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 4, r->species, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 5, r->sex, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 6, r->birth_date, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 7, r->health, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 8, r->breeding_cycle, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 9, r->cdc_number, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 10, r->aoe_number, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 11, r->ifap_id, -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 12, r->quota_limit);
  sqlite3_bind_int(stmt, 13, r->quota_used);
  sqlite3_bind_int(stmt, 14, r->cerfa_valid_until);
  sqlite3_bind_int(stmt, 15, r->cites_valid_until);
  if (!db_step_finalize(stmt))
    return false;
  animals[animal_count] = *r;
  animal_count++;
  ESP_LOGI(TAG, "Ajout du reptile %d", r->id);
  return true;
}

static int find_index(int id) {
  for (int i = 0; i < animal_count; ++i) {
    if (animals[i].id == id)
      return i;
  }
  return -1;
}

const Reptile *animals_get(int id) {
  int idx = find_index(id);
  if (idx >= 0)
    return &animals[idx];
  return NULL;
}

bool animals_update(int id, const Reptile *r) {
  int idx = find_index(id);
  if (idx < 0 || !r)
    return false;
  if (!legal_numbers_is_cdc_valid(r->cdc_number, NULL, r->elevage_id)) {
    ESP_LOGW(TAG, "CDC invalide pour %s", r->name);
    ui_notify("CDC invalide");
    return false;
  }
  if (!legal_numbers_is_aoe_valid(r->aoe_number, NULL, r->elevage_id)) {
    ESP_LOGW(TAG, "AOE invalide pour %s", r->name);
    ui_notify("AOE invalide");
    return false;
  }
  sqlite3_stmt *stmt = db_prepare(
      "UPDATE animals SET elevage_id=?,name=?,species=?,sex=?,birth_date=?,health=?,"
      "breeding_cycle=?,cdc_number=?,aoe_number=?,ifap_id=?,quota_limit=?,quota_used=?,"
      "cerfa_valid_until=?,cites_valid_until=? WHERE id=?;");
  if (!stmt)
    return false;
  sqlite3_bind_int(stmt, 1, r->elevage_id);
  sqlite3_bind_text(stmt, 2, r->name, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, r->species, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 4, r->sex, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 5, r->birth_date, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 6, r->health, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 7, r->breeding_cycle, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 8, r->cdc_number, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 9, r->aoe_number, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 10, r->ifap_id, -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 11, r->quota_limit);
  sqlite3_bind_int(stmt, 12, r->quota_used);
  sqlite3_bind_int(stmt, 13, r->cerfa_valid_until);
  sqlite3_bind_int(stmt, 14, r->cites_valid_until);
  sqlite3_bind_int(stmt, 15, id);
  if (!db_step_finalize(stmt))
    return false;
  animals[idx] = *r;
  ESP_LOGI(TAG, "Mise a jour du reptile %d", id);
  return true;
}

bool animals_delete(int id) {
  int idx = find_index(id);
  if (idx < 0)
    return false;
  sqlite3_stmt *stmt = db_prepare("DELETE FROM animals WHERE id=?;");
  if (!stmt)
    return false;
  sqlite3_bind_int(stmt, 1, id);
  if (!db_step_finalize(stmt))
    return false;
  for (int i = idx; i < animal_count - 1; ++i) {
    animals[i] = animals[i + 1];
  }
  animal_count--;
  ESP_LOGI(TAG, "Suppression du reptile %d", id);
  return true;
}

int animals_count(void) { return animal_count; }

const Reptile *animals_get_by_index(int index) {
  if (index < 0 || index >= animal_count)
    return NULL;
  return &animals[index];
}

int animals_count_for_elevage(int elevage_id) {
  int count = 0;
  for (int i = 0; i < animal_count; ++i) {
    if (animals[i].elevage_id == elevage_id)
      count++;
  }
  return count;
}

const Reptile *animals_get_by_index_for_elevage(int index, int elevage_id) {
  int current = 0;
  for (int i = 0; i < animal_count; ++i) {
    if (animals[i].elevage_id == elevage_id) {
      if (current == index)
        return &animals[i];
      current++;
    }
  }
  return NULL;
}
