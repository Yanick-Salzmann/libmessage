cmake_minimum_required(VERSION 3.12)
project(libmessage)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

if (WIN32)
    add_definitions("-DWIN32_LEAN_AND_MEAN")
endif ()

add_subdirectory(third_party)
add_subdirectory(utils)
add_subdirectory(definition)
