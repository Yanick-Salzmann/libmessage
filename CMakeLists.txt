cmake_minimum_required(VERSION 3.14)
project(libmessage)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

add_subdirectory(third_party)
add_subdirectory(utils)
add_subdirectory(definition)
