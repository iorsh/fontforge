# Distributed under the original FontForge BSD 3-clause license

# Note that you should invoke the `check` target instead of
# the `test` target so that test-specific prerequisites are checked.

include(TestUtils)

if(NOT ENABLE_NATIVE_SCRIPTING AND NOT ENABLE_PYTHON_SCRIPTING_RESULT)
  return()
endif()

add_executable(systestdriver systestdriver.c)
target_link_libraries(systestdriver PRIVATE GLIB::GLIB)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(systestdriver PRIVATE -Wall -pedantic)
endif()

# Downloaded files
add_download_target("NotoSans-Regular.ttc" "https://github.com/fontforge/debugfonts/raw/master/NotoSans-Regular.ttc")
add_download_target("MunhwaGothic-Bold" "https://github.com/fontforge/debugfonts/raw/master/MunhwaGothic-Bold")

add_custom_target(test_dependencies
  DEPENDS
  "${CMAKE_CURRENT_BINARY_DIR}/fonts/NotoSans-Regular.ttc"
  "${CMAKE_CURRENT_BINARY_DIR}/fonts/MunhwaGothic-Bold"
)

add_custom_target(check
  COMMAND "${CMAKE_CTEST_COMMAND}" "--output-on-failure" "--force-new-ctest-process"
  DEPENDS
    test_dependencies
    fontforgeexe
    systestdriver
  VERBATIM
  USES_TERMINAL
)

if(ENABLE_PYTHON_SCRIPTING_RESULT AND ENABLE_PYTHON_EXTENSION_RESULT)
  add_dependencies(check fontforge_pyhook psMat_pyhook)
endif()

if(ENABLE_NATIVE_SCRIPTING)
  add_ff_test(test139.pe "StrokeTests.sfd"                                            "ExpandStroke parameters")
endif()

if(ENABLE_PYTHON_SCRIPTING_RESULT)
  add_py_test(test1003.py "StrokeTests.sfd" "Various stroke tests")
endif()
