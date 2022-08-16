add_library(QuoteApi INTERFACE)

target_sources(QuoteApi INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/QuoteData.hpp
                                  ${CMAKE_CURRENT_SOURCE_DIR}/QuoteApi.hpp)

target_include_directories(QuoteApi INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(QuoteApi INTERFACE spdlog_header_only Util)
