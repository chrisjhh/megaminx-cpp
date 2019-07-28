#pragma once

#include <string>
#include <set>
#include <utility>

namespace Utils {
  class DBCache {
    public:
      // Set a value in the cache by key
      void set(const std::string& key, const std::string& value);

      // Get a value from the cache by key
      std::string get(const std::string& key) const;

      // Get first key-value pair found from a set of keys
      std::pair<std::string,std::string> get_any(const std::set<std::string>& keys);
    protected:
    private:
  };
}