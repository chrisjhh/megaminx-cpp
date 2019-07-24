#include <gtest/gtest.h>
#include "face.h"
#include <memory>

#define EXPECT_ARRAY_EQ(array1,array2) \
{\
  EXPECT_EQ(array1.size(),array2.size());\
  for (int i=0;i<array1.size();++i) {\
    EXPECT_EQ(array1[i],array2[i]);\
  }\
}


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

TEST(FaceTest,rotate)
{
  Megaminx::Face f('x');
  f.set_facet(0,'W');
  f.set_facet(1,'l');
  f.set_facet(8,'d');
  f.set_facet(9,'e');
  EXPECT_EQ(f.str(),"Wlxxxxxxde");
  f.rotate_clockwise();
  EXPECT_EQ(f.str(),"deWlxxxxxx");
  f.rotate_anticlockwise();
  EXPECT_EQ(f.str(),"Wlxxxxxxde");
  f.rotate_anticlockwise();
  EXPECT_EQ(f.str(),"xxxxxxdeWl");
  f.rotate_clockwise();
  EXPECT_EQ(f.str(),"Wlxxxxxxde");
}

TEST(FaceTest,edge_facets)
{
  Megaminx::Face f('x');
  char c = 'a';
  for(int i=0;i<10;++i) {
    f.set_facet(i,c+i);
  }
  EXPECT_EQ(f.str(),"abcdefghij");
  auto e = f.edge_facets(0);
  std::array<char,3> expected = {'a','b','c'};
  EXPECT_ARRAY_EQ((*e),expected);
  e = f.edge_facets(1);
  expected = {'c','d','e'};
  EXPECT_ARRAY_EQ((*e),expected);
  e = f.edge_facets(2);
  expected = {'e','f','g'};
  EXPECT_ARRAY_EQ((*e),expected);
  e = f.edge_facets(3);
  expected = {'g','h','i'};
  EXPECT_ARRAY_EQ((*e),expected);
  e = f.edge_facets(4);
  expected = {'i','j','a'};
  EXPECT_ARRAY_EQ((*e),expected);
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
  EXPECT_DEATH(f.connect(5, fptr), "Assertion failed");
  EXPECT_DEATH(f.connect(-1, fptr), "Assertion failed");
  f.connect(0,fptr);
  EXPECT_DEATH(f.connect(0, fptr), "Assertion failed");
}

TEST(FaceDeathTest,edge_facets)
{
  Megaminx::Face f('w');
  EXPECT_DEATH(f.edge_facets(5), "Assertion failed");
  EXPECT_DEATH(f.edge_facets(-1), "Assertion failed");
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
