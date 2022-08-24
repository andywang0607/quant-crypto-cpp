add_library(Util-Log INTERFACE)
target_sources(Util-Log INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Logger.hpp)
target_include_directories(Util-Log INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Util-Log INTERFACE spdlog_header_only)

