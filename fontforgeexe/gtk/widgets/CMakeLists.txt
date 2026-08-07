# Distributed under the original FontForge BSD 3-clause license

add_library(ffgtk_widgets STATIC
  histogram.hpp
  histogram.cpp
  monochrome_ribbon.hpp
  monochrome_ribbon.cpp
  verified_entry.hpp
  verified_entry.cpp
  num_entry.hpp
  num_entry.cpp
)

target_link_libraries(ffgtk_widgets PRIVATE
  PkgConfig::GTKMM
)
