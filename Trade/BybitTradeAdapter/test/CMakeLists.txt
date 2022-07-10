add_executable(BybitTradeTest
  main.cpp
)

target_link_libraries(BybitTradeTest PRIVATE BybitTradeAdapter)

install(
  TARGETS BybitTradeTest
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION bin)