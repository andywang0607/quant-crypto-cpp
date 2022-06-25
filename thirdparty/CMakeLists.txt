# hash-library
add_subdirectory(hash-library)
target_include_directories(hash-library
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/hash-library)
# restclient
add_subdirectory(restclient-cpp)

# websocketpp
add_library(websocketpp INTERFACE)
target_include_directories(websocketpp
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/websocketpp)

# json
add_library(json INTERFACE)
target_include_directories(json
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/json/include)

# spdlog
add_subdirectory(spdlog)