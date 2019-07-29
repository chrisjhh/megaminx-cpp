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

      // Copy constructor
      Megaminx(const Megaminx& other);

      // Asignment operator
      Megaminx& operator=(const Megaminx& other);

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

      // Apply a list of instructions to the megaminx
      // inscrutions are in the form of colour + rotation [+number]
      // eg. "x> Y< G>2" for rotate grey clockwise, dark yellow anticlockwise
      // and green clockwise twice
      void apply(const std::string& instructions);

      // Utility function for asignment operator and copy constructor
      void become(const Megaminx& other);

    protected:
    private:
      // Utility to do the guts of construction
      void init();
      std::array<std::shared_ptr<Face>,12> m_faces;
  };

}
