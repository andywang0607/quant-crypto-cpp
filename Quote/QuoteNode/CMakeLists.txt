add_library(QuoteNode INTERFACE)

target_sources(QuoteNode INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/QuoteNode.hpp
                                   ${CMAKE_CURRENT_SOURCE_DIR}/QuoteAdapter.hpp)

target_include_directories(QuoteNode INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(QuoteNode INTERFACE Util QuoteApi json)
