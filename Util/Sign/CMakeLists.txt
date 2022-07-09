add_library(Util-Sign INTERFACE)
target_sources(Util-Sign INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/BybitSignTool.hpp)
target_include_directories(Util-Sign INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Util-Sign INTERFACE hash-library)

add_subdirectory(UnitTest)