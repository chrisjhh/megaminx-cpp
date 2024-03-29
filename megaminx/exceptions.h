#pragma once

#include <stdexcept>
#include <string>

#define DERIVED_EXCEPTION(TYPE) \
class TYPE : public std::runtime_error { \
    public: \
      TYPE(const std::string& m) : std::runtime_error(m) {} \
      TYPE(const char* m) : std::runtime_error(m) {} \
};


namespace Megaminx {

  // Exception thrown if the megaminx faces are not connected as expected
  DERIVED_EXCEPTION(connection_error);

  // Exception thrown if megaminx face fails to parse from string given
  DERIVED_EXCEPTION(parse_error);

  // Exception thrown for a face not found
  DERIVED_EXCEPTION(face_not_found);

  // Exception thrown if an invalid instruction is given
  DERIVED_EXCEPTION(invalid_instruction);
}
