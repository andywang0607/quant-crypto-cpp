add_library(Util-Websocket INTERFACE)

target_sources(Util-Websocket
               INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/WebSocketReceiver.hpp)

target_include_directories(Util-Websocket INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Util-Websocket INTERFACE Util-Log Util-Thread websocketpp json)
find_package(Threads REQUIRED)
if(Threads_FOUND)
  target_link_libraries(Util-Websocket INTERFACE Threads::Threads)
endif()

find_package(Boost REQUIRED COMPONENTS system)
if(Boost_FOUND)
  message(STATUS "Boost_INCLUDE_DIRS = ${Boost_INCLUDE_DIRS}")
  message(STATUS "Boost_LIBRARIES = ${Boost_LIBRARIES}")
  message(STATUS "Boost_VERSION = ${Boost_VERSION}")
  message(STATUS "Boost_LIB_VERSION = ${Boost_LIB_VERSION}")

  target_link_libraries(Util-Websocket INTERFACE Boost::system)
endif()

find_package(OpenSSL REQUIRED)
if(OpenSSL_FOUND)
  target_link_libraries(Util-Websocket INTERFACE OpenSSL::SSL)
endif()
