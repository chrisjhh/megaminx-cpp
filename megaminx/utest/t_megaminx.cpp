#include <gtest/gtest.h>
#include "face.h"
#include "megaminx.h"
#include "exceptions.h"
#include <memory>

TEST(MegaminxTest,Constructor)
{
  Megaminx::Megaminx m;
  EXPECT_EQ(m.face(0)->colour(), 'w');
}
