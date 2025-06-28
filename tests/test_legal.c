#include "unity.h"
#include "legal.h"
#include "animals.h"
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

TEST_CASE("legal document generation","[legal]")
{
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
    struct stat st; 
    TEST_ASSERT_EQUAL_INT(0, stat("cerfa_test.pdf", &st));
    unlink("cerfa_test.pdf");
}

TEST_CASE("legal validation","[legal]")
{
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
}
