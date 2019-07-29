#include "DBCache.h"
#include <string>
#include "sqlite3.h"
#include <stdexcept>

namespace Utils {

  DBCache::DBCache(const std::string& filename)
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
    sqlite3_prepare_v2(m_db, statement.c_str(), statement.size(), &stmt, &tail);

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
    execute_statement("CREATE INDEX IF NOT EXISTS cache_keys ON cache (key);");
  }

}
