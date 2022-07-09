cmake_minimum_required(VERSION 3.12)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Configure.cmake")

project(Saper)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/SFML")
include_directories("${CMAKE_SOURCE_DIR}/3rdparty/SFML/include")

include_directories("${CMAKE_SOURCE_DIR}/3rdparty/")

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Compiler.cmake")

add_subdirectory(code)