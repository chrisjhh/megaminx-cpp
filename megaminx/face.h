#pragma once

#include <string>
#include <array>
#include <memory>

namespace Megaminx {
  class Face {
    public:
      // Constructor
      // Initialise the array of facets with the given char
      Face(char c);

      // Return the string representation of this face
      std::string str() const;

      // Connect another face
      void connect(std::shared_ptr<Face> face, int edge);

      // Rotate this face clockwise
      void rotate_clockwise();

      // Rotate this face anticlockwise
      void rotate_anticlockwise();

      // Get the facets along an edge
      std::unique_ptr<std::array<char,3>> edge_facets(int edge) const;

      // Set the facets along an edge
      void set_edge_facets(int edge, const std::array<char,3>& facets);

      // Get the connected facets along an edge
      std::unique_ptr<std::array<char,3>> connected_edge_facets(int edge) const;

      // Set the connected facets along an edge
      void set_connected_edge_facets(int edge, const std::array<char,3>& facets);

      // Return the edge that is connected to the face of the given colour
      int connecting_edge(char col) const;

      // Return the face that is opposite to this one
      std::shared_ptr<Face> opposite_face() const;

      // Parse this face from the string representation
      void parse(const std::string& string);


    protected:
    private:
      // The array that holds the facets
      std::array<char,10> m_facets;

      // The array of connected faces
      std::array<std::weak_ptr<Face>,5> m_connected_faces;
  };
}
