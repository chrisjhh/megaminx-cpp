#include "face.h"
#include <assert.h>
#include <algorithm>
#include "exceptions.h"
//#include <iostream>

namespace Megaminx
{
  // Constructor
  Face::Face(char c) 
  {
    m_colour = c;
    m_facets.fill(c);
  }

  // Copy constructor
  Face::Face(const Face& other)
  {
    become(other);
  }

  // Assignment operator
  Face& Face::operator=(const Face& other)
  {
    become(other);
    return *this;
  }

  // Return a string representation of the face
  std::string Face::str() const 
  {
    if (!m_cached_state.empty()) {
      return m_cached_state;
    }
    std::string result("");
    result.reserve(10);
    bool different = false;
    for(int i=0;i<10;++i) {
      if (!different && i > 0) {
        different = m_facets[i] != m_facets[i-1];
      }
      result += m_facets[i];
    }
    if (!different) {
      // All facets are the same. Use shorthand notation
      result = '[';
      result += m_facets[0];
      result += ']';
    }
    // Cache result
    m_cached_state = result;
    return result;
  }

  // Return the colour of the face
  char Face::colour() const
  {
    return m_colour;
  }

  // Get an individual facet of the face
  char Face::facet(int i) const
  {
    assert(i >= 0 && i < 10);
    return m_facets[i];
  }

  // Set an individual facet of the face
  void Face::set_facet(int i, char c)
  {
    // Clear the cached state
    m_cached_state.clear();

    assert(i >= 0 && i < 10);
    m_facets[i] = c;
  }

  // Connect this face to another face
  // Does not add reverse connection
  // There must not already be a connected face at this edge
  void Face::connect(int edge, std::shared_ptr<Face> face)
  {
    assert(edge >= 0 && edge < 5);
    assert(!m_connected_faces[edge].lock());
    m_connected_faces[edge] = face;
  }

  // Return the connected face at an edge
  std::shared_ptr<Face> Face::connected_face(int edge) const
  {
    assert(edge >= 0 && edge < 10);
    return m_connected_faces[edge].lock();
  }

  // Rotate the face anticlockwise
  void Face::rotate_anticlockwise()
  {
    // Clear the cached state
    m_cached_state.clear();

    // Grab the top two facets
    std::array<char,2> top_two;
    std::copy(m_facets.begin(),m_facets.begin()+2,top_two.begin()); 

    // Shift rest of array up
    std::copy(m_facets.begin()+2,m_facets.end(),m_facets.begin());

    // Insert top two at bottom
    std::copy(top_two.begin(),top_two.end(),m_facets.end()-2);

    // If we have connected faces rotate them too
    if (std::all_of(
      m_connected_faces.begin(), 
      m_connected_faces.end(),
      [](std::weak_ptr<Face> f) { return f.lock() != nullptr; } 
    )) {
      auto after = connected_edge_facets(0);
      for (int i=4;i>=0;--i) {
        auto safe = connected_edge_facets(i);
        set_connected_edge_facets(i,*after);
        after.reset(safe.release());
      }
    }
  }

  // Rotate the face clockwise
  void Face::rotate_clockwise()
  {
    // Clear the cached state
    m_cached_state.clear();

    // Grab the bottom two facets
    std::array<char,2> bottom_two;
    std::copy(m_facets.end()-2,m_facets.end(),bottom_two.begin()); 

    // Shift rest of array down
    std::copy_backward(m_facets.begin(),m_facets.end()-2,m_facets.end());

    // Insert bottom two at top
    std::copy(bottom_two.begin(),bottom_two.end(),m_facets.begin());

    // If we have connected faces rotate them too
    if (std::all_of(
      m_connected_faces.begin(), 
      m_connected_faces.end(),
      [](std::weak_ptr<Face> f) { return f.lock() != nullptr; } 
    )) {
      auto before = connected_edge_facets(4);
      for (int i=0;i<5;++i) {
        auto safe = connected_edge_facets(i);
        set_connected_edge_facets(i,*before);
        before.reset(safe.release());
      }
    }
  }

  void Face::rotate_corner_clockwise(int corner)
  {
    rotate_corner(corner, true);
  }

  void Face::rotate_corner_anticlockwise(int corner)
  {
    rotate_corner(corner, false);
  }

