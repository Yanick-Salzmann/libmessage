#include "string/comparison.hpp"
#include <absl/strings/match.h>
#include "string/conversion.hpp"

namespace message::utils::string {

    bool contains(const std::string &str, const std::string &search) {
        return absl::StrContains(str, search);
    }

    bool contains_ignore_case(const std::string &str, const std::string &search) {
        return contains(to_lower(str), to_lower(search));
    }
}