add_executable(BybitTest
  main.cpp
)

target_link_libraries(BybitTest PRIVATE BybitQuote spdlog_header_only)

install(
  TARGETS BybitTest
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION bin)