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
}

