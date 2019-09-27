#ifndef LIBMESSAGE_CONST_LITERALS_HPP
#define LIBMESSAGE_CONST_LITERALS_HPP

#include <string>
#include <cstdint>

inline constexpr auto operator "" _z(uint64_t value) -> std::size_t {
    return static_cast<std::size_t>(value);
}

#endif //LIBMESSAGE_CONST_LITERALS_HPP
