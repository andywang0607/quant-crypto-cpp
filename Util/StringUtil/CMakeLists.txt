add_library(Util-StringUtil INTERFACE)
target_sources(Util-StringUtil INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/StringUtil.hpp)
target_include_directories(Util-StringUtil INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Util-StringUtil INTERFACE Util-Event)

add_subdirectory(UnitTest)