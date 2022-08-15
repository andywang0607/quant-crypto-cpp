add_library(QuoteData INTERFACE)

target_sources(QuoteData INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/QuoteData.hpp
                                   ${CMAKE_CURRENT_SOURCE_DIR}/QuoteNode.hpp)

target_include_directories(QuoteData INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(QuoteData INTERFACE spdlog_header_only Util)