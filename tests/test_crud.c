#include "unity.h"
#include "db.h"
#include "animals.h"
#include "terrariums.h"
#include "stocks.h"
#include "transactions.h"
#include "health.h"
#include "breeding.h"

TEST_CASE("animals crud","[animals]")
{
    db_set_key("");
    TEST_ASSERT_TRUE(db_open_custom(":memory:"));
    animals_init();
    Reptile r = { .id = 1, .elevage_id = 1, .name = "Liz", .species="Lizard", .sex="F", .birth_date="2021", .health="ok", .breeding_cycle="" };
    TEST_ASSERT_TRUE(animals_add(&r));
    TEST_ASSERT_EQUAL_INT(1, animals_count());
    const Reptile *g = animals_get(1);
    TEST_ASSERT_NOT_NULL(g);
    strcpy(r.health, "good");
    TEST_ASSERT_TRUE(animals_update(1, &r));
    g = animals_get(1);
    TEST_ASSERT_EQUAL_STRING("good", g->health);
    TEST_ASSERT_TRUE(animals_delete(1));
    TEST_ASSERT_EQUAL_INT(0, animals_count());
    db_close();
}

TEST_CASE("terrariums crud","[terrariums]")
{
    db_set_key("");
    TEST_ASSERT_TRUE(db_open_custom(":memory:"));
    terrariums_init();
    Terrarium t = { .id=1, .elevage_id=1, .name="T1", .capacity=2 };
    TEST_ASSERT_TRUE(terrariums_add(&t));
    const Terrarium *gt = terrariums_get(1);
    TEST_ASSERT_NOT_NULL(gt);
    t.capacity = 3;
    TEST_ASSERT_TRUE(terrariums_update(1,&t));
    gt = terrariums_get(1);
    TEST_ASSERT_EQUAL_INT(3, gt->capacity);
    TEST_ASSERT_TRUE(terrariums_delete(1));
    TEST_ASSERT_NULL(terrariums_get(1));
    db_close();
}

TEST_CASE("stocks crud","[stocks]")
{
    db_set_key("");
    TEST_ASSERT_TRUE(db_open_custom(":memory:"));
    stocks_init();
    StockItem s = { .id=1, .name="Food", .quantity=5, .min_quantity=1 };
    TEST_ASSERT_TRUE(stocks_add(&s));
    const StockItem *gs = stocks_get(1);
    TEST_ASSERT_NOT_NULL(gs);
    s.quantity = 7;
    TEST_ASSERT_TRUE(stocks_update(1,&s));
    gs = stocks_get(1);
    TEST_ASSERT_EQUAL_INT(7, gs->quantity);
    TEST_ASSERT_TRUE(stocks_delete(1));
    TEST_ASSERT_NULL(stocks_get(1));
    db_close();
}

TEST_CASE("transactions crud","[transactions]")
{
    db_set_key("");
    TEST_ASSERT_TRUE(db_open_custom(":memory:"));
    transactions_init();
    Transaction tr = { .id=1, .stock_id=1, .quantity=2, .type=TRANSACTION_PURCHASE };
    TEST_ASSERT_TRUE(transactions_add(&tr));
    const Transaction *gt = transactions_get(1);
    TEST_ASSERT_NOT_NULL(gt);
    tr.quantity = 3;
    TEST_ASSERT_TRUE(transactions_update(1,&tr));
    gt = transactions_get(1);
    TEST_ASSERT_EQUAL_INT(3, gt->quantity);
    TEST_ASSERT_TRUE(transactions_delete(1));
    TEST_ASSERT_NULL(transactions_get(1));
    db_close();
}

TEST_CASE("health records crud","[health]")
{
    db_set_key("");
    TEST_ASSERT_TRUE(db_open_custom(":memory:"));
    health_init();
    HealthRecord hr = { .id=1, .animal_id=1, .description="check", .date=0 };
    TEST_ASSERT_TRUE(health_add(&hr));
    const HealthRecord *gr = health_get(1);
    TEST_ASSERT_NOT_NULL(gr);
    strcpy(hr.description, "done");
    TEST_ASSERT_TRUE(health_update(1, &hr));
    gr = health_get(1);
    TEST_ASSERT_EQUAL_STRING("done", gr->description);
    TEST_ASSERT_TRUE(health_delete(1));
    TEST_ASSERT_NULL(health_get(1));
    db_close();
}

TEST_CASE("breeding events crud","[breeding]")
{
    db_set_key("");
    TEST_ASSERT_TRUE(db_open_custom(":memory:"));
    breeding_init();
    BreedingEvent ev = { .id=1, .animal_id=1, .description="pair", .date=0 };
    TEST_ASSERT_TRUE(breeding_add(&ev));
    const BreedingEvent *ge = breeding_get(1);
    TEST_ASSERT_NOT_NULL(ge);
    strcpy(ev.description, "laid");
    TEST_ASSERT_TRUE(breeding_update(1, &ev));
    ge = breeding_get(1);
    TEST_ASSERT_EQUAL_STRING("laid", ge->description);
    TEST_ASSERT_TRUE(breeding_delete(1));
    TEST_ASSERT_NULL(breeding_get(1));
    db_close();
}
