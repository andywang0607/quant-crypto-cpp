add_library(Util-Querier INTERFACE)
target_sources(Util-Querier INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Querier.hpp)
target_include_directories(Util-Querier INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Util-Querier INTERFACE Util-Log)

add_subdirectory(UnitTest)