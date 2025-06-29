#include "unity.h"
#include "db.h"
#include "animals.h"
#include "legal_numbers.h"

static char last_msg[64];

void ui_notify(const char *msg) {
    if (!msg) return;
    strncpy(last_msg, msg, sizeof(last_msg)-1);
    last_msg[sizeof(last_msg)-1] = '\0';
}

static void reset_msg(void) {
    last_msg[0] = '\0';
}

TEST_CASE("reject invalid cdc on add","[animals]")
{
    db_set_key("");
    TEST_ASSERT_TRUE(db_open_custom(":memory:"));
    legal_numbers_init();
    LegalNumber cdc={.id=1,.username="",.elevage_id=0,.type="CDC",.number="GOODCDC"};
    LegalNumber aoe={.id=2,.username="",.elevage_id=0,.type="AOE",.number="GOODAOE"};
    TEST_ASSERT_TRUE(legal_numbers_add(&cdc));
    TEST_ASSERT_TRUE(legal_numbers_add(&aoe));
    animals_init();
    Reptile r={.id=1,.elevage_id=0,.name="Liz",.species="Lizard",.sex="F",.birth_date="2020",.cdc_number="BAD",.aoe_number="GOODAOE",.ifap_id="",.quota_limit=0,.quota_used=0,.cerfa_valid_until=0,.cites_valid_until=0};
    reset_msg();
    TEST_ASSERT_FALSE(animals_add(&r));
    TEST_ASSERT_EQUAL_INT(0, animals_count());
    TEST_ASSERT_EQUAL_STRING("CDC invalide", last_msg);
    db_close();
}

TEST_CASE("reject invalid aoe on update","[animals]")
{
    db_set_key("");
    TEST_ASSERT_TRUE(db_open_custom(":memory:"));
    legal_numbers_init();
    LegalNumber cdc={.id=1,.username="",.elevage_id=0,.type="CDC",.number="GOODCDC"};
    LegalNumber aoe={.id=2,.username="",.elevage_id=0,.type="AOE",.number="GOODAOE"};
    TEST_ASSERT_TRUE(legal_numbers_add(&cdc));
    TEST_ASSERT_TRUE(legal_numbers_add(&aoe));
    animals_init();
    Reptile r={.id=1,.elevage_id=0,.name="Liz",.species="Lizard",.sex="F",.birth_date="2020",.cdc_number="GOODCDC",.aoe_number="GOODAOE",.ifap_id="",.quota_limit=0,.quota_used=0,.cerfa_valid_until=0,.cites_valid_until=0};
    TEST_ASSERT_TRUE(animals_add(&r));
    r.aoe_number[0]='\0';
    strcpy(r.aoe_number,"BAD");
    reset_msg();
    TEST_ASSERT_FALSE(animals_update(1,&r));
    const Reptile *g = animals_get(1);
    TEST_ASSERT_NOT_NULL(g);
    TEST_ASSERT_EQUAL_STRING("GOODAOE", g->aoe_number);
    TEST_ASSERT_EQUAL_STRING("AOE invalide", last_msg);
    db_close();
}
