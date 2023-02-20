add_library(TradeNode INTERFACE)
target_sources(
  TradeNode
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/TradeStruct.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/SpotTradeNode.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/PerpetualTradeNode.hpp)

target_include_directories(TradeNode INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(TradeNode INTERFACE Util-Resource spdlog_header_only)
