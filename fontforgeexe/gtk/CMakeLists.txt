# Distributed under the original FontForge BSD 3-clause license

add_subdirectory(widgets)

add_library(ffgtk OBJECT
  application.hpp
  application.cpp
  bitmap_view.hpp
  bitmap_view.cpp
  bitmap_view_shim.hpp
  bitmap_view_shim.cpp
  bitmaps_dlg.hpp
  bitmaps_dlg.cpp
  c_context.h
  css_builder.hpp
  css_builder.cpp
  dialog_base.hpp
  dialog_base.cpp
  find_problems.hpp
  find_problems.cpp
  language_list.hpp
  language_list.cpp
  pixel_grid.hpp
  pixel_grid.cpp
  show_histogram.hpp
  show_histogram.cpp
  simple_dialogs.hpp
  simple_dialogs.cpp
  utils.hpp
  utils.cpp
)

target_link_libraries(ffgtk PUBLIC
  fontforge_common_headers
  ffgtk_widgets
  PkgConfig::GTKMM
)
