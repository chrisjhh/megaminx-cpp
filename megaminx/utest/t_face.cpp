#include <gtest/gtest.h>
#include "face.h"
#include <memory>

TEST(FaceTest,Constructor)
{
  Megaminx::Face f('w');
  EXPECT_EQ(f.str(), "[w]");
  EXPECT_EQ(f.colour(), 'w');
}

TEST(FaceTest,facets)
{
  Megaminx::Face f('w');
  EXPECT_EQ(f.facet(0),'w');
  f.set_facet(0,'x');
  EXPECT_EQ(f.facet(0),'x');
  EXPECT_EQ(f.facet(9),'w');
  f.set_facet(9,'r');
  EXPECT_EQ(f.facet(9),'r');
  EXPECT_EQ(f.str(), "xwwwwwwwwr");
}

TEST(FaceTest,connect)
{
  Megaminx::Face f('w');
  EXPECT_EQ(f.connected_face(0), nullptr);
  std::shared_ptr<Megaminx::Face> fptr(new Megaminx::Face('r'));
  f.connect(0, fptr);
  ASSERT_NE(f.connected_face(0), nullptr);
  EXPECT_EQ(f.connected_face(0)->colour(), 'r');
  // As connected faces stored as weak refs they should release
  // memory when external memory is released
  fptr.reset();
  EXPECT_EQ(fptr, nullptr);
  EXPECT_EQ(f.connected_face(0), nullptr);
}


//----- Death Tests
TEST(FaceDeathTest,facets)
{
  // Test invalid use cases
  Megaminx::Face f('w');
  EXPECT_DEATH(f.facet(10), "Assertion failed");
  EXPECT_DEATH(f.facet(-1), "Assertion failed");
}

TEST(FaceDeathTest,connect)
{
  Megaminx::Face f('w');
  std::shared_ptr<Megaminx::Face> fptr(new Megaminx::Face('r'));
  EXPECT_DEATH(f.connect(10, fptr), "Assertion failed");
  EXPECT_DEATH(f.connect(-1, fptr), "Assertion failed");
  f.connect(0,fptr);
  EXPECT_DEATH(f.connect(0, fptr), "Assertion failed");
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
