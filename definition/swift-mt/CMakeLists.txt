cmake_minimum_required(VERSION 3.12)

project(definition-swift-mt)

set(PROTO_OUTPUT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/src/proto)
set(PROTO_FILE_INPUT_LIST SwiftMtMsgDefinition)
set(PROTO_FILE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/src/proto)
set(GENERATED_TARGET swift-mt-definition-proto)

include(ProtoCompile)

find_package(Threads)
find_package(Java)

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/antlr/SwiftMtComponentFormatParser.cpp
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/antlr/SwiftMtComponentFormat.g4
        COMMAND ${Java_JAVA_EXECUTABLE}
        ARGS -cp ${CMAKE_SOURCE_DIR}/third_party/antlr4/src/antlr4/tool/target/antlr4-4.7.3-SNAPSHOT-complete.jar
        org.antlr.v4.Tool
        -o ${CMAKE_CURRENT_SOURCE_DIR}/src/antlr
        -package message::definition::swift::mt
        -Dlanguage=Cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/antlr/SwiftMtComponentFormat.g4
)

set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/antlr/SwiftMtComponentFormatParser.cpp PROPERTIES GENERATED TRUE)

add_executable(swift-mt-web-repo
        src/WebMessageIndex.cpp
        src/web/WebDefinitionRepository.hpp
        src/web/WebDefinitionRepository.cpp
        src/web/MessageIndex.hpp
        src/web/MessageIndex.cpp
        src/web/MessageIndexEntry.cpp
        src/web/MessageIndexEntry.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/antlr/SwiftMtComponentFormatParser.cpp
        ${GENERATED_FILES} src/definition/MessageIndexParser.cpp src/definition/MessageIndexParser.hpp src/definition/SequenceStack.cpp src/definition/SequenceStack.hpp)

target_compile_definitions(swift-mt-web-repo PUBLIC "-DANTLR4CPP_STATIC -D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING")

add_dependencies(swift-mt-web-repo ${GENERATED_TARGET} antlr4)

target_link_libraries(swift-mt-web-repo utils Threads::Threads)

if (WIN32)
    target_link_libraries(swift-mt-web-repo ${CMAKE_BINARY_DIR}/third_party/antlr4/lib/antlr4-runtime-static.lib)
else ()
endif ()

if (UNIX)
    target_link_libraries(swift-mt-web-repo lber)
endif ()

target_include_directories(swift-mt-web-repo PUBLIC ${CMAKE_SOURCE_DIR}/utils/include)
target_include_directories(swift-mt-web-repo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(swift-mt-web-repo PUBLIC ${CMAKE_BINARY_DIR}/third_party/antlr4/include/antlr4-runtime)
