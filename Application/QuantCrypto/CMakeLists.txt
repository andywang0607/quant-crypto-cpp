add_executable(QuantCrypto main.cpp)
target_link_libraries(QuantCrypto Util-DynamicLoader Util-Thread json)
install(TARGETS QuantCrypto RUNTIME
        DESTINATION bin)