cmake_minimum_required(VERSION 3.14)

project(Utils)

add_library(utils STATIC include/http/HttpClient.hpp src/http/HttpClient.cpp src/log/Log.cpp include/log/Log.hpp src/string/conversion.cpp include/string/conversion.hpp include/http/HtmlDocument.hpp src/http/HtmlDocument.cpp include/string/comparison.hpp src/string/comparison.cpp)

add_dependencies(utils curl spdlog gumbo-query abseil)

target_include_directories(utils PUBLIC ${CMAKE_BINARY_DIR}/third_party/libcurl/include)
target_include_directories(utils PUBLIC ${CMAKE_BINARY_DIR}/third_party/spdlog/include)
target_include_directories(utils PUBLIC ${CMAKE_BINARY_DIR}/third_party/gumbo-parser/include/gumbo)
target_include_directories(utils PUBLIC ${CMAKE_BINARY_DIR}/third_party/gumbo-query/include)

target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_definitions(utils PUBLIC "-DCURL_STATICLIB")

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/third_party/abseil/lib/cmake/absl")

find_package(absl REQUIRED)

target_include_directories(utils PUBLIC absl::base)

find_package(OpenSSL)

if (${OpenSSL_FOUND})
    message("Linking against OpenSSL ${OPENSSL_VERSION}")
    set(SSL_LIBRARIES ${OPENSSL_LIBRARIES})
else ()
    message("Linking against source build of OpenSSL")
    set(SSL_LIBRARIES "${CMAKE_SOURCE_DIR}/third_party/openssl/src/openssl/dist/lib/libcrypto.lib;${CMAKE_SOURCE_DIR}/third_party/openssl/src/openssl/dist/lib/libssl.lib")
endif ()

message("OpenSSL Libraries: ${SSL_LIBRARIES}")

if (WIN32)
    target_link_libraries(utils
            ${CMAKE_BINARY_DIR}/third_party/libcurl/lib/libcurl-d.lib
            ${SSL_LIBRARIES}
            ${CMAKE_BINARY_DIR}/third_party/gumbo-query/lib/gq.lib
            ${CMAKE_BINARY_DIR}/third_party/gumbo-parser/lib/gumbo.lib
            absl::base absl::strings absl::algorithm absl::utility absl::strings_internal
            dbghelp
            ws2_32.lib
            wldap32.lib)
endif ()
