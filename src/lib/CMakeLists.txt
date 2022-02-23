add_library(
  c_objects

  OBJECT

  bn.c
  ff1.c
  ff3_1.c
  ffx.c)

if(WIN32)
  # silence warnings about "more secure"
  # alternative functions on windows
  #
  # silence warnings about ISO conformant names
  target_compile_definitions(
    c_objects
    PRIVATE
    DLL_EXPORT
    _CRT_SECURE_NO_WARNINGS
    strdup=_strdup
    strcasecmp=_stricmp)
  target_sources(
    c_objects

    PRIVATE

    wincrypt.c)
else()
  target_link_libraries(
  c_objects
  unistring)


  # target_sources(
  #   c_objects

  #   PRIVATE

  #   openssl.c)
endif()

target_include_directories(
  c_objects
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../include)
set_target_properties(
  c_objects
  PROPERTIES
  POSITION_INDEPENDENT_CODE ON)
