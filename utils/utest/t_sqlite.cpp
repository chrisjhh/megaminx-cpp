#include <gtest/gtest.h>

#include "sqlite3.h"

TEST(SQLiteTest,Basic)
{
  sqlite3* db;
  int status = sqlite3_open("./sqlite.db", &db);

  // The SQL statement to use
  std::string sql = "CREATE TABLE IF NOT EXISTS cache " 
    "(key TEXT, value TEXT);";

  // Prepare the statement
  sqlite3_stmt* create_table_stmt;
  const char* tail;
  sqlite3_prepare_v2(db, sql.c_str(), (int)sql.size(), &create_table_stmt, &tail);

  // Execute the statement
  int result = sqlite3_step(create_table_stmt);

  // Dispose of the statement
  sqlite3_finalize(create_table_stmt);

  // Check result
  EXPECT_EQ(result, SQLITE_DONE);

  // Next statement
  sql = "CREATE INDEX IF NOT EXISTS cache_keys ON cache (key);";
  sqlite3_stmt* create_index_stmt;
  sqlite3_prepare_v2(db, sql.c_str(), (int)sql.size(), &create_index_stmt, &tail);

  result = sqlite3_step(create_index_stmt);

  // Dispose of the statement
  sqlite3_finalize(create_index_stmt);

  // Check result
  EXPECT_EQ(result, SQLITE_DONE);

  sqlite3_close(db);

}
