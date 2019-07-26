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
