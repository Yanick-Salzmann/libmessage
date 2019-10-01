#include "ComponentFormatStack.hpp"
#include <misc/const_literals.hpp>

namespace message::definition::swift::mt {

    ComponentFormatStack::ComponentFormatStack(SwiftMtComponentDefinition root_component) : _root_component(std::move(root_component)) {
        flat_map_formats();
    }

    auto ComponentFormatStack::total_format_count() const -> std::size_t {
        auto cur_count = 0_z;

        for(const auto& child : _root_component.formats()) {
            cur_count += 1_z + count_child_formats(child);
        }

        return cur_count;
    }

    auto ComponentFormatStack::count_child_formats(const ComponentContent &format) const -> std::size_t {
        auto cur_count = 0_z;

        for(const auto& child : format.formats()) {
            if(child.content_oneof_case() == ComponentContentFormat::kChild) {
                cur_count += count_child_formats(child.child());
            } else {
                ++cur_count;
            }
        }

        return cur_count;
    }

    void ComponentFormatStack::flat_map_formats() {
        for(const auto& format : _root_component.formats()) {
            process_format_entry(format, 0);
        }
    }

    void ComponentFormatStack::process_format_entry(const ComponentContent &format, uint32_t depth) {
        add_non_empty_separator(format.separator_before(), depth);

        for(const auto& entry : format.formats()) {
            process_format_value(entry, format.optional(), depth);
        }

        add_non_empty_separator(format.separator_after(), depth);
    }

    void ComponentFormatStack::process_format_value(const ComponentContentFormat &format, bool is_optional, uint32_t depth) {
        if(format.content_oneof_case() == ComponentContentFormat::kChild) {
            process_format_entry(format.child(), depth + 1);
        } else {
            add_value_entry(format.value(), is_optional, depth);
        }
    }
}