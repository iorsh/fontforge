# TODO: try and make it build in a way that links properly...
#add_library(gtk STATIC)
#target_sources(gtk
target_sources(fontforgeexe
   PUBLIC
      FILE_SET cxx_modules TYPE CXX_MODULES FILES
         colour.cppm
         open_dialog.cppm
)
