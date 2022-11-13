add_executable(StrategyTest
  main.cpp
)

target_link_libraries(StrategyTest PRIVATE TestStrategy Strategy-Loader BybitQuote spdlog)

install(
  TARGETS BybitTest
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION bin)