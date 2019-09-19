cmake_minimum_required(VERSION 3.12)

project(ThirdParty)

include(ExternalProject)
include(CheckIncludeFile)

set(ANTLR4_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/antlr4)
set(CURL_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/libcurl)
set(SPDLOG_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/spdlog)
set(GUMBO_QUERY_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/gumbo-query)
set(GUMBO_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/gumbo-parser)
set(OPENSSL_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/openssl)
set(PROTOBUF_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/protobuf)
set(ABSEIL_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/abseil)
set(FS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/filesystem)

ExternalProject_Add(
        antlr4
        PREFIX ${ANTLR4_ROOT}
        GIT_REPOSITORY https://github.com/antlr/antlr4.git
        GIT_TAG 8bcbea425b110f2fd6524aabd4ef7d823bb5c5c9
        UPDATE_COMMAND ""
        PATCH_COMMAND ""
        SOURCE_DIR ${ANTLR4_ROOT}/src/antlr4
        SOURCE_SUBDIR runtime/Cpp
        CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/antlr4
        -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
        -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
)

ExternalProject_Add_Step(
        antlr4
        java-build
        COMMAND ./mvnw package -DskipTests -f antlr4/src/antlr4 -pl runtime/Java -am
        COMMENT "Building Java Antlr4"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        ALWAYS TRUE
)

set(ANTLR4_TOOL ${CMAKE_CURRENT_SOURCE_DIR}/antlr4/src/antlr4/runtime/Java/target/antlr4-runtime-4.7.3-SNAPSHOT.jar)

find_package(OpenSSL)
if (OpenSSL_FOUND)
    message("Found OpenSSL version ${OPENSSL_VERSION}")
    ExternalProject_Add(
            curl
            PREFIX ${CURL_ROOT}
            GIT_REPOSITORY https://github.com/curl/curl.git
            GIT_TAG master
            UPDATE_COMMAND ""
            PATCH_COMMAND ""
            SOURCE_DIR ${CURL_ROOT}/src/curl
            CMAKE_CACHE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/libcurl
            -DBUILD_CURL_EXE:BOOL=OFF
            -DBUILD_SHARED_LIBS:BOOL=OFF
            -DENABLE_MANUAL:BOOL=OFF
            -DCMAKE_USE_OPENSSL:BOOL=ON
            -DBUILD_TESTING:BOOL=OFF
            -DOPENSSL_ROOT_DIR:PATH=${OPENSSL_ROOT_DIR}
            -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
            -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
    )
else ()
    message("OpenSSL not found, building from source...")

    find_package(perl REQUIRED)

    ExternalProject_Add(
            openssl
            BUILD_IN_SOURCE 1
            PREFIX ${OPENSSL_ROOT}
            GIT_REPOSITORY https://github.com/openssl/openssl.git
            GIT_TAG master
            UPDATE_COMMAND ""
            PATCH_COMMAND ""
            SOURCE_DIR ${OPENSSL_ROOT}/src/openssl
            CONFIGURE_COMMAND ${PERL_EXECUTABLE} ${OPENSSL_ROOT}/src/openssl/Configure VC-WIN64A no-asm no-shared no-hw no-psk no-srp no-ec2m no-dtls no-dtls1 no-engine no-dso --prefix=${OPENSSL_ROOT}/src/openssl/dist --openssldir=${OPENSSL_ROOT}/src/openssl
    )

    ExternalProject_Add(
            curl
            PREFIX ${CURL_ROOT}
            GIT_REPOSITORY https://github.com/curl/curl.git
            GIT_TAG master
            UPDATE_COMMAND ""
            PATCH_COMMAND ""
            SOURCE_DIR ${CURL_ROOT}/src/curl
            CMAKE_CACHE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/libcurl
            -DBUILD_CURL_EXE:BOOL=OFF
            -DBUILD_SHARED_LIBS:BOOL=OFF
            -DENABLE_MANUAL:BOOL=OFF
            -DCMAKE_USE_OPENSSL:BOOL=ON
            -DBUILD_TESTING:BOOL=OFF
            -DOPENSSL_ROOT_DIR:PATH=${CMAKE_CURRENT_SOURCE_DIR}/openssl/src/openssl/dist
            -DOPENSSL_INCLUDE_DIR:PATH=${CMAKE_CURRENT_SOURCE_DIR}/openssl/src/openssl/dist/include
            -DOPENSSL_LIBRARY:FILEPATH=${CMAKE_CURRENT_SOURCE_DIR}/openssl/src/openssl/dist/lib/libssl.lib
            -DOPENSSL_CRYPTO_LIBRARY:FILEPATH=${CMAKE_CURRENT_SOURCE_DIR}/openssl/src/openssl/dist/lib/libcrypto.lib
            -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
            -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
    )

    add_dependencies(curl openssl)
