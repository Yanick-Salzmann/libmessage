#include "http/HttpClient.hpp"
#include <string/conversion.hpp>
#include <stdexcept>

namespace message::utils::http {
    LOGGER_IMPL(HttpClient);

    HttpClient::HttpClient() {
        _curl = curl_easy_init();
        curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYHOST, 0);
        curl_easy_setopt(_curl, CURLOPT_COOKIEFILE, "");
        curl_easy_setopt(_curl, CURLOPT_ERRORBUFFER, _curl_error_buffer);
        curl_easy_setopt(_curl, CURLOPT_WRITEDATA, this);
        curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, HttpClient::write_callback);
    }

    HttpClient::~HttpClient() {
        curl_easy_cleanup(_curl);
    }

    void HttpClient::url(const std::string &url) {
        curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
        _url = url;
    }

    void HttpClient::execute() {
        log->debug("Fetching {}...", _url);
        const auto result = curl_easy_perform(_curl);
        if(result != CURLE_OK) {
            _curl_error_buffer[sizeof(_curl_error_buffer) - 1] = '\0';

            log->warn("Error execution cURL request: {} (detail: {})", curl_easy_strerror(result), _curl_error_buffer);
            throw std::runtime_error("Error executing cURL request");
        }
    }

    std::size_t HttpClient::write_callback(void const* buffer, std::size_t element_size, std::size_t elements, void* user_data) {
        if(user_data == nullptr) {
            return element_size * elements;
        }

        auto* client = (HttpClient*) user_data;

        const auto start = (const uint8_t*) buffer;
        const auto end = start + (element_size * elements);

        client->_buffer.insert(client->_buffer.end(), start, end);

        return element_size * elements;
    }

    std::string HttpClient::response_to_string_utf8() const {
        return string::to_string_utf8(_buffer);
    }

    std::string HttpClient::download_string(const std::string &url) {
        HttpClient client{};
        client.url(url);
        client.execute();

        return client.response_to_string_utf8();
    }
}
