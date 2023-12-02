add_library(ffgtk OBJECT
  open_dialog.hpp
  open_dialog.cpp
  open_dialog_shim.hpp
  open_dialog_shim.cpp
  utils.hpp
  utils.cpp
)

target_link_libraries(ffgtk PRIVATE
  fontforge_common_headers
  PkgConfig::GTKMM
)