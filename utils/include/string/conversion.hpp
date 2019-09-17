#ifndef LIBMESSAGE_CONVERSION_HPP
#define LIBMESSAGE_CONVERSION_HPP

#include <string>
#include <vector>

namespace message::utils::string {
    std::string to_string_utf8(const std::vector<uint8_t>& data);
    std::string to_lower(const std::string& value);
    std::string to_upper(const std::string& value);
}

#endif //LIBMESSAGE_CONVERSION_HPP
