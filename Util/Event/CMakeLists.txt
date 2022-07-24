add_library(Util-Event INTERFACE)
target_sources(Util-Event INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/InvokeSequence.hpp)
target_include_directories(Util-Event INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(UnitTest)