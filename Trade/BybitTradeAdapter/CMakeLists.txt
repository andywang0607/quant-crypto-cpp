add_library(BybitTradeAdapter INTERFACE)
target_sources(
  BybitTradeAdapter
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/BybitSpotTradeAdapter.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/BybitPerpetualTradeAdapter.hpp)
target_include_directories(BybitTradeAdapter
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(BybitTradeAdapter INTERFACE TradeNode Util json)

add_subdirectory(test)
