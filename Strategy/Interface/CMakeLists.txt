add_library(Strategy-Interface INTERFACE)
target_sources(
  Strategy-Interface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/StrategyBase.h)
target_include_directories(Strategy-Interface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Strategy-Interface INTERFACE QuoteApi Util-DynamicLoader json)
