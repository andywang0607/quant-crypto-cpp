add_library(TradeApi INTERFACE)
target_sources(
  TradeApi
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/TradeStruct.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/TradeNode.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/TradeAdapter.hpp)

target_include_directories(TradeApi INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(TradeApi INTERFACE spdlog_header_only)
