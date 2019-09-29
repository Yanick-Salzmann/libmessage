#ifndef LIBMESSAGE_COMPONENTFORMATSTACK_HPP
#define LIBMESSAGE_COMPONENTFORMATSTACK_HPP

#include "proto/SwiftMtComponentDefinition.pb.h"

#include <functional>
#include <list>

namespace message::definition::swift::mt {
    class ComponentFormatStack {
        class ComponentFormatEntry {
            std::string _separator;
            ValueEntry _element;

            bool _is_separator = false;
            bool _is_optional = false;

        public:
            explicit ComponentFormatEntry(std::string separator) : _separator(std::move(separator)), _is_separator(true) { }
            explicit ComponentFormatEntry(ValueEntry element, bool optional) : _element(std::move(element)), _is_optional(optional) { }

            bool is_separator() const {
                return _is_separator;
            }
        };

        SwiftMtComponentDefinition _root_component;

        std::list<ComponentFormatEntry> _rule_list;

        void add_non_empty_separator(const std::string& separator) {
            if(separator.empty()) {
                return;
            }

            _rule_list.emplace_back(separator);
        }

        void add_value_entry(ValueEntry entry, bool is_optional) {
            _rule_list.emplace_back(std::move(entry), is_optional);
        }

        void process_format_entry(const ComponentContent& format);
        void process_format_value(const ComponentContentFormat& format, bool is_optional);

        void flat_map_formats();

        auto count_child_formats(const ComponentContent& format) const -> std::size_t;

    public:
        explicit ComponentFormatStack(SwiftMtComponentDefinition root_component);

        auto total_format_count() const -> std::size_t;

        const std::list<ComponentFormatEntry>& format_list() const {
            return _rule_list;
        }
    };

}

#endif //LIBMESSAGE_COMPONENTFORMATSTACK_HPP
