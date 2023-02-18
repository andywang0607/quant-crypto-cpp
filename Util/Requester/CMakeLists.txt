add_library(Util-Requester INTERFACE)
target_sources(Util-Requester INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/RestRequester.hpp)
target_include_directories(Util-Requester INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Util-Requester INTERFACE Util-Log restclient-cpp)
