add_library(BinanceQuote SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/BinanceQuoteAdapter.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/BinanceSpotQuote.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/BinanceContractQuote.hpp
)

target_include_directories(BinanceQuote PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(BinanceQuote PUBLIC QuoteNode QuoteApi Util-DynamicLoader Util-Time Util-Log Util-StringUtil json restclient-cpp)

install(
  TARGETS BinanceQuote
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION bin)

add_subdirectory(test)
