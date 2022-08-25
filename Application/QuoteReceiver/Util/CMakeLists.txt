add_library(QuoteReceiver-Util INTERFACE)
target_sources(
  QuoteReceiver-Util INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/QuoteSerializer.hpp
                               ${CMAKE_CURRENT_SOURCE_DIR}/QuoteWritter.hpp)
target_include_directories(QuoteReceiver-Util
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(QuoteReceiver-Util INTERFACE QuoteApi Util)

add_subdirectory(UnitTest)
