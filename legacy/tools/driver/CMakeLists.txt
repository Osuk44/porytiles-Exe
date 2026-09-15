project(LegacyDriver CXX)

FILE(GLOB_RECURSE CppSources CONFIGURE_DEPENDS *.cpp)
add_executable(LegacyDriver ${CppSources})
# Adding this to try to help CLion and CMake with inspections
target_compile_definitions(LegacyDriver PRIVATE DOCTEST_CONFIG_DISABLE)
set_target_properties(LegacyDriver PROPERTIES OUTPUT_NAME "porytiles-legacy")
target_link_libraries(LegacyDriver PRIVATE LegacyLib)

# Windows compatibility: replicate LegacyLib's MSVC warning suppressions.
# LegacyLib sets these PRIVATE, so linking alone does not propagate them.
if(MSVC)
    target_compile_definitions(LegacyDriver PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(LegacyDriver PRIVATE
        /wd4100 /wd4101 /wd4244 /wd4267 /wd4458 /wd4459 /wd4611 /wd4702
    )
endif()

install(TARGETS LegacyDriver
        RUNTIME DESTINATION bin)
