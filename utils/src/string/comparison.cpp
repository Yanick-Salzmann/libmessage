#include "string/comparison.hpp"
#include "string/conversion.hpp"

namespace message::utils::string {

    bool contains(const std::string &str, const std::string &search) {
        return str.find(search) != std::string::npos;
    }

    bool contains_ignore_case(const std::string &str, const std::string &search) {
        return contains(to_lower(str), to_lower(search));
    }
}