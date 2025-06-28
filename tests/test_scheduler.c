#include "unity.h"
#include "db.h"
#include "stocks.h"
#include "animals.h"
#include "legal.h"
#include "scheduler.h"
#include <string.h>
#include <time.h>

static char last_msg[64];

void ui_notify(const char *msg)
{
    if (!msg) return;
    strncpy(last_msg, msg, sizeof(last_msg) - 1);
    last_msg[sizeof(last_msg) - 1] = '\0';
}

static void reset_msg(void)
{
    last_msg[0] = '\0';
}

TEST_CASE("scheduler low stock notification","[scheduler]")
{
    db_set_key("");
    TEST_ASSERT_TRUE(db_open_custom(":memory:"));
    stocks_init();
    StockItem s = { .id=1, .name="Crickets", .quantity=1, .min_quantity=2 };
    TEST_ASSERT_TRUE(stocks_add(&s));
    reset_msg();
    scheduler_check_stock_levels();
    TEST_ASSERT_EQUAL_STRING("Stock bas: Crickets", last_msg);
    db_close();
}

TEST_CASE("scheduler document expiration warning","[scheduler]")
{
    db_set_key("");
    TEST_ASSERT_TRUE(db_open_custom(":memory:"));
    animals_init();
    time_t now = time(NULL);
    Reptile r = {
        .id = 1,
        .elevage_id = 0,
        .name = "Liz",
        .species = "Lizard",
        .cdc_number = "CDC12345",
        .aoe_number = "AOE12345",
        .ifap_id = "IFAP1",
        .quota_limit = 10,
        .quota_used = 0,
        .cerfa_valid_until = now + 20 * 86400,
        .cites_valid_until = now + 20 * 86400
    };
    TEST_ASSERT_TRUE(animals_add(&r));
    reset_msg();
    scheduler_check_regulatory_deadlines();
    TEST_ASSERT_EQUAL_STRING("Documents bientot expir\xC3\xA9s", last_msg);
    db_close();
}
