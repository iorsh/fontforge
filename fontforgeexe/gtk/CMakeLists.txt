# Distributed under the original FontForge BSD 3-clause license

add_library(ffgtk OBJECT
  application.hpp
  application.cpp
  css_builder.hpp
  css_builder.cpp
  dialog_base.hpp
  dialog_base.cpp
  language_list.hpp
  language_list.cpp
  print_dialog.hpp
  print_dialog.cpp
  print_preview.hpp
  print_preview.cpp
  simple_dialogs.hpp
  simple_dialogs.cpp
  utils.hpp
  utils.cpp
  widgets.hpp
  win32_utils.hpp
  win32_utils.cpp
)

target_link_libraries(ffgtk PRIVATE
  fontforge_common_headers
  fontforge
  PkgConfig::GTKMM
)
