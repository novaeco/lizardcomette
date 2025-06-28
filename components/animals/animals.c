#include "animals.h"
#include "db.h"
#include "esp_log.h"
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
  if (!db_exec("INSERT INTO "
               "animals(id,elevage_id,name,species,sex,birth_date,health,"
               "breeding_cycle,cdc_number,aoe_number,ifap_id,quota_limit,quota_"
               "used,cerfa_valid_until,cites_valid_until) "
               "VALUES(%d,%d,'%s','%s','%s','%s','%s','%s','%s','%s','%s',%d,%"
               "d,%d,%d);",
               r->id, r->elevage_id, r->name, r->species, r->sex, r->birth_date,
               r->health, r->breeding_cycle, r->cdc_number, r->aoe_number,
               r->ifap_id, r->quota_limit, r->quota_used, r->cerfa_valid_until,
               r->cites_valid_until))
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
  if (!db_exec(
          "UPDATE animals SET elevage_id=%d,name='%s',species='%s',sex='%s',"
          "birth_date='%s',health='%s',breeding_cycle='%s',cdc_number='%s',aoe_"
          "number='%s',ifap_id='%s',quota_limit=%d,quota_used=%d,cerfa_valid_"
          "until=%d,cites_valid_until=%d WHERE id=%d;",
          r->elevage_id, r->name, r->species, r->sex, r->birth_date, r->health,
          r->breeding_cycle, r->cdc_number, r->aoe_number, r->ifap_id,
          r->quota_limit, r->quota_used, r->cerfa_valid_until,
          r->cites_valid_until, id))
    return false;
  animals[idx] = *r;
  ESP_LOGI(TAG, "Mise a jour du reptile %d", id);
  return true;
}

bool animals_delete(int id) {
  int idx = find_index(id);
  if (idx < 0)
    return false;
  if (!db_exec("DELETE FROM animals WHERE id=%d;", id))
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
