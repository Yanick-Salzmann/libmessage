#ifndef LIBMESSAGE_CONVERSION_HPP
#define LIBMESSAGE_CONVERSION_HPP

#include <string>
#include <vector>
#include <sstream>

namespace message::utils::string {
    std::string to_string_utf8(const std::vector<uint8_t>& data);
    std::string to_lower(const std::string& value);
    std::string to_upper(const std::string& value);

    template<typename T>
    inline std::string join(const T& container) {
        std::stringstream stream{};
        for(const auto& element : container) {
            stream << element;
        }

        return stream.str();
    }

    template<typename T>
    inline auto join(const T& container, const char& delimiter) -> std::string {
        std::stringstream stream{};
        auto first = true;

        for(const auto& element : container) {
            if(first) {
                first = false;
            } else {
                stream << delimiter;
            }

            stream << element;
        }

        return stream.str();
    }

    template<typename T>
    inline auto join(const T& container, const std::string& delimiter) -> std::string {
        std::stringstream stream{};
        auto first = true;

        for(const auto& element : container) {
            if(first) {
                first = false;
            } else {
                stream << delimiter;
            }

            stream << element;
        }

        return stream.str();
    }
}

#endif //LIBMESSAGE_CONVERSION_HPP
