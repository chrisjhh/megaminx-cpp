#include <gtest/gtest.h>
#include "DBCache.h"
#include <sys/stat.h>
#include <stdio.h>

static bool exists(std::string filename)
{
  struct stat buffer; \
  bool exists = stat (filename.c_str(), &buffer) == 0;
  return exists;
}

#define EXPECT_EXISTS(filename) \
{ \
  EXPECT_EQ(exists(filename), true); \
}

#define EXPECT_NOT_EXISTS(filename) \
{ \
  EXPECT_EQ(exists(filename), false); \
}

#define DELETE_IF_EXISTS(filename) \
{ \
  if (exists(filename)) { \
    remove(filename.c_str()); \
  } \
}

TEST(DBCacheTest,Connect)
{
  std::string file = "./t_dbcache_01.db";
  DELETE_IF_EXISTS(file);
  EXPECT_NOT_EXISTS(file);

  {
    Utils::DBCache cache(file);
  }
  EXPECT_EXISTS(file);
  DELETE_IF_EXISTS(file);
}

TEST(DBCacheTest,SetAndGet)
{
  std::string file = "./t_dbcache_02.db";
  DELETE_IF_EXISTS(file);
  EXPECT_NOT_EXISTS(file);
  
  {
    Utils::DBCache cache(file);

    cache.set("name", "Chris");
    cache.set("age", "48");

    EXPECT_EQ(cache.get("name"), "Chris");
    EXPECT_EQ(cache.get("age"), "48");
    EXPECT_EQ(cache.get("wibble"), "");
  }
  DELETE_IF_EXISTS(file);
}

TEST(DBCacheTest,get_any)
{
  std::string file = "./t_dbcache_03.db";
  DELETE_IF_EXISTS(file);
  EXPECT_NOT_EXISTS(file);

  {
    Utils::DBCache cache(file);

    cache.set("name", "Chris");
    cache.set("age", "48");

    std::set<std::string> keys;
    keys.insert("name");
    keys.insert("wibble");
    auto result = cache.get_any(keys);
    EXPECT_EQ(result.first, "name");
    EXPECT_EQ(result.second, "Chris");

    keys.clear();
    keys.insert("age");
    keys.insert("fish");
    result = cache.get_any(keys);
    EXPECT_EQ(result.first, "age");
    EXPECT_EQ(result.second, "48");

    keys.clear();
    keys.insert("age");
    keys.insert("name");
    result = cache.get_any(keys);
    EXPECT_TRUE(result.first == "name" || result.first == "age");
    if (result.first == "name") {
      EXPECT_EQ(result.second, "Chris");
    } else {
      EXPECT_EQ(result.second, "48");
    }

    keys.clear();
    keys.insert("one");
    keys.insert("two");
    keys.insert("three");
    result = cache.get_any(keys);
    EXPECT_EQ(result.first, "");
    EXPECT_EQ(result.second, "");

    keys.clear();
    keys.insert("one");
    keys.insert("two");
    keys.insert("name");
    result = cache.get_any(keys);
    EXPECT_EQ(result.first, "name");
    EXPECT_EQ(result.second, "Chris");

  }
  DELETE_IF_EXISTS(file);

}
