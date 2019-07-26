#pragma once

#include <string>
#include <memory>
#include <array>

namespace Megaminx {

  // Predeclarations
  class Face;

  // Globals
  extern const char* solved;
  extern const char colours[12];
  extern const char connections[12][5];

  /**
   * Class representing the megaminx puzzle
   */
  class Megaminx {
    public:
      // Constructor
      Megaminx();

      /**
       * Return a face of the puzzle by index
       * @param i The index 0 -> 11
       */
      std::shared_ptr<Face> face(int i) const;

      /**
       * Return a face of the puzzle by its colour
       * @param col the colour
       */
      std::shared_ptr<Face> face(char col) const;

      // Return string representation of the megaminx
      std::string str() const;

      // Parse the state of the megaminx from the string
      void parse(const std::string& string);

    protected:
    private:
      std::array<std::shared_ptr<Face>,12> m_faces;
  };

}
