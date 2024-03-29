#include "megaminx.h"
#include "face.h"
#include <assert.h>
#include <string>
#include <memory>
#include "exceptions.h"
#include <cctype>

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
    init();
  }

  void Megaminx::init()
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

  // Copy constructor
  Megaminx::Megaminx(const Megaminx& other)
  {
    init();
    become(other);
  }

  // Asignment operator
  Megaminx& Megaminx::operator=(const Megaminx& other)
  {
    become(other);
    return *this;
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
    for (int i=0; i<12; ++i) {
      if (pos == string.end()) {
        throw parse_error("String does not contain all faces");
      }
      if (*pos == '[') {
        if (std::distance(pos,string.end()) < 3) {
          throw parse_error("String not long enough");
        }
        m_faces[i]->parse(std::string(pos, pos+3));
        pos += 3;
      } else {
        if (std::distance(pos,string.end()) < 10) {
          throw parse_error("String not long enough");
        }
        m_faces[i]->parse(std::string(pos, pos+10));
        pos += 10;
      }
    }
  }

  void Megaminx::apply(const std::string& instructions)
  {
    auto pos = instructions.begin();
    auto end = instructions.end();
    while(pos != end) {
      char col = *pos;
      std::shared_ptr<Face> f;
      try {
        f = face(col);
      } catch (face_not_found e) {
        throw invalid_instruction(e.what());
      }
      ++pos;
      if (pos == end) {
        throw invalid_instruction("No direction given");
      }
      char dir = *pos;
      if (dir != '<' && dir != '>') {
        throw invalid_instruction(std::string("Invalid direction given") + dir);
      }
      ++pos;
      // See if there is a number
      int num = 1;
      if (pos != end && std::isdigit(*pos)) {
        num = *pos - '0';
        assert(num >= 0 && num <= 9);
        if (num == 0) {
          throw invalid_instruction("Zero is not a valid number of moves");
        }
        ++pos;
      }
      for (int i=0; i<num; ++i) {
        if (dir == '>') {
          f->rotate_clockwise();
        } else {
          f->rotate_anticlockwise();
        }
      }
      // Iterate past any spaces
      while (pos != end && *pos == ' ') {
        ++pos;
      }
    }
  }

  void Megaminx::become(const Megaminx& other)
  {
    for (int i=0;i<12;++i) {
      m_faces[i]->become(*other.m_faces[i]);
    }
  }
}
