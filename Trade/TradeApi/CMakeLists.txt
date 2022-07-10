add_library(TradeApi INTERFACE)
target_sources(TradeApi INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/TradeApi.hpp)
target_include_directories(TradeApi INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
