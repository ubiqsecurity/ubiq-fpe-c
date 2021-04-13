add_subdirectory(lib)
add_subdirectory(test)

function(strip_target TARGET)
  set(CMAKE_STRIP_COMMAND /usr/bin/strip)

  if(CMAKE_BUILD_TYPE STREQUAL Release OR
      CMAKE_BUILD_TYPE STREQUAL MinSizeRel)
    add_custom_command(
      TARGET ${TARGET} POST_BUILD
      COMMAND ${CMAKE_STRIP_COMMAND} $<TARGET_FILE:${TARGET}>)
  endif()
endfunction(strip_target)

# c static library
# add_library(
#   ubiqfpe-static
#   STATIC
#   $<TARGET_OBJECTS:c_objects>)
# set_target_properties(
#   ubiqfpe-static
#   PROPERTIES
#   OUTPUT_NAME ubiqfpe)
# target_include_directories(
#   ubiqfpe-static
#   PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# strip_target(ubiqfpe-static)

add_library(
  ubiqfpe
  SHARED
  $<TARGET_OBJECTS:c_objects>)
set_target_properties(
  ubiqfpe
  PROPERTIES
  VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})
target_include_directories(
  ubiqfpe
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
if(WIN32)
  target_link_libraries(
    ubiqfpe
    ws2_32 userenv bcrypt crypt32)
else()
  target_link_libraries(
    ubiqfpe
    crypto
    gmp)
endif()
