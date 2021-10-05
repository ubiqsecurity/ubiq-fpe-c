set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(googletest)

add_executable(
  unittests

  bn.cpp
  ff1.cpp
  ff3_1.cpp
  ffx.cpp)
target_link_libraries(
  unittests
  gtest gtest_main ubiqfpe)

add_executable(
  unittests-static

  bn.cpp
  ff1.cpp
  ff3_1.cpp
  ffx.cpp)

target_link_libraries(
  unittests-static
  gtest gtest_main ubiqfpe-static)


add_custom_target(
  test
  COMMAND unittests
  COMMAND unittests-static
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})  


