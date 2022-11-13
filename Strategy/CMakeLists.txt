set(targets Interface Loader)

add_library(Strategy INTERFACE)
foreach(target ${targets})
  add_subdirectory(${target})
  target_link_libraries(Strategy INTERFACE Strategy-${target})
endforeach(target)

add_subdirectory(TestStrategy)
add_subdirectory(test)