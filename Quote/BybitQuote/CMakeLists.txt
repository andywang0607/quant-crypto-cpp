add_library(BybitQuote SHARED ${CMAKE_CURRENT_SOURCE_DIR}/BybitQuoteAdapter.cpp
                              ${CMAKE_CURRENT_SOURCE_DIR}/BybitV5QuoteHandler.hpp
                              ${CMAKE_CURRENT_SOURCE_DIR}/BybitSpotQuote.hpp
                              ${CMAKE_CURRENT_SOURCE_DIR}/BybitContractQuote.hpp
)

target_include_directories(BybitQuote PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(BybitQuote PUBLIC QuoteNode QuoteApi Util-Time Util-Log json)

install(
  TARGETS BybitQuote
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION bin)

add_subdirectory(test)
