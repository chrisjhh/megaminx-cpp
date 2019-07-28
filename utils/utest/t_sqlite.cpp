#include <gtest/gtest.h>

#include <sqlite3/sqlite3.h>

TEST(SQLiteTest,Basic)
{
  sqlite3* db;
  int status = sqlite3_open("./sqlite.db", &db);
  sqlite3_close(db);

}