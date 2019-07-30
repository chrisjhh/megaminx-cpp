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
  sql = "CREATE UNIQUE INDEX IF NOT EXISTS cache_keys ON cache (key);";
  sqlite3_stmt* create_index_stmt;
  sqlite3_prepare_v2(db, sql.c_str(), (int)sql.size(), &create_index_stmt, &tail);

  result = sqlite3_step(create_index_stmt);

  // Dispose of the statement
  sqlite3_finalize(create_index_stmt);

  // Check result
  EXPECT_EQ(result, SQLITE_DONE);

  // Insert into table
  sql = "INSERT INTO cache (key,value) VALUES (?,?);";
  sqlite3_stmt* insert_stmt;
  sqlite3_prepare_v2(db, sql.c_str(), (int)sql.size(), &insert_stmt, &tail);

  std::string key = "name";
  std::string value = "Chris";
  sqlite3_bind_text(insert_stmt, 1, key.c_str(), (int)key.size(), SQLITE_STATIC);
  sqlite3_bind_text(insert_stmt, 2, value.c_str(), (int)value.size(), SQLITE_STATIC);

  // Run the statement
  result = sqlite3_step(insert_stmt);
  if (result != SQLITE_CONSTRAINT) {
    EXPECT_EQ(result, SQLITE_DONE);
  }

  // Reuse the statement to insert again
  sqlite3_reset(insert_stmt);

  key = "age";
  value = "48";
  sqlite3_bind_text(insert_stmt, 1, key.c_str(), (int)key.size(), SQLITE_STATIC);
  sqlite3_bind_text(insert_stmt, 2, value.c_str(), (int)value.size(), SQLITE_STATIC);

  // Run the statement
  result = sqlite3_step(insert_stmt);
  if (result != SQLITE_CONSTRAINT) {
    EXPECT_EQ(result, SQLITE_DONE);
  }

  // Dispose of the statement
  sqlite3_finalize(insert_stmt);

  // Query an entry
  sql = "SELECT value FROM cache WHERE key = ?;";
  sqlite3_stmt* select_stmt;
  sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &select_stmt, &tail);

  key = "name";
  sqlite3_bind_text(select_stmt, 1, key.c_str(), (int)key.size(), SQLITE_STATIC);

  result = sqlite3_step(select_stmt);
  ASSERT_EQ(result, SQLITE_ROW);
  const char* value_str = (const char*)sqlite3_column_text(select_stmt, 0);
  EXPECT_EQ(std::string(value_str), "Chris");

  // Reuse statement for another query
  sqlite3_reset(select_stmt);

  key = "age";
  sqlite3_bind_text(select_stmt, 1, key.c_str(), (int)key.size(), SQLITE_STATIC);

  result = sqlite3_step(select_stmt);
  ASSERT_EQ(result, SQLITE_ROW);
  value_str = (const char*)sqlite3_column_text(select_stmt, 0);
  EXPECT_EQ(std::string(value_str), "48");

  // Reuse statement for another query for something that does not exist
  sqlite3_reset(select_stmt);

  key = "wibble";
  sqlite3_bind_text(select_stmt, 1, key.c_str(), (int)key.size(), SQLITE_STATIC);

  result = sqlite3_step(select_stmt);
  ASSERT_EQ(result, SQLITE_DONE);

  // Dispose of statement
  sqlite3_finalize(select_stmt);

  sqlite3_close(db);

}
