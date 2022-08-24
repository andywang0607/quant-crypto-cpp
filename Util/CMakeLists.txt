set(targets Time Sign Event Log)

add_library(Util INTERFACE)
foreach(target ${targets})
  add_subdirectory(${target})
  target_link_libraries(Util INTERFACE Util-${target})
endforeach(target)
