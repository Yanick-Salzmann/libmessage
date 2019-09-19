cmake_minimum_required(VERSION 3.12)

set(PROTO_EXECUTABLE ${CMAKE_BINARY_DIR}/third_party/protobuf/bin/protoc)

set(GENERATED_PROTOBUF_PATH ${PROTO_OUTPUT_FOLDER})

list(LENGTH PROTO_FILE_INPUT_LIST num_proto_files_raw)
math(EXPR num_proto_files "${num_proto_files_raw} - 1")

foreach (loop_var RANGE ${num_proto_files})
    list(GET PROTO_FILE_INPUT_LIST ${loop_var} PROTO_FILE_NAME)
    list(GET PROTO_FILE_FOLDER ${loop_var} PROTO_PATH)

    add_custom_command(
            OUTPUT "${GENERATED_PROTOBUF_PATH}/${PROTO_FILE_NAME}.pb.cc"
            "${GENERATED_PROTOBUF_PATH}/${PROTO_FILE_NAME}.pb.h"
            DEPENDS ${PROTO_PATH}/${PROTO_FILE_NAME}.proto
            COMMAND ${PROTO_EXECUTABLE}
            ARGS "--proto_path=${PROTO_PATH}"
            "--cpp_out=${GENERATED_PROTOBUF_PATH}"
            "${PROTO_PATH}/${PROTO_FILE_NAME}.proto"
    )

    list(APPEND GENERATED_FILES
            "${GENERATED_PROTOBUF_PATH}/${PROTO_FILE_NAME}.pb.cc"
            )

    list(APPEND OUT_FILES ${GENERATED_PROTOBUF_PATH}/${PROTO_FILE_NAME}.pb.cc ${GENERATED_PROTOBUF_PATH}/${PROTO_FILE_NAME}.pb.h)


endforeach (loop_var)

include_directories(${CMAKE_BINARY_DIR}/third_party/protobuf/include)

add_custom_target(${GENERATED_TARGET} ALL DEPENDS ${OUT_FILES})
set_source_files_properties(${GENERATED_FILES}
        PROPERTIES GENERATED TRUE)

if (WIN32)
    link_libraries(${CMAKE_BINARY_DIR}/third_party/protobuf/lib/libprotobufd.lib)
else ()
    link_libraries(${CMAKE_BINARY_DIR}/third_party/protobuf/lib/libprotobuf.a)
endif ()

add_dependencies(${GENERATED_TARGET} protobuf)
