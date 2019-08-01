add_library(utils DBCache.cpp)

target_link_libraries(utils sqlite3)
include_directories("${PROJECT_SOURCE_DIR}/sqlite3")


#----- tests
include(CTest)

add_executable(t_sqlite utest/t_sqlite.cpp)
target_link_libraries(t_sqlite gtest_main sqlite3)
add_test(SQLite_Tests t_sqlite)

add_executable(t_DBCache utest/t_DBCache.cpp)
target_link_libraries(t_DBCache gtest_main utils)
add_test(DBCache_Tests t_DBCache)

add_executable(t_FilePagedQueue utest/t_FilePagedQueue.cpp)
target_link_libraries(t_FilePagedQueue gtest_main utils)
add_test(FilePagedQueue_Tests t_FilePagedQueue)

add_executable(t_PersistentFilePagedQueue utest/t_PersistentFilePagedQueue.cpp)
target_link_libraries(t_PersistentFilePagedQueue gtest_main utils)
add_test(PersistentFilePagedQueue_Tests t_PersistentFilePagedQueue)
