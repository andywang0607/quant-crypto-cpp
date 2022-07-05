add_library(Util-Time INTERFACE)
target_sources(Util-Time INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/TimeUtils.hpp)
target_include_directories(Util-Time INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
