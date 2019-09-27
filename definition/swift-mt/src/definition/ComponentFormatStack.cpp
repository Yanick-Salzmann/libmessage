#include "ComponentFormatStack.hpp"
#include <misc/const_literals.hpp>

namespace message::definition::swift::mt {

    ComponentFormatStack::ComponentFormatStack(SwiftMtComponentDefinition root_component) : _root_component(std::move(root_component)) {
        move_initial();
    }

    void ComponentFormatStack::move_initial() {
        if(_root_component.formats().empty()) {
            _is_at_end = true;
            return;
        }

        _active_content = _root_component.formats(0);
        _increment = compute_advance(_root_component, _active_content, 1);
    }

    auto ComponentFormatStack::compute_advance(SwiftMtComponentDefinition parent, ComponentContent current, std::size_t index) -> std::function<void()> {
        if(index >= parent.formats_size()) {
            return [this]() {
                _is_at_end = true;
            };
        } else {
            return [this]() {
                _is_at_end = true;
            };
        }
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
}