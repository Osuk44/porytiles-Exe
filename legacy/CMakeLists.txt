
# Set a variable that points to the global include directory for Porytiles
set(PORYTILES_LEGACY_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/legacy/include)

add_subdirectory(lib)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
add_subdirectory(tools)
