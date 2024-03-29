#include "DBCache.h"
#include <string>
#include "sqlite3.h"
#include <stdexcept>
#include <assert.h>
//#include <iostream>

namespace Utils {

  DBCache::DBCache(const std::string& filename)
    : m_insert_statement(nullptr),
      m_select_statement(nullptr),
      m_multi_select_statement(nullptr),
      m_last_multi_select_count(0)
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
    // Finalize any starements still in use
    if (m_insert_statement) {
      sqlite3_finalize(m_insert_statement);
    }
    if (m_select_statement) {
      sqlite3_finalize(m_select_statement);
    }
    if (m_multi_select_statement) {
      sqlite3_finalize(m_multi_select_statement);
    }
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

  std::string DBCache::get(const std::string& key)
  {
    // create the insert statement if it does not exist
    if (!m_select_statement) {
      std::string sql = "SELECT value FROM cache WHERE key = ?;";
      const char* tail;
      sqlite3_prepare_v2(m_db, sql.c_str(), sql.size(), &m_select_statement, &tail);
    } else {
      // Reset it if it does
      sqlite3_reset(m_select_statement);
    }

    // Bind the value
    sqlite3_bind_text(m_select_statement, 1, key.c_str(), (int)key.size(), SQLITE_STATIC);

    // Execute the statement
    int result = sqlite3_step(m_select_statement);
    if (result == SQLITE_DONE) {
      // Nothing found. Return empty string
      return std::string();
    }
    if (result != SQLITE_ROW) {
      std::string message("Unexpected return value from select statement: ");
      throw std::runtime_error(message + std::to_string(result));
    }
    const unsigned char* value_str = sqlite3_column_text(m_select_statement, 0);
    return std::string((const char*)value_str);
  }

  std::pair<std::string,std::string> DBCache::get_any(const std::set<std::string>& keys)
  {
    assert(!keys.empty());
    // Reuse previous statement if it is applicable
    if (m_multi_select_statement) {
      if (m_last_multi_select_count == keys.size()) {
        // Reuse
        //std::cout << "Reusing last statement" << std::endl;
        sqlite3_reset(m_multi_select_statement);
      } else {
        // Dispose
        sqlite3_finalize(m_multi_select_statement);
        m_multi_select_statement = nullptr;
      }
    }
    if (!m_multi_select_statement) {
      // Prepare or re-prepare
      std::string sql = "SELECT key,value FROM cache WHERE key IN (";
      for (int i=0; i< keys.size() - 1; ++i) {
        sql += "?,";
      }
      sql += "?) LIMIT 1;";
      //std::cout << "Preparing statement \"" << sql << "\"" << std::endl;
      const char* tail;
      sqlite3_prepare_v2(m_db, sql.c_str(), sql.size(), &m_multi_select_statement, &tail);
      m_last_multi_select_count = keys.size();
    }

    // Bind the values
    int index = 0;
    for (const std::string& key : keys) {
      ++index;
      //std::cout << "Binding key " << index << " to " << key << std::endl; 
      sqlite3_bind_text(
        m_multi_select_statement, 
        index, 
        key.c_str(), 
        (int)key.size(), 
        SQLITE_STATIC
      );
    }

    // Execute the statement
    int result = sqlite3_step(m_multi_select_statement);
    if (result == SQLITE_DONE) {
      // Nothing found. Return empty pair
      //std::cout << "Nothing found" << std::endl;
      return std::pair<std::string,std::string>();
    }
    if (result != SQLITE_ROW) {
      std::string message("Unexpected return value from select-any statement: ");
      throw std::runtime_error(message + std::to_string(result));
    }
    // Get the values from the returned row
    const unsigned char* key_str = sqlite3_column_text(m_multi_select_statement, 0);
    const unsigned char* value_str = sqlite3_column_text(m_multi_select_statement, 1);
    //std::cout << "returning pair <" << (const char*)key_str << "," << 
    // (const char*)value_str << ">" << std::endl; 
    // Return as a pair
    return std::pair<std::string,std::string>((const char*)key_str,(const char*)value_str);
  }

}
