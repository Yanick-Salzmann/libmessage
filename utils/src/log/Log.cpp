#include "log/Log.hpp"

#ifdef __GNUG__
#include <cstdlib>
#include <memory>
#include <cxxabi.h>

std::string demangle(const char* name) {

    int status = -4; // some arbitrary value to eliminate the compiler warning

    // enable c++11 by passing the flag -std=c++11 to g++
    std::unique_ptr<char, void(*)(void*)> res {
            abi::__cxa_demangle(name, NULL, NULL, &status),
            std::free
    };

    return (status==0) ? res.get() : name ;
}

#else
#include <dbghelp.h>

std::string demangle(const char* name) {
    char undecorated_name[5000]{};
    if(!UnDecorateSymbolName(name, undecorated_name, sizeof(undecorated_name), UNDNAME_COMPLETE)) {
        return name;
    }

    undecorated_name[4999] = '\0';
    return undecorated_name;
}

#endif

namespace message::utils::log {
    void Logger::init_logger (const std::string& name) {
        _logger = spdlog::stdout_color_mt (name);
        _logger->set_pattern ("[%Y-%m-%d %H:%I:%S.%e] [%P:%t] [%n] %^[%l]%$ %v");
#ifndef NDEBUG
        //_logger->set_level (spdlog::level::debug);
        _logger->set_level (spdlog::level::info);
#else
        _logger->set_level (spdlog::level::info);
#endif
    }

    std::string Logger::sanitize_type_name(const std::string &name) {
        auto actual_name = demangle(name.c_str());

        const auto idx_space = actual_name.find(' ');
        if(idx_space == std::string::npos) {
            return actual_name;
        }

        return actual_name.substr(idx_space + 1);
    }
}