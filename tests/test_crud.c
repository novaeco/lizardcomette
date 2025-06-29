#include "animals.h"
#include "breeding.h"
#include "db.h"
#include "health.h"
#include "legal_numbers.h"
#include "stocks.h"
#include "terrariums.h"
#include "transactions.h"
#include "unity.h"

TEST_CASE("animals crud", "[animals]") {
  db_set_key("");
  TEST_ASSERT_TRUE(db_open_custom(":memory:"));
  animals_init();
  Reptile r = {.id = 1,
               .elevage_id = 1,
               .name = "L'iz",
               .species = "Lizard's",
               .sex = "F",
               .birth_date = "2021",
               .health = "o'k",
               .breeding_cycle = "",
               .cdc_number = "",
               .aoe_number = "",
               .ifap_id = "",
               .quota_limit = 0,
               .quota_used = 0,
               .cerfa_valid_until = 0,
               .cites_valid_until = 0};
  TEST_ASSERT_TRUE(animals_add(&r));
  TEST_ASSERT_EQUAL_INT(1, animals_count());
  const Reptile *g = animals_get(1);
  TEST_ASSERT_NOT_NULL(g);
  TEST_ASSERT_EQUAL_STRING("L'iz", g->name);
  TEST_ASSERT_EQUAL_STRING("Lizard's", g->species);
  strcpy(r.health, "g'ood");
  TEST_ASSERT_TRUE(animals_update(1, &r));
  g = animals_get(1);
  TEST_ASSERT_EQUAL_STRING("g'ood", g->health);
  TEST_ASSERT_TRUE(animals_delete(1));
  TEST_ASSERT_EQUAL_INT(0, animals_count());
  db_close();
}

TEST_CASE("terrariums crud", "[terrariums]") {
  db_set_key("");
  TEST_ASSERT_TRUE(db_open_custom(":memory:"));
  terrariums_init(0);
  Terrarium t = {.id = 1, .elevage_id = 1, .name = "T'1", .capacity = 2};
  TEST_ASSERT_TRUE(terrariums_add(&t));
  const Terrarium *gt = terrariums_get(1);
  TEST_ASSERT_NOT_NULL(gt);
  TEST_ASSERT_EQUAL_STRING("T'1", gt->name);
  t.capacity = 3;
  TEST_ASSERT_TRUE(terrariums_update(1, &t));
  gt = terrariums_get(1);
  TEST_ASSERT_EQUAL_INT(3, gt->capacity);
  TEST_ASSERT_TRUE(terrariums_delete(1));
  TEST_ASSERT_NULL(terrariums_get(1));
  db_close();
}

TEST_CASE("stocks crud", "[stocks]") {
  db_set_key("");
  TEST_ASSERT_TRUE(db_open_custom(":memory:"));
  stocks_init();
  StockItem s = {.id = 1, .name = "Fo'o", .quantity = 5, .min_quantity = 1};
  TEST_ASSERT_TRUE(stocks_add(&s));
  const StockItem *gs = stocks_get(1);
  TEST_ASSERT_NOT_NULL(gs);
  TEST_ASSERT_EQUAL_STRING("Fo'o", gs->name);
  s.quantity = 7;
  TEST_ASSERT_TRUE(stocks_update(1, &s));
  gs = stocks_get(1);
  TEST_ASSERT_EQUAL_INT(7, gs->quantity);
  TEST_ASSERT_TRUE(stocks_delete(1));
  TEST_ASSERT_NULL(stocks_get(1));
  db_close();
}

TEST_CASE("transactions crud", "[transactions]") {
  db_set_key("");
  TEST_ASSERT_TRUE(db_open_custom(":memory:"));
  transactions_init();
  Transaction tr = {
      .id = 1, .stock_id = 1, .quantity = 2, .type = TRANSACTION_PURCHASE};
  TEST_ASSERT_TRUE(transactions_add(&tr));
  const Transaction *gt = transactions_get(1);
  TEST_ASSERT_NOT_NULL(gt);
  tr.quantity = 3;
  TEST_ASSERT_TRUE(transactions_update(1, &tr));
  gt = transactions_get(1);
  TEST_ASSERT_EQUAL_INT(3, gt->quantity);
  TEST_ASSERT_TRUE(transactions_delete(1));
  TEST_ASSERT_NULL(transactions_get(1));
  db_close();
}

TEST_CASE("health records crud", "[health]") {
  db_set_key("");
  TEST_ASSERT_TRUE(db_open_custom(":memory:"));
  health_init();
  HealthRecord hr = {
      .id = 1, .animal_id = 1, .description = "che'ck", .date = 0};
  TEST_ASSERT_TRUE(health_add(&hr));
  const HealthRecord *gr = health_get(1);
  TEST_ASSERT_NOT_NULL(gr);
  TEST_ASSERT_EQUAL_STRING("che'ck", gr->description);
  strcpy(hr.description, "do'ne");
  TEST_ASSERT_TRUE(health_update(1, &hr));
  gr = health_get(1);
  TEST_ASSERT_EQUAL_STRING("do'ne", gr->description);
  TEST_ASSERT_TRUE(health_delete(1));
  TEST_ASSERT_NULL(health_get(1));
  db_close();
}

TEST_CASE("breeding events crud", "[breeding]") {
  db_set_key("");
  TEST_ASSERT_TRUE(db_open_custom(":memory:"));
  breeding_init();
  BreedingEvent ev = {
      .id = 1, .animal_id = 1, .description = "pa'ir", .date = 0};
  TEST_ASSERT_TRUE(breeding_add(&ev));
  const BreedingEvent *ge = breeding_get(1);
  TEST_ASSERT_NOT_NULL(ge);
  TEST_ASSERT_EQUAL_STRING("pa'ir", ge->description);
  strcpy(ev.description, "la'id");
  TEST_ASSERT_TRUE(breeding_update(1, &ev));
  ge = breeding_get(1);
  TEST_ASSERT_EQUAL_STRING("la'id", ge->description);
  TEST_ASSERT_TRUE(breeding_delete(1));
  TEST_ASSERT_NULL(breeding_get(1));
  db_close();
}

TEST_CASE("legal numbers crud", "[legal_numbers]") {
  db_set_key("");
  TEST_ASSERT_TRUE(db_open_custom(":memory:"));
  legal_numbers_init();
  LegalNumber n = {.id = 1,
                   .username = "user",
                   .elevage_id = 0,
                   .type = "CDC",
                   .number = "CDC999"};
  TEST_ASSERT_TRUE(legal_numbers_add(&n));
  TEST_ASSERT_EQUAL_INT(1, legal_numbers_count());
  const LegalNumber *gn = legal_numbers_get(1);
  TEST_ASSERT_NOT_NULL(gn);
  strcpy(n.number, "CDC888");
  TEST_ASSERT_TRUE(legal_numbers_update(1, &n));
  gn = legal_numbers_get(1);
  TEST_ASSERT_EQUAL_STRING("CDC888", gn->number);
  TEST_ASSERT_TRUE(legal_numbers_delete(1));
  TEST_ASSERT_EQUAL_INT(0, legal_numbers_count());
  db_close();
}
