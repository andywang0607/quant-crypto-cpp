add_library(QuoteNode INTERFACE)

target_sources(QuoteNode INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/QuoteNode.hpp)

target_include_directories(QuoteNode INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(QuoteNode INTERFACE spdlog_header_only Util QuoteApi)