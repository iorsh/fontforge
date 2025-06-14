# Distributed under the GNU GPL version 3 or later

add_library(cgal_utils OBJECT
  remove_overlap.hpp
  remove_overlap.cpp
)

target_link_libraries(cgal_utils PRIVATE
  fontforge_common_headers
  fontforge_base
  ${CGAL_LIBRARIES}
)

set_target_properties(cgal_utils PROPERTIES POSITION_INDEPENDENT_CODE ON)
