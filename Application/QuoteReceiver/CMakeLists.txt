add_library(QuoteReceiver SHARED 
  QuoteReceiver.cpp
  QuoteReceiver.h
)

target_link_libraries(QuoteReceiver PRIVATE QuoteReceiver-Util)

install(
  TARGETS QuoteReceiver
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION bin)

install(FILES config_sample.json DESTINATION ${QuantCrypto_INSTALL_DIR}/bin)

add_subdirectory(Util)
