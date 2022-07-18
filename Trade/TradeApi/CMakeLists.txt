add_library(TradeApi INTERFACE)
target_sources(
  TradeApi
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/TradeApi.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/TradeStruct.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/TradeNode.hpp)

target_include_directories(TradeApi INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(TradeApi INTERFACE spdlog_header_only)
