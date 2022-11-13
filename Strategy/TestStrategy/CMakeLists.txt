add_library(TestStrategy SHARED TestStrategy.cpp TestStrategy.h)
target_include_directories(TestStrategy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(TestStrategy Strategy-Interface)
install(TARGETS TestStrategy
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION bin)

