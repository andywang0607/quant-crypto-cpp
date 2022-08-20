set(UNIT_TEST_NAME QuoteReceiverUtilTester)
add_executable(${UNIT_TEST_NAME} QuoteSerializerTest.cpp)

target_link_libraries(${UNIT_TEST_NAME} QuoteReceiver-Util)
target_link_libraries(${UNIT_TEST_NAME} gtest_main)
add_test(${UNIT_TEST_NAME} ${UNIT_TEST_NAME})
add_custom_command(
  TARGET ${UNIT_TEST_NAME}
  POST_BUILD
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMAND ${CMAKE_CTEST_COMMAND} -R "^${UNIT_TEST_NAME}" --verbose
          --output-on-failures)
