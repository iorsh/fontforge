# TODO: try and make it build in a way that links properly...
#add_library(gtk STATIC)
#target_sources(gtk
target_sources(fontforgeexe
   PUBLIC
      application.hpp
      application.cpp
      common_menus.cpp
      font_view.hpp
      font_view.cpp
      font_view_menu.cpp
      menu_builder.hpp
      menu_builder.cpp
      open_dialog.hpp
      open_dialog.cpp
      utils.hpp
      utils.cpp
)
