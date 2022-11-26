add_library(BinanceQuote INTERFACE)
target_sources(
  BinanceQuote INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/BinanceSpotQuote.hpp)

target_include_directories(BinanceQuote INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(BinanceQuote INTERFACE QuoteNode QuoteApi Util-Time Util-Log Util-StringUtil json restclient-cpp)

install(
  TARGETS BinanceQuote
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION bin)

add_subdirectory(test)
