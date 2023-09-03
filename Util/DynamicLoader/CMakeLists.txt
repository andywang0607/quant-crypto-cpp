add_library(Util-DynamicLoader INTERFACE)
target_sources(
    Util-DynamicLoader INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/DynamicLoader.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BaseNode.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BaseNodeLoader.hpp
)
target_include_directories(Util-DynamicLoader INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Util-DynamicLoader INTERFACE dl json Util-Log)