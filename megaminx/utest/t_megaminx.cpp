#include <gtest/gtest.h>
#include "face.h"
#include "megaminx.h"
#include "exceptions.h"
#include <memory>

TEST(MegaminxTest,Constructor)
{
  Megaminx::Megaminx m;
  EXPECT_EQ(m.face(0)->colour(), 'w');
  EXPECT_EQ(m.face(0)->opposite_face()->colour(), 'x');
  EXPECT_EQ(m.face(0)->connected_face(0)->colour(), 'r');
}

TEST(MegaminxTest,faces) {
  Megaminx::Megaminx m;

  // Check faces returned by index and colour return the same face
  EXPECT_EQ(m.face(0).get(),m.face('w').get());
  EXPECT_EQ(m.face(1).get(),m.face('r').get());
  EXPECT_EQ(m.face(2).get(),m.face('G').get());
  EXPECT_EQ(m.face(3).get(),m.face('p').get());
  EXPECT_EQ(m.face(4).get(),m.face('Y').get());
  EXPECT_EQ(m.face(5).get(),m.face('B').get());
  EXPECT_EQ(m.face(6).get(),m.face('x').get());
  EXPECT_EQ(m.face(7).get(),m.face('y').get());
  EXPECT_EQ(m.face(8).get(),m.face('k').get());
  EXPECT_EQ(m.face(9).get(),m.face('g').get());
  EXPECT_EQ(m.face(10).get(),m.face('o').get());
  EXPECT_EQ(m.face(11).get(),m.face('b').get());

  EXPECT_THROW(m.face('j'),Megaminx::face_not_found);
}

TEST(MegaminxTest,connections) {
  // Check the megaminx is wired up right
  Megaminx::Megaminx m;
  EXPECT_EQ(m.face('w')->opposite_face()->colour(), 'x');
  EXPECT_EQ(m.face('x')->opposite_face()->colour(), 'w');
  EXPECT_EQ(m.face('r')->opposite_face()->colour(), 'o');
  EXPECT_EQ(m.face('o')->opposite_face()->colour(), 'r');
  EXPECT_EQ(m.face('Y')->opposite_face()->colour(), 'y');
  EXPECT_EQ(m.face('y')->opposite_face()->colour(), 'Y');
  EXPECT_EQ(m.face('g')->opposite_face()->colour(), 'G');
  EXPECT_EQ(m.face('G')->opposite_face()->colour(), 'g');
  EXPECT_EQ(m.face('B')->opposite_face()->colour(), 'b');
  EXPECT_EQ(m.face('b')->opposite_face()->colour(), 'B');
  EXPECT_EQ(m.face('p')->opposite_face()->colour(), 'k');
  EXPECT_EQ(m.face('k')->opposite_face()->colour(), 'p');
}

TEST(MegaminxTest,str)
{
  Megaminx::Megaminx m;
  EXPECT_EQ(m.str(), Megaminx::solved);
  m.face('w')->rotate_clockwise();
  EXPECT_EQ(m.str(), "[w]rrBBBrrrrrrrrGGGGGGGppppppGGGpYYYYpppYYYYBBBBBBBYY[x][y][k][g][o][b]");
}

TEST(MegaminxTest,parse)
{
  Megaminx::Megaminx m;
  std::string test = "[w]rrBBBrrrrrrrrGGGGGGGppppppGGGpYYYYpppYYYYBBBBBBBYY[x][y][k][g][o][b]";
  m.parse(test);
  EXPECT_EQ(m.str(), test);
  m.face('w')->rotate_anticlockwise();
  EXPECT_EQ(m.str(), Megaminx::solved);
  m.face('w')->rotate_clockwise();
  m.parse(Megaminx::solved);
  EXPECT_EQ(m.str(), Megaminx::solved);
}

//----- Death Tests
TEST(MegaminxDeathTest,faces)
{
  Megaminx::Megaminx m;
  EXPECT_DEATH(m.face(-1), "Assertion failed");
  EXPECT_DEATH(m.face(12), "Assertion failed");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
