#include "megaminx.h"
#include "face.h"
#include <assert.h>
#include <string>
#include <memory>
#include "exceptions.h"

namespace Megaminx {

  // Globals
  const char* solved = "[w][r][G][p][Y][B][x][y][k][g][o][b]";
  const char colours[12] = {'w','r','G','p','Y','B','x','y','k','g','o','b'};
  extern const char connections[12][5] = {
    {'r','G','p','Y','B'},
    {'G','w','B','k','y'},
    {'w','r','y','b','p'},
    {'b','o','Y','w','G'},
    {'g','B','w','p','o'},
    {'Y','g','k','r','w'},
    {'y','k','g','o','b'},
    {'k','x','b','G','r'},
    {'x','y','r','B','g'},
    {'B','Y','o','x','k'},
    {'p','b','x','g','Y'},
    {'o','p','G','y','x'}
  };


  // Constructor
  Megaminx::Megaminx()
  {
    // Construct the faces
    for (int i=0;i<12;++i) {
      m_faces[i] = std::make_shared<Face>(colours[i]);
    }
    // Connect them up
    for (int i=0;i<12;++i) {
      for (int j=0;j<5;++j) {
        m_faces[i]->connect(j,face(connections[i][j]));
      }
    }
  }

  // Return face by index
  std::shared_ptr<Face> Megaminx::face(int i) const
  {
    assert(i >=0 && i < 12);
    return m_faces[i];
  }

  // Return face by colour
  std::shared_ptr<Face> Megaminx::face(char col) const
  {
    for (int i=0;i<12;++i) {
      if (m_faces[i]->colour() == col) {
        return m_faces[i];
      }
    }
    throw face_not_found(std::string("No such face with colour ") + col);
  }

  // Return string representation of the state
  std::string Megaminx::str() const
  {
    std::string result;
    result.reserve(120);
    std::for_each(
      m_faces.begin(),
      m_faces.end(),
      [&result](std::shared_ptr<Face> f) {result.append(f->str());}
    );
    return result;
  }

  void Megaminx::parse(const std::string& string)
  {
    auto pos = string.begin();
    for (int i=0; i<10; ++i) {
      if (*pos == '[') {
        m_faces[i]->parse(std::string(pos, pos+3));
        pos += 3;
      } else {
        m_faces[i]->parse(std::string(pos, pos+10));
        pos += 10;
      }
    }
  }
}
