set(targets Websocket Time Sign Event Log Thread StringUtil Querier Requester Resource)

add_library(Util INTERFACE)
foreach(target ${targets})
  add_subdirectory(${target})
  target_link_libraries(Util INTERFACE Util-${target})
endforeach(target)
