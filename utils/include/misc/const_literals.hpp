#ifndef LIBMESSAGE_CONST_LITERALS_HPP
#define LIBMESSAGE_CONST_LITERALS_HPP

#include <string>
#include <cstdint>
#include <cstring>

inline constexpr auto operator "" _z(unsigned long long n) -> std::size_t {
    return static_cast<std::size_t>(n);
}

#endif //LIBMESSAGE_CONST_LITERALS_HPP
