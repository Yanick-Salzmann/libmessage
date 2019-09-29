#ifndef LIBMESSAGE_COMPONENTNAMESTACK_HPP
#define LIBMESSAGE_COMPONENTNAMESTACK_HPP

#include <list>
#include "proto/SwiftMtMsgDefinition.pb.h"

namespace message::definition::swift::mt {
    class ComponentNameStack {
    public:
        class ComponentNameEntry {
            ComponentName _name{};
            std::string _separator{};

            bool _is_separator = false;

        public:
            explicit ComponentNameEntry(std::string separator) : _separator(std::move(separator)), _is_separator(true) { }
            explicit ComponentNameEntry(ComponentName name) : _name(std::move(name)), _is_separator(false) { }
        };

    private:

        std::list<ComponentNameEntry> _names{};

        void add_non_empty_separator(const std::string& separator) {
            if(separator.empty()) {
                return;
            }

            _names.emplace_back(separator);
        }

        void process_name(const ComponentName& name);

    public:
        template<typename T>
        explicit ComponentNameStack(const T& names) {
            for(const auto& name : names) {
                process_name(name);
            }
        }

        [[nodiscard]] auto names() const -> const std::list<ComponentNameEntry>& {
            return _names;
        }
    };
}


#endif //LIBMESSAGE_COMPONENTNAMESTACK_HPP
