set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(googletest)

add_executable(
  unittests

  ffx.cpp)
target_link_libraries(
  unittests
  ubiqfpe gtest gtest_main)

add_custom_target(
  test
  COMMAND unittests
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
