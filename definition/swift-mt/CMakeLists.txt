cmake_minimum_required(VERSION 3.12)

project(definition-swift-mt)

set(PROTO_OUTPUT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/src/proto)
set(PROTO_FILE_INPUT_LIST SwiftMtMsgDefinition)
set(PROTO_FILE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/src/proto)
set(GENERATED_TARGET swift-mt-definition-proto)

include(ProtoCompile)

add_executable(swift-mt-web-repo
        src/WebMessageIndex.cpp
        src/web/WebDefinitionRepository.hpp
        src/web/WebDefinitionRepository.cpp
        src/web/MessageIndex.hpp
        src/web/MessageIndex.cpp
        src/web/MessageIndexEntry.cpp
        src/web/MessageIndexEntry.hpp
        ${GENERATED_FILES} src/definition/MessageIndexParser.cpp src/definition/MessageIndexParser.hpp src/definition/SequenceStack.cpp src/definition/SequenceStack.hpp)

add_dependencies(swift-mt-web-repo ${GENERATED_TARGET})

target_link_libraries(swift-mt-web-repo utils)
target_include_directories(swift-mt-web-repo PUBLIC ${CMAKE_SOURCE_DIR}/utils/include)
target_include_directories(swift-mt-web-repo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
