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
