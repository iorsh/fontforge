# Distributed under the original FontForge BSD 3-clause license

add_library(layout INTERFACE
  i_printer.hpp
)

if(BUILD_SHARED_LIBS)
  set_property(TARGET layout PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()
