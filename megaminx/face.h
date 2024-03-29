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

      // Copy constructor
      Face(const Face& other);

      // Asignment operator
      Face& operator=(const Face& other);

      // Return the 'colour' of the face
      char colour() const;

      // Return the string representation of this face
      std::string str() const;

      // Return a facet 0 <= i < 10
      char facet(int i) const;

      // Set a facet of the face 0 <= i < 10
      void set_facet(int i, char c);

      // Connect another face
      void connect(int edge, std::shared_ptr<Face> face);

      // Return the face connected at an edge
      std::shared_ptr<Face> connected_face(int edge) const;

      // Rotate this face clockwise
      void rotate_clockwise();

      // Rotate this face anticlockwise
      void rotate_anticlockwise();

      // These are not legal moves but can be useful for setting the cube up to solve
      void rotate_corner_clockwise(int corner);
      void rotate_corner_anticlockwise(int corner);

      // Get the facets along an edge
      std::unique_ptr<std::array<char,3>> edge_facets(int edge) const;

      /**
      * Set the facets along an edge
      * @param edge The edge to set 0 -> 4
      * @param facets The facets to set
      */
      void set_edge_facets(int edge, const std::array<char,3>& facets);

      /**
       * Get the connected facets along an edge
       * There must be a connected face at this edge
       * or a Megaminx::connection_error error will be throwm
       * @param edge the edge 0 -> 4
       */
      std::unique_ptr<std::array<char,3>> connected_edge_facets(int edge) const;

      /**
       * Set the connected facets along an edge
       * @param the edge with the connecting facets
       * @param the facets to set
       */
      void set_connected_edge_facets(int edge, const std::array<char,3>& facets);

      /**
       * Return the edge that is connected to the face of the given colour
       * Throws a Megaminx::connection_error if there is no match
       * @param col the colour to match
       */
      int connecting_edge(char col) const;

      // Return the face that is opposite to this one
      std::shared_ptr<Face> opposite_face() const;

      // Parse this face from the string representation
      void parse(const std::string& string);

      // Utility method for copy constructor and assignment operator
      void become(const Face& other);

    protected:
    private:
      void rotate_corner(int corner, bool clockwise);

      // The colour of this face
      char m_colour;

      // The array that holds the facets
      std::array<char,10> m_facets;

      // The array of connected faces
      std::array<std::weak_ptr<Face>,5> m_connected_faces;

      // A cache of the string representation
      mutable std::string m_cached_state;
  };
}