  void Face::rotate_corner(int corner, bool clockwise)
  {
    // Clear the cached state
    m_cached_state.clear();
    
    assert(corner >=0 && corner < 5);
    int facet_index = corner * 2;
    int edge_before = corner - 1;
    if (edge_before < 0) {
      edge_before += 5;
    }
    int edge_after = corner;
    char top = m_facets[facet_index];
    std::unique_ptr<std::array<char,3>> before_side = 
      connected_edge_facets(edge_before);
    std::unique_ptr<std::array<char,3>> after_side = 
      connected_edge_facets(edge_after);
    char before = (*before_side)[0];
    char after = (*after_side)[2];
    if (clockwise) {
      // Rotate clockwise top -> before -> after -> top
      char safe = before;
      before = top;
      top = after;
      after = safe;
    } else {
      // Rotate anticlockwise top -> after -> before -> top
      char safe = after;
      after = top;
      top = before;
      before = safe;
    }
    // Put them back
    (*before_side)[0] = before;
    set_connected_edge_facets(edge_before, *before_side);
    (*after_side)[2] = after;
    set_connected_edge_facets(edge_after, *after_side);
    m_facets[facet_index] = top;
  }

  

  // Return the facets along an edge
  std::unique_ptr<std::array<char,3>> Face::edge_facets(int edge) const
  {
    assert(edge >= 0 && edge < 5);
    std::unique_ptr<std::array<char,3>> facets(new std::array<char,3>());
    int offset = edge * 2;
    int end_offset = offset + 3;
    if (end_offset > 10) {
      end_offset = 10;
    }
    std::copy(m_facets.begin()+offset,m_facets.begin()+end_offset,facets->begin());
    if (edge == 4) {
      // Last facet on the last edge is the first facet on the first edge
      (*facets)[2] = m_facets[0];
    }
    return facets;
  }

  // Set the facets along an edge
  void Face::set_edge_facets(int edge, const std::array<char,3>& facets) {
    assert(edge >= 0 && edge < 5);

    // Clear the cached state
    m_cached_state.clear();

    int offset = edge * 2;
    int len = (edge == 4) ? 2 : 3;
    std::copy(facets.begin(),facets.begin()+len,m_facets.begin()+offset);
    if (edge == 4) {
      // Setting last facet on the last edge is actually 
      // the first facet on the first edge
      m_facets[0] = facets[2];
    }
  }

  // Return the edge that is connected to the face of the given colour
  int Face::connecting_edge(char col) const
  {
    for (int i=0;i<m_connected_faces.size();++i) {
      auto face = m_connected_faces[i].lock();
      if (face && face->colour() == col) {
        return i;
      }
    }
    throw connection_error("No matching face found");
  }

  // Get the connected facets along an edge
  std::unique_ptr<std::array<char,3>> Face::connected_edge_facets(int edge) const
  {
    assert(edge >= 0 && edge < 5);
    auto face = connected_face(edge);
    if (!face) throw connection_error("No connecting face");
    int matching_edge = face->connecting_edge(m_colour);
    return face->edge_facets(matching_edge);
  }

  void Face::set_connected_edge_facets(int edge, const std::array<char,3>& facets)
  {
    assert(edge >= 0 && edge < 5);
    auto face = connected_face(edge);
    if (!face) throw connection_error("No connecting face");
    int matching_edge = face->connecting_edge(m_colour);
    face->set_edge_facets(matching_edge,facets);
  }

  void Face::parse(const std::string& string)
  {
    //std::cout << "parse(\"" << string << "\");" << std::endl; 
    // Clear the cached state
    m_cached_state.clear();

    if (string.size() == 3) {
      if (string[0] != '[' || string[2] != ']') {
        std::string message("Invalid string format for parse: ");
        throw parse_error(message + string);
      }
      m_facets.fill(string[1]);
    } else if (string.size() == 10) {
      std::copy(string.begin(),string.end(),m_facets.begin());
    } else {
      std::string message("Invalid string length for parse: ");
      throw parse_error(message + string);
    }
  }

  std::shared_ptr<Face> Face::opposite_face() const
  {
    auto topside_face = connected_face(0);
    if (!topside_face) throw connection_error("No connected face");
    int topside_edge = topside_face->connecting_edge(m_colour);
    int x = topside_edge + 2;
    if (x > 4) {
      x -= 5;
    }
    auto underside_face = topside_face->connected_face(x);
    if (!underside_face) throw connection_error("No connected face");
    int underside_edge = underside_face->connecting_edge(topside_face->colour());
    int y = underside_edge + 3;
    if (y > 4) {
      y -= 5;
    }
    auto op_face = underside_face->connected_face(y);
    if (!op_face) throw connection_error("No connected face");
    return op_face;
  }

  void Face::become(const Face& other) {
    m_colour = other.colour();
    std::copy(other.m_facets.begin(), other.m_facets.end(), m_facets.begin());
    m_cached_state = other.m_cached_state;
  }

}

