add_library(Util-Thread INTERFACE)
target_sources(Util-Thread INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/ThreadUtil.hpp)
target_include_directories(Util-Thread INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Util-Thread INTERFACE Util-Event)

add_subdirectory(UnitTest)