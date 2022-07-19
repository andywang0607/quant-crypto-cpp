add_executable(BybitSpotTradeTest
  spot.cpp
)

target_link_libraries(BybitSpotTradeTest PRIVATE BybitTradeAdapter)

install(
  TARGETS BybitSpotTradeTest
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION bin)

add_executable(BybitPerpetualTradeTest
  perpetual.cpp
)

target_link_libraries(BybitPerpetualTradeTest PRIVATE BybitTradeAdapter)

install(
  TARGETS BybitPerpetualTradeTest
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION bin)