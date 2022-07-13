add_library(TradeApi INTERFACE)
target_sources(TradeApi INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/TradeApi.hpp ${CMAKE_CURRENT_SOURCE_DIR}/TradeStruct.hpp)
target_include_directories(TradeApi INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
