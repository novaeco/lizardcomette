#include "unity.h"
#include "legal.h"
#include "legal_numbers.h"
#include "db.h"
#include "animals.h"
#include "transactions.h"
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

TEST_CASE("legal document generation","[legal]")
{
    db_set_key("");
    TEST_ASSERT_TRUE(db_open_custom(":memory:"));
    legal_numbers_init();
    LegalNumber n1 = { .id=1, .username="", .elevage_id=0, .type="CDC", .number="CDC12345" };
    LegalNumber n2 = { .id=2, .username="", .elevage_id=0, .type="AOE", .number="AOE12345" };
    TEST_ASSERT_TRUE(legal_numbers_add(&n1));
    TEST_ASSERT_TRUE(legal_numbers_add(&n2));

    Reptile r = {
        .name = "Test",
        .species = "Lizard",
        .sex = "M",
        .birth_date = "2020-01-01",
        .cdc_number = "CDC12345",
        .aoe_number = "AOE12345",
        .ifap_id = "ID1",
        .quota_limit = 10,
        .quota_used = 1,
        .cerfa_valid_until = (int)time(NULL)+3600,
        .cites_valid_until = (int)time(NULL)+3600
    };
    TEST_ASSERT_TRUE(legal_generate_cerfa("cerfa_test.pdf", &r));
    TEST_ASSERT_TRUE(legal_generate_cerfa_14367("cerfa_14367_test.pdf", &r));
    struct stat st;
    TEST_ASSERT_EQUAL_INT(0, stat("cerfa_test.pdf", &st));
    TEST_ASSERT_EQUAL_INT(0, stat("cerfa_14367_test.pdf", &st));
    unlink("cerfa_test.pdf");
    unlink("cerfa_14367_test.pdf");
    db_close();
}

TEST_CASE("legal validation","[legal]")
{
    db_set_key("");
    TEST_ASSERT_TRUE(db_open_custom(":memory:"));
    legal_numbers_init();
    LegalNumber n1 = { .id=1, .username="", .elevage_id=0, .type="CDC", .number="CDC12345" };
    LegalNumber n2 = { .id=2, .username="", .elevage_id=0, .type="AOE", .number="AOE67890" };
    TEST_ASSERT_TRUE(legal_numbers_add(&n1));
    TEST_ASSERT_TRUE(legal_numbers_add(&n2));

    Reptile r = {
        .cdc_number = "CDC12345",
        .aoe_number = "AOE67890",
        .cerfa_valid_until = (int)time(NULL)+3600,
        .cites_valid_until = (int)time(NULL)+3600,
        .quota_limit = 2,
        .quota_used = 1
    };
    TEST_ASSERT_TRUE(legal_is_cdc_valid(&r));
    TEST_ASSERT_TRUE(legal_is_aoe_valid(&r));
    TEST_ASSERT_TRUE(legal_is_cerfa_valid(&r));
    TEST_ASSERT_TRUE(legal_is_cites_valid(&r));
    TEST_ASSERT_TRUE(legal_quota_remaining(&r));
    db_close();
}

TEST_CASE("quota edge cases","[legal]")
{
    Reptile r = { .quota_limit = 0, .quota_used = 5 };
    TEST_ASSERT_TRUE(legal_quota_remaining(&r));
    r.quota_limit = 5;
    r.quota_used = 5;
    TEST_ASSERT_FALSE(legal_quota_remaining(&r));
    r.quota_used = -1;
    TEST_ASSERT_FALSE(legal_quota_remaining(&r));
}

TEST_CASE("invoice generated on sale","[transactions]")
{
    db_set_key("");
    TEST_ASSERT_TRUE(db_open_custom(":memory:"));
    animals_init();
    transactions_init();
    Reptile r = {
        .id = 1,
        .elevage_id = 0,
        .name = "Liz",
        .species = "Lizard",
        .sex = "F",
        .birth_date = "2021",
        .cdc_number = "",
        .aoe_number = "",
        .ifap_id = "",
        .quota_limit = 0,
        .quota_used = 0,
        .cerfa_valid_until = 0,
        .cites_valid_until = 0
    };
    TEST_ASSERT_TRUE(animals_add(&r));
    Transaction t = { .id = 1, .stock_id = 1, .quantity = 1, .type = TRANSACTION_SALE };
    TEST_ASSERT_TRUE(transactions_add(&t));
    struct stat st;
    TEST_ASSERT_EQUAL_INT(0, stat("invoice_1.pdf", &st));
    unlink("invoice_1.pdf");
    db_close();
}
