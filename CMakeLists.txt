cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)

project("QuantCrypto")

set(QuantCrypto_INSTALL_DIR ${PROJECT_SOURCE_DIR})
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

set(targets thirdparty Quote Util)
foreach(target ${targets})
  add_subdirectory(${target})
endforeach(target)