endif ()

ExternalProject_Add(
        spdlog
        PREFIX ${SPDLOG_ROOT}
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG master
        UPDATE_COMMAND ""
        PATCH_COMMAND ""
        SOURCE_DIR ${SPDLOG_ROOT}/src/spdlog
        CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/spdlog
        -DSPDLOG_BUILD_SHARED:BOOL=OFF
        -DSPDLOG_BUILD_TESTS:BOOL=OFF
        -DSPDLOG_BUILD_TESTS_HO:BOOL=OFF
        -DSPDLOG_BUILD_BENCH:BOOL=OFF
        -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
        -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
)

if (WIN32)
    set(GUMBO_LIBRARY_PATH ${CMAKE_CURRENT_BINARY_DIR}/gumbo-parser/lib/gumbo.lib)
else ()
    set(GUMBO_LIBRARY_PATH ${CMAKE_CURRENT_BINARY_DIR}/gumbo-parser/lib/libgumbo.a)
endif ()

ExternalProject_Add(
        gumbo-query
        PREFIX ${GUMBO_QUERY_ROOT}
        GIT_REPOSITORY https://github.com/Yanick-Salzmann/gumbo-query.git
        GIT_TAG master
        UPDATE_COMMAND ""
        PATCH_COMMAND ""
        SOURCE_DIR ${GUMBO_QUERY_ROOT}/src/gumbo-query
        CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/gumbo-query
        -DGumbo_INCLUDE_DIR:PATH=${CMAKE_CURRENT_BINARY_DIR}/gumbo-parser/include/gumbo
        -DGumbo_LIBRARY:FILE=${GUMBO_LIBRARY_PATH}
        -DGumbo_static_LIBRARY:FILEPATH=${GUMBO_LIBRARY_PATH}
        -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
        -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
)

ExternalProject_Add(
        gumbo
        PREFIX ${GUMBO_ROOT}
        GIT_REPOSITORY https://github.com/Yanick-Salzmann/gumbo-parser.git
        GIT_TAG master
        UPDATE_COMMAND ""
        PATCH_COMMAND ""
        SOURCE_DIR ${GUMBO_ROOT}/src/gumbo-parser
        CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/gumbo-parser
        -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
        -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
)

add_dependencies(gumbo-query gumbo)

ExternalProject_Add(
        protobuf
        PREFIX ${PROTOBUF_ROOT}
        GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
        GIT_TAG master
        UPDATE_COMMAND ""
        PATCH_COMMAND ""
        SOURCE_DIR ${PROTOBUF_ROOT}/src/protobuf
        SOURCE_SUBDIR cmake
        CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/protobuf
        -Dprotobuf_BUILD_TESTS:BOOL=OFF
        -Dprotobuf_BUILD_CONFORMANCE:BOOL=OFF
        -Dprotobuf_BUILD_EXAMPLES:BOOL=OFF
        -Dprotobuf_BUILD_SHARED_LIBS:BOOL=OFF
        -Dprotobuf_MSVC_STATIC_RUNTIME:BOOL=OFF
        -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
        -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
)

ExternalProject_Add(
        abseil
        PREFIX ${ABSEIL_ROOT}
        GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
        GIT_TAG master
        UPDATE_COMMAND ""
        PATCH_COMMAND ""
        SOURCE_DIR ${ABSEIL_ROOT}/src/abseil-cpp
        CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/abseil
        -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
        -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
)

check_include_file("filesystem" HAVE_FILESYSTEM_HEADER)

if ("${HAVE_FILESYSTEM_HEADER}" STREQUAL "")
    ExternalProject_Add(
            filesystem
            PREFIX ${FS_ROOT}
            GIT_REPOSITORY https://github.com/gulrak/filesystem.git
            GIT_TAG master
            UPDATE_COMMAND ""
            SOURCE_DIR ${FS_ROOT}/src/filesystem
            CMAKE_CACHE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/filesystem
            -DBUILD_TESTING:BOOL=FALSE
            -DBUILD_EXAMPLES:BOOL=FALSE
            -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
            -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
    )
endif ()
