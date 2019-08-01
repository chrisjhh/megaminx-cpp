#include <gtest/gtest.h>
#include "PersistentFilePagedQueue.h"
#include <string>
#include <sys/stat.h>
#include <stdio.h>
#include <filesystem>
#include <stdlib.h>
#include <time.h>
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

#define REPEAT(times,code) \
  for (int _i=0;_i<times;++_i) { \
    code; \
  }

std::string random_string(int len) 
{ 
  std::string result;
  result.reserve(len);
  for (int i=0;i<len;++i) {
    // Generate char from 32 (space) to 126 (~)
    int r = rand() % 95;
    char c = 32 + r;
    result += c;
  }
  return result;
}

TEST(PersistentFilePagedQueueTest,constructor)
{
  std::string dir = "t_PersistentFilePagedQueue_01";
  RMDIR(dir);
  MKDIR(dir);
  {
    Utils::PersistentFilePagedQueue<int> q(dir,"queue",3);
    EXPECT_EQ(q.size(), 0);
    EXPECT_TRUE(q.empty());
  }
  {
    Utils::PersistentFilePagedQueue<int> q(dir,"queue",3);
    EXPECT_EQ(q.size(), 0);
    EXPECT_TRUE(q.empty());
  }
  RMDIR(dir);
}

TEST(PersistentFilePagedQueueTest,push)
{
  std::string dir = "t_PersistentFilePagedQueue_02";
  RMDIR(dir);
  MKDIR(dir);
  {
    Utils::PersistentFilePagedQueue<int> q(dir,"queue",3);
    for(int i=1;i<=20;++i) {
      q.push(i);
      EXPECT_EQ(q.size(), i);
      EXPECT_TRUE(!q.empty());
    }
  }
  {
    Utils::PersistentFilePagedQueue<int> q(dir,"queue",3);
    EXPECT_EQ(q.size(), 20);
    EXPECT_TRUE(!q.empty());
  }
  RMDIR(dir);
}

TEST(PersistentFilePagedQueueTest,pop)
{
  std::string dir = "t_PersistentFilePagedQueue_03";
  RMDIR(dir);
  MKDIR(dir);
  {
    for(int n=1;n<=28;n+=3) {
      {
        Utils::PersistentFilePagedQueue<int> q(dir,"queue",3);
        for(int i=1;i<=n;++i) {
          q.push(i);
          EXPECT_EQ(q.size(), i);
          EXPECT_TRUE(!q.empty());
        }
      }
      {
        Utils::PersistentFilePagedQueue<int> q(dir,"queue",3);
        int expected = 1;
        for(int i=n;i>0;--i) {
          EXPECT_EQ(q.size(), i);
          EXPECT_EQ(q.front(), expected++);
          q.pop();
          if (i > 1) {
            EXPECT_TRUE(!q.empty());
          } else {
            EXPECT_TRUE(q.empty());
          }
        }
      }
    }
  }
  RMDIR(dir);
}

TEST(PersistentFilePagedQueueTest,paging)
{
  std::string dir = "t_PersistentFilePagedQueue_04";
  RMDIR(dir);
  MKDIR(dir);
  {
    Utils::PersistentFilePagedQueue<int> q(dir,"queue",3);
    // If we push 9 items the first page file should appear
    REPEAT(9,q.push(0));
    q.syncronize();
    EXPECT_EXISTS("t_PersistentFilePagedQueue_04\\queue1.q");
  }
  {
    Utils::PersistentFilePagedQueue<int> q(dir,"queue",3);
    // Read three and it should disappear again
    REPEAT(3,q.pop());
    q.syncronize();
    EXPECT_NOT_EXISTS("t_PersistentFilePagedQueue_04\\queue1.q");
  }
  {
    Utils::PersistentFilePagedQueue<int> q(dir,"queue",3);
    // Push three and it should come back
    REPEAT(3,q.push(0));
    q.syncronize();
    EXPECT_EXISTS("t_PersistentFilePagedQueue_04\\queue1.q");
  }
  {
    Utils::PersistentFilePagedQueue<int> q(dir,"queue",3);
    // Push another three and second page file should appear
    REPEAT(3,q.push(0));
    q.syncronize();
    EXPECT_EXISTS("t_PersistentFilePagedQueue_04\\queue1.q");
    EXPECT_EXISTS("t_PersistentFilePagedQueue_04\\queue2.q");
  }
  {
    Utils::PersistentFilePagedQueue<int> q(dir,"queue",3);
    // Pop three and first page file should go but second remain
    REPEAT(3,q.pop());
    q.syncronize();
    EXPECT_NOT_EXISTS("t_PersistentFilePagedQueue_04\\queue1.q");
    EXPECT_EXISTS("t_PersistentFilePagedQueue_04\\queue2.q");
  }
  {
    Utils::PersistentFilePagedQueue<int> q(dir,"queue",3);
    // Push another 3 now and third file should appear but still not first
    REPEAT(3,q.push(0));
    q.syncronize();
    EXPECT_NOT_EXISTS("t_PersistentFilePagedQueue_04\\queue1.q");
    EXPECT_EXISTS("t_PersistentFilePagedQueue_04\\queue2.q");
    EXPECT_EXISTS("t_PersistentFilePagedQueue_04\\queue3.q");
  }
  {
    Utils::PersistentFilePagedQueue<int> q(dir,"queue",3);
    // Pop three and only third file should be left
    REPEAT(3,q.pop());
    q.syncronize();
    EXPECT_NOT_EXISTS("t_PersistentFilePagedQueue_04\\queue1.q");
    EXPECT_NOT_EXISTS("t_PersistentFilePagedQueue_04\\queue2.q");
    EXPECT_EXISTS("t_PersistentFilePagedQueue_04\\queue3.q");
  }
  {
    Utils::PersistentFilePagedQueue<int> q(dir,"queue",3);
    // Pop another three and they should be all gone
    REPEAT(3,q.pop());
    q.syncronize();
    EXPECT_NOT_EXISTS("t_PersistentFilePagedQueue_04\\queue1.q");
    EXPECT_NOT_EXISTS("t_PersistentFilePagedQueue_04\\queue2.q");
    EXPECT_NOT_EXISTS("t_PersistentFilePagedQueue_04\\queue3.q");
  }
  {
    Utils::PersistentFilePagedQueue<int> q(dir,"queue",3);
    // Push three now and counter should reset to one
    REPEAT(3,q.push(0));
    q.syncronize();
    EXPECT_EXISTS("t_PersistentFilePagedQueue_04\\queue1.q");
  }
  RMDIR(dir);
}

TEST(PersistentFilePagedQueueTest,randomExercise)
{
  // Simulate a typical use
  srand ((unsigned int)time(NULL));
  std::string dir = "t_PersistentFilePagedQueue_05";
  RMDIR(dir);
  MKDIR(dir);
  {
    std::queue<std::string> reference;
    for (int n=0;n<5;++n) {
      Utils::PersistentFilePagedQueue<std::string> q(dir,"queue",3);
      int limit = rand() % 500;
      for (int i=0;i<limit;++i) {
        if (q.empty() || (rand() % 3) > 0) {
          std::string next_item = random_string(5 + (rand() % 10));
          q.push(next_item);
          reference.push(next_item);
        } else {
          ASSERT_EQ(q.front(),reference.front());
          ASSERT_EQ(q.size(),reference.size());
          q.pop();
          reference.pop();
        }
      }
    }
  }
  RMDIR(dir);
}
