#pragma once

#include <string>
#include <set>
#include <utility>
#include "sqlite3.h"

namespace Utils {
  class DBCache {
    public:
      DBCache(const std::string& filename);

      // Set a value in the cache by key
      void set(const std::string& key, const std::string& value);

      // Get a value from the cache by key
      std::string get(const std::string& key);

      // Get first key-value pair found from a set of keys
      std::pair<std::string,std::string> get_any(const std::set<std::string>& keys);

      // Virtual destructor
      virtual ~DBCache(); 
    protected:
    private:
      // Execute an sql statement which does not return any values
      void execute_statement(const std::string& statement);

      // Create the cache table and set up index on keys
      void create_cache_table();

      sqlite3* m_db;
      sqlite3_stmt* m_insert_statement;
      sqlite3_stmt* m_select_statement;
  };
}
