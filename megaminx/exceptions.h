#pragma once

#include <stdexcept>
#include <string>


namespace Megaminx {
  /**
   * Exception thrown if the megaminx faces are not connected as expected
   */
  class connection_error : public std::runtime_error {
    public:
      connection_error(const std::string m) : std::runtime_error(m) {}
      connection_error(const char* m) : std::runtime_error(m) {}
  };
}
