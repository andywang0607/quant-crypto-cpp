add_library(Strategy-Loader INTERFACE)
target_sources(
  Strategy-Loader INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/DynamicStrategyLoader.hpp
                       ${CMAKE_CURRENT_SOURCE_DIR}/StrategyLoader.hpp)
target_include_directories(Strategy-Loader INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Strategy-Loader INTERFACE dl json)
