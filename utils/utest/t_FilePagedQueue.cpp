#include <gtest/gtest.h>
#include "FilePagedQueue.h"
#include <string>
#include <sys/stat.h>
#include <stdio.h>
#include <filesystem>
namespace fs = std::experimental::filesystem;


#define EXPECT_EXISTS(filename) \
  EXPECT_EQ(fs::exists(filename), true)

#define EXPECT_NOT_EXISTS(filename) \
  EXPECT_EQ(fs::exists(filename), false);

#define DELETE_IF_EXISTS(filename) \
  if (fs::exists(filename)) fs::remove(filename);

#define MKDIR(dirname) \
  fs::create_directory(dirname)

#define RMDIR(directory) \
  fs::remove_all(directory)

#define TEMP \
  fs::temp_directory_path()

TEST(FilePagedQueueTest,constructor)
{
  std::string dir = "t_FilePagedCache_01";
  MKDIR(dir);
  Utils::FilePagedQueue<int> q(dir,"queue",3);
  EXPECT_EQ(q.size(), 0);
  EXPECT_TRUE(q.empty());
  RMDIR(dir);
}
