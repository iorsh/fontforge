# Distributed under the original FontForge BSD 3-clause license

add_library(layout OBJECT
  i_printer.hpp
  legacy_printer.hpp
)

target_sources(layout PRIVATE
  legacy_printer.cpp
)

if(BUILD_SHARED_LIBS)
  set_property(TARGET layout PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()
