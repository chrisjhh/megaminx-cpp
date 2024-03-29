#include <gtest/gtest.h>
#include "face.h"
#include "exceptions.h"
#include <memory>

#define EXPECT_ARRAY_EQ(array1,array2) \
{\
  EXPECT_EQ((array1).size(),(array2).size());\
  for (int i=0;i<(array1).size();++i) {\
    EXPECT_EQ((array1)[i],(array2)[i]);\
  }\
}


TEST(FaceTest,Constructor)
{
  Megaminx::Face f('w');
  EXPECT_EQ(f.str(), "[w]");
  EXPECT_EQ(f.colour(), 'w');
}

TEST(FaceTest,become)
{
  Megaminx::Face f1('w');
  Megaminx::Face f2(f1);
  EXPECT_EQ(f2.str(), "[w]");
  Megaminx::Face f3('r');
  f1 = f3;
  EXPECT_EQ(f1.str(), "[r]");
  Megaminx::Face f4 = f2;
  EXPECT_EQ(f4.str(), "[w]");
  f4.set_facet(3,'x');
  EXPECT_EQ(f2.str(), "[w]");
  f2 = f4;
  EXPECT_EQ(f2.str(), "wwwxwwwwww");
  f1.become(f2);
  EXPECT_EQ(f1.str(), "wwwxwwwwww");
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

TEST(FaceTest,rotate_with_connections)
{
  // Make the face with all connections
  auto f = std::make_shared<Megaminx::Face>('x');
  std::vector<std::shared_ptr<Megaminx::Face>> store;
  char c = 'a';
  for(int i=0;i<10;++i) {
    f->set_facet(i,c+i);
  }
  c = '1';
  for (int i=0;i<5;++i) {
    auto face = std::make_shared<Megaminx::Face>(c+i);
    // Add it to the store to stop it getting deleted as connections are weak
    store.push_back(face);
    f->connect(i,face);
    face->connect(0,f);
  }

  f->rotate_anticlockwise();
  ASSERT_EQ(f->str(), "cdefghijab");
  ASSERT_EQ(f->connected_face(0)->str(), "2221111111");
  ASSERT_EQ(f->connected_face(1)->str(), "3332222222");
  ASSERT_EQ(f->connected_face(2)->str(), "4443333333");
  ASSERT_EQ(f->connected_face(3)->str(), "5554444444");
  ASSERT_EQ(f->connected_face(4)->str(), "1115555555");
  f->rotate_clockwise();
  ASSERT_EQ(f->str(), "abcdefghij");
  ASSERT_EQ(f->connected_face(0)->str(), "[1]");
  ASSERT_EQ(f->connected_face(1)->str(), "[2]");
  ASSERT_EQ(f->connected_face(2)->str(), "[3]");
  ASSERT_EQ(f->connected_face(3)->str(), "[4]");
  ASSERT_EQ(f->connected_face(4)->str(), "[5]");
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
  EXPECT_ARRAY_EQ(*e,expected);
  e = f.edge_facets(1);
  expected = {'c','d','e'};
  EXPECT_ARRAY_EQ(*e,expected);
  e = f.edge_facets(2);
  expected = {'e','f','g'};
  EXPECT_ARRAY_EQ(*e,expected);
  e = f.edge_facets(3);
  expected = {'g','h','i'};
  EXPECT_ARRAY_EQ(*e,expected);
  e = f.edge_facets(4);
  expected = {'i','j','a'};
  EXPECT_ARRAY_EQ(*e,expected);
}

TEST(FaceTest,set_edge_facets)
{
  Megaminx::Face f('x');
  char c = 'a';
  for(int i=0;i<10;++i) {
    f.set_facet(i,c+i);
  }
  EXPECT_EQ(f.str(),"abcdefghij");
  std::array<char,3> facets = {'1','2','3'};
  f.set_edge_facets(0,facets);
  EXPECT_EQ(f.str(),"123defghij");
  facets = {'4','5','6'};
  f.set_edge_facets(1,facets);
  EXPECT_EQ(f.str(),"12456fghij");
  facets = {'7','8','9'};
  f.set_edge_facets(2,facets);
  EXPECT_EQ(f.str(),"1245789hij");
  facets = {'A','B','C'};
  f.set_edge_facets(3,facets);
  EXPECT_EQ(f.str(),"124578ABCj");
  facets = {'D','E','F'};
  f.set_edge_facets(4,facets);
  EXPECT_EQ(f.str(),"F24578ABDE");
  f.set_edge_facets(0,*f.edge_facets(4));
  EXPECT_EQ(f.str(),"DEF578ABDE");
}

TEST(FaceTest,connecting_edge)
{
  auto f1 = std::make_shared<Megaminx::Face>('x');
  EXPECT_THROW(f1->connecting_edge('w'),Megaminx::connection_error);
  auto f2 = std::make_shared<Megaminx::Face>('w');
  f1->connect(0,f2);
  EXPECT_EQ(f1->connecting_edge('w'),0);
  EXPECT_THROW(f2->connecting_edge('x'),Megaminx::connection_error);
  f2->connect(2,f1);
  EXPECT_EQ(f2->connecting_edge('x'),2);
}

TEST(FaceTest,connected_edge_facets)
{
  auto f1 = std::make_shared<Megaminx::Face>('x');
  char c = 'a';
  for(int i=0;i<10;++i) {
    f1->set_facet(i,c+i);
  }
  EXPECT_THROW(f1->connected_edge_facets(0),Megaminx::connection_error);
  auto f2 = std::make_shared<Megaminx::Face>('x');
  c = 'A';
  for(int i=0;i<10;++i) {
    f2->set_facet(i,c+i);
  }
  f1->connect(0,f2);
  EXPECT_THROW(f1->connected_edge_facets(0),Megaminx::connection_error);
  f2->connect(1,f1);
  std::array<char,3> expected = {'C','D','E'};
  EXPECT_ARRAY_EQ(*f1->connected_edge_facets(0),expected);
  expected = {'a','b','c'};
  EXPECT_ARRAY_EQ(*f2->connected_edge_facets(1),expected);
}

TEST(FaceTest,set_connected_edge_facets)
{
  auto f1 = std::make_shared<Megaminx::Face>('x');
  char c = 'a';
  for(int i=0;i<10;++i) {
    f1->set_facet(i,c+i);
  }
  std::array<char,3> facets = {'1','2','3'};
  EXPECT_THROW(f1->set_connected_edge_facets(0,facets),Megaminx::connection_error);
  auto f2 = std::make_shared<Megaminx::Face>('x');
  c = 'A';
  for(int i=0;i<10;++i) {
    f2->set_facet(i,c+i);
  }
  f1->connect(0,f2);
  EXPECT_THROW(f1->set_connected_edge_facets(0,facets),Megaminx::connection_error);
  f2->connect(1,f1);
  f1->set_connected_edge_facets(0,facets);
  EXPECT_EQ(f2->str(), "AB123FGHIJ");
  f2->set_connected_edge_facets(1,facets);
  EXPECT_EQ(f1->str(), "123defghij");
}

TEST(FaceTest,parse)
{
  Megaminx::Face f('x');

  f.parse("[w]");
  EXPECT_EQ(f.str(),"[w]");
  std::string test = "abcdefghij";
  f.parse(test);
  EXPECT_EQ(f.str(),test);
  EXPECT_THROW(f.parse("[a)"),Megaminx::parse_error);
  EXPECT_THROW(f.parse("a"),Megaminx::parse_error);
  EXPECT_THROW(f.parse("abcdefghijk"),Megaminx::parse_error);
}

TEST(FaceTest,opposite_face)
{
  auto top = std::make_shared<Megaminx::Face>('1');
  EXPECT_THROW(top->opposite_face(),Megaminx::connection_error);
  auto topside = std::make_shared<Megaminx::Face>('2');
  top->connect(0,topside);
  EXPECT_THROW(top->opposite_face(),Megaminx::connection_error);
  topside->connect(0,top);
  EXPECT_THROW(top->opposite_face(),Megaminx::connection_error);
  auto bottomside = std::make_shared<Megaminx::Face>('3');
  topside->connect(2,bottomside);
  EXPECT_THROW(top->opposite_face(),Megaminx::connection_error);
  bottomside->connect(0,topside);
  EXPECT_THROW(top->opposite_face(),Megaminx::connection_error);
  auto bottom = std::make_shared<Megaminx::Face>('4');
  bottomside->connect(3,bottom);
  EXPECT_EQ(top->opposite_face()->colour(),'4');
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

TEST(FaceDeathTest,set_edge_facets)
{
  Megaminx::Face f('w');
  std::array<char,3> facets = {'1','2','3'};
  EXPECT_DEATH(f.set_edge_facets(5,facets), "Assertion failed");
  EXPECT_DEATH(f.set_edge_facets(-1,facets), "Assertion failed");
}

TEST(FaceDeathTest,connected_edge_facets)
{
  Megaminx::Face f('w');
  EXPECT_DEATH(f.connected_edge_facets(5), "Assertion failed");
  EXPECT_DEATH(f.connected_edge_facets(-1), "Assertion failed");
}

TEST(FaceDeathTest,set_connected_edge_facets)
{
  Megaminx::Face f('w');
  std::array<char,3> facets = {'1','2','3'};
  EXPECT_DEATH(f.set_connected_edge_facets(5,facets), "Assertion failed");
  EXPECT_DEATH(f.set_connected_edge_facets(-1,facets), "Assertion failed");
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
