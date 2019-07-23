#include <gtest/gtest.h>
#include "face.h"

TEST(FaceTest,Constructor)
{
  Megaminx::Face f('w');
  EXPECT_EQ(f.str(), "[w]");
  EXPECT_EQ(f.colour(), 'w');
}

TEST(FaceTest,Facets)
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


//----- Death Tests
TEST(FaceDeathTest,Facets)
{
  // Test invalid use cases
  Megaminx::Face f('w');
  EXPECT_DEATH(f.facet(10), "Assertion failed");
  EXPECT_DEATH(f.facet(-1), "Assertion failed");
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
