add_executable(QuotePrinter
  QuotePrinter.cpp
)

target_link_libraries(QuotePrinter PRIVATE BybitQuote)

install(
  TARGETS QuotePrinter
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION bin)

install(FILES config_sample.json
  DESTINATION ${QuantCrypto_INSTALL_DIR}/bin)