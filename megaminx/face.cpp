#include "face.h"
#include <assert.h>

namespace Megaminx
{
  // Constructor
  Face::Face(char c) 
  {
    m_colour = c;
    m_facets.fill(c);
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
    assert(i >= 0 && i < 10);
    m_facets[i] = c;
  }

  // Connect this face to another face
  // Does not add reverse connection
  // There must not already be a connected face at this edge
  void Face::connect(int edge, std::shared_ptr<Face> face)
  {
    assert(edge >= 0 && edge < 10);
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
  }


}

