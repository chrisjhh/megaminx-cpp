#include "DBCache.h"
#include <string>
#include "sqlite3.h"
#include <stdexcept>

namespace Utils {

  DBCache::DBCache(const std::string& filename)
    : m_insert_statement(0)
  {
    // Initialise the database connection
    sqlite3_open(
      filename.c_str(),
      &m_db
    );
    // Create the table and index
    create_cache_table();
  }

  DBCache::~DBCache()
  {
    // Close the database connection
    sqlite3_close(m_db);
  }

  void DBCache::execute_statement(const std::string& statement)
  {
    // Prepare the statement
    sqlite3_stmt* stmt;
    const char* tail;
    sqlite3_prepare_v2(m_db, statement.c_str(), (int)statement.size(), &stmt, &tail);

    // Execute the statement
    int result = sqlite3_step(stmt);

    // Dispose of the statement
    sqlite3_finalize(stmt);

    // Check result
    if (result != SQLITE_DONE) {
      std::string message("Unexpected return value from executing statement: ");
      throw std::runtime_error(message + std::to_string(result) + statement);
    }
  }

  void DBCache::create_cache_table()
  {
    execute_statement("CREATE TABLE IF NOT EXISTS cache (key TEXT, value TEXT);");
    execute_statement("CREATE UNIQUE INDEX IF NOT EXISTS cache_keys ON cache (key);");
  }

  void DBCache::set(const std::string& key, const std::string& value)
  {
    // create the insert statement if it does not exist
    if (!m_insert_statement) {
      std::string sql = "INSERT INTO cache (key,value) VALUES (?,?);";
      const char* tail;
      sqlite3_prepare_v2(m_db, sql.c_str(), sql.size(), &m_insert_statement, &tail);
    } else {
      // Reset it if it does
      sqlite3_reset(m_insert_statement);
    }

    // Bind the values
    sqlite3_bind_text(m_insert_statement, 1, key.c_str(), (int)key.size(), SQLITE_STATIC);
    sqlite3_bind_text(m_insert_statement, 2, value.c_str(), (int)value.size(), SQLITE_STATIC);

    // Execute the statement
    int result = sqlite3_step(m_insert_statement);

    // Don't dispose of the statement as we can reuse it!

    // Check result
    if (result != SQLITE_DONE) {
      std::string message("Unexpected return value from inserting into cache: ");
      throw std::runtime_error(message + std::to_string(result));
    }

  }

}
