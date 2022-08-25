add_library(BybitTradeAdapter INTERFACE)
target_sources(
  BybitTradeAdapter
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/BybitSpotTradeAdapter.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/BybitPerpetualTradeAdapter.hpp)
target_include_directories(BybitTradeAdapter
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(BybitTradeAdapter INTERFACE TradeApi Util json
                                                  restclient-cpp)

add_subdirectory(test)
