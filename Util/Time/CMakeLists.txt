add_library(Util-Time INTERFACE)
target_sources(Util-Time INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/TimeUtils.hpp)
target_include_directories(Util-Time INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Util-Time INTERFACE boost_date_time)

add_subdirectory(UnitTest)