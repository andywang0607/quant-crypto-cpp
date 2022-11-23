add_executable(BinanceQuoteTest
  main.cpp
)

target_link_libraries(BinanceQuoteTest PRIVATE BinanceQuote spdlog_header_only)

install(
  TARGETS BinanceQuoteTest
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION bin)