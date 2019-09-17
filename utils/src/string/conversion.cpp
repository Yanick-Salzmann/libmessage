#include "string/conversion.hpp"

#include <algorithm>

namespace message::utils::string {
    std::string to_string_utf8(const std::vector<uint8_t> &data) {
        const auto *ptr = (const char *) data.data();
        return std::string{ptr, ptr + data.size()};
    }

    std::string to_lower(const std::string &value) {
        std::string ret;
        std::transform(value.begin(), value.end(), std::back_inserter(ret), [](const auto& chr) { return std::tolower(chr); });
        return ret;
    }

    std::string to_upper(const std::string &value) {
        std::string ret;
        std::transform(value.begin(), value.end(), std::back_inserter(ret), [](const auto& chr) { return std::toupper(chr); });
        return ret;
    }
}

