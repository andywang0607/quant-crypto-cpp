add_library(QuoteReceiver-Util INTERFACE)
target_sources(QuoteReceiver-Util
               INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/QuoteSerializer.hpp)
target_include_directories(QuoteReceiver-Util
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(QuoteReceiver-Util INTERFACE QuoteApi)

add_subdirectory(UnitTest)
