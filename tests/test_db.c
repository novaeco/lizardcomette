#include "unity.h"
#include "db.h"
#include <sqlite3.h>

TEST_CASE("db exec and query","[db]")
{
    db_set_key("");
    TEST_ASSERT_TRUE(db_open_custom(":memory:"));
    TEST_ASSERT_TRUE(db_exec("CREATE TABLE test(id INTEGER PRIMARY KEY, name TEXT);"));
    TEST_ASSERT_TRUE(db_exec("INSERT INTO test(id,name) VALUES(1,'foo');"));
    sqlite3_stmt *stmt = db_query("SELECT name FROM test WHERE id=%d;",1);
    TEST_ASSERT_NOT_NULL(stmt);
    TEST_ASSERT_EQUAL_INT(SQLITE_ROW, sqlite3_step(stmt));
    TEST_ASSERT_EQUAL_STRING("foo", (const char*)sqlite3_column_text(stmt,0));
    sqlite3_finalize(stmt);
    db_close();
}
