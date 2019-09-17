#ifndef LIBMESSAGE_HTTPCLIENT_HPP
#define LIBMESSAGE_HTTPCLIENT_HPP

#include <curl/curl.h>
#include <string>
#include "log/Log.hpp"

namespace message::utils::http {
    class HttpClient {
        LOGGER;

        static std::size_t write_callback(void const* buffer, std::size_t element_size, std::size_t elements, void* user_data);

        std::vector<uint8_t> _buffer;
        std::string _url{};

        char _curl_error_buffer[CURL_ERROR_SIZE]{};

        CURL* _curl;

    public:
        HttpClient();
        HttpClient(const HttpClient&) = delete;
        HttpClient(HttpClient&&) = delete;

        ~HttpClient();

        void operator = (const HttpClient&) = delete;
        void operator = (HttpClient&&) = delete;

        void url(const std::string& url);

        void execute();

        [[nodiscard]] std::string response_to_string_utf8() const;

        static std::string download_string(const std::string& url);
    };
}


#endif //LIBMESSAGE_HTTPCLIENT_HPP
