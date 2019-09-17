#ifndef LIBMESSAGE_STRING_COMPARISON_HPP
#define LIBMESSAGE_STRING_COMPARISON_HPP

#include <string>

namespace message::utils::string {
    bool contains_ignore_case(const std::string& str, const std::string& search);
    bool contains(const std::string& str, const std::string& search);

    bool starts_with(const std::string& str, const std::string& prefix);
    bool starts_with_ignore_case(const std::string& str, const std::string& prefix);
}

#endif //LIBMESSAGE_STRING_COMPARISON_HPP
