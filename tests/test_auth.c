#include "unity.h"
#include "auth.h"
#include "db.h"
#include <unistd.h>

TEST_CASE("auth add and check","[auth]")
{
    db_set_key("");
    TEST_ASSERT_TRUE(db_open_custom("auth_test.db"));
    auth_init();
    TEST_ASSERT_TRUE(auth_add_user("user1","pass1", ROLE_PARTICULIER));
    TEST_ASSERT_TRUE(auth_check("user1","pass1"));
    TEST_ASSERT_FALSE(auth_check("user1","wrong"));
    db_close();
    TEST_ASSERT_TRUE(db_open_custom("auth_test.db"));
    auth_init();
    TEST_ASSERT_TRUE(auth_check("user1","pass1"));
    db_close();
    unlink("auth_test.db");
}

TEST_CASE("auth elevage link","[auth]")
{
    db_set_key("");
    TEST_ASSERT_TRUE(db_open_custom("auth_elev.db"));
    auth_init();
    auth_add_user("user2","pass2", ROLE_PARTICULIER);
    TEST_ASSERT_TRUE(auth_link_elevage("user2", 42));
    TEST_ASSERT_EQUAL_INT(1, auth_get_elevage_count("user2"));
    TEST_ASSERT_EQUAL_INT(42, auth_get_elevage_by_index("user2",0));
    db_close();
    TEST_ASSERT_TRUE(db_open_custom("auth_elev.db"));
    auth_init();
    TEST_ASSERT_EQUAL_INT(1, auth_get_elevage_count("user2"));
    TEST_ASSERT_EQUAL_INT(42, auth_get_elevage_by_index("user2",0));
    db_close();
    unlink("auth_elev.db");
}

TEST_CASE("auth persistence","[auth]")
{
    db_set_key("");
    TEST_ASSERT_TRUE(db_open_custom("auth_persist.db"));
    auth_init();
    TEST_ASSERT_TRUE(auth_add_user("persist","secret", ROLE_PARTICULIER));
    db_close();
    TEST_ASSERT_TRUE(db_open_custom("auth_persist.db"));
    auth_init();
    TEST_ASSERT_TRUE(auth_check("persist","secret"));
    db_close();
    unlink("auth_persist.db");
}
