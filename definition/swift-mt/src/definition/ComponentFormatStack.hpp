#ifndef LIBMESSAGE_COMPONENTFORMATSTACK_HPP
#define LIBMESSAGE_COMPONENTFORMATSTACK_HPP

#include "proto/SwiftMtComponentDefinition.pb.h"

#include <functional>
#include <list>

namespace message::definition::swift::mt {
    class ComponentFormatStack {
    public:
        class ComponentFormatEntry {
            std::string _separator;
            ValueEntry _element;

            bool _is_separator = false;
            bool _is_optional = false;
            uint32_t _depth = 0U;

        public:
            explicit ComponentFormatEntry(std::string separator, uint32_t depth) : _separator(std::move(separator)), _is_separator(true), _depth(depth) { }
            explicit ComponentFormatEntry(ValueEntry element, bool optional, uint32_t depth) : _element(std::move(element)), _is_optional(optional), _depth(depth) { }

            auto is_separator() const -> bool {
                return _is_separator;
            }

            auto separator() const -> const std::string& {
                return _separator;
            }

            auto value_entry() const -> const ValueEntry& {
                return _element;
            }

            auto depth() const -> uint32_t {
                return _depth;
            }
        };

    private:

        SwiftMtComponentDefinition _root_component;

        std::list<ComponentFormatEntry> _rule_list;

        void add_non_empty_separator(const std::string& separator, uint32_t depth) {
            if(separator.empty()) {
                return;
            }

            _rule_list.emplace_back(separator, depth);
        }

        void add_value_entry(ValueEntry entry, bool is_optional, uint32_t depth) {
            _rule_list.emplace_back(std::move(entry), is_optional, depth);
        }

        void process_format_entry(const ComponentContent& format, uint32_t depth);
        void process_format_value(const ComponentContentFormat& format, bool is_optional, uint32_t depth);

        void flat_map_formats();

        auto count_child_formats(const ComponentContent& format) const -> std::size_t;

    public:
        explicit ComponentFormatStack(SwiftMtComponentDefinition root_component);

        auto total_format_count() const -> std::size_t;

        [[nodiscard]] auto format_list() const -> const std::list<ComponentFormatEntry>& {
            return _rule_list;
        }
    };

}

#endif //LIBMESSAGE_COMPONENTFORMATSTACK_HPP
