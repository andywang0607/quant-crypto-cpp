add_library(Util-Resource INTERFACE)
target_sources(Util-Resource INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/FixedObjectPool.hpp)
target_include_directories(Util-Resource INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(UnitTest)