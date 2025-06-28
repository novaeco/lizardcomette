#include "unity.h"
#include "auth.h"

TEST_CASE("auth add and check","[auth]")
{
    auth_init();
    TEST_ASSERT_TRUE(auth_add_user("user1","pass1", ROLE_PARTICULIER));
    TEST_ASSERT_TRUE(auth_check("user1","pass1"));
    TEST_ASSERT_FALSE(auth_check("user1","wrong"));
}

TEST_CASE("auth elevage link","[auth]")
{
    auth_init();
    auth_add_user("user2","pass2", ROLE_PARTICULIER);
    TEST_ASSERT_TRUE(auth_link_elevage("user2", 42));
    TEST_ASSERT_EQUAL_INT(1, auth_get_elevage_count("user2"));
    TEST_ASSERT_EQUAL_INT(42, auth_get_elevage_by_index("user2",0));
}
