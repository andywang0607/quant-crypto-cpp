add_library(BybitQuote INTERFACE)
target_sources(
  BybitQuote INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/BybitSpotQuote.hpp
                       ${CMAKE_CURRENT_SOURCE_DIR}/BybitPerpetualQuote.hpp)

target_include_directories(BybitQuote INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(BybitQuote INTERFACE QuoteData Receiver json spdlog_header_only Util-Time)

install(
  TARGETS BybitQuote
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION bin)

add_subdirectory(test)
