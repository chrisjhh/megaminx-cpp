add_library(megaminx face.cpp megaminx.cpp)


#----- tests
include(CTest)

add_executable(t_face utest/t_face.cpp)
target_link_libraries(t_face gtest_main megaminx)

add_executable(t_megaminx utest/t_megaminx.cpp)
target_link_libraries(t_megaminx gtest_main megaminx)

add_test(Face_Tests t_face)
add_test(Megaminx_Tests t_megaminx)

