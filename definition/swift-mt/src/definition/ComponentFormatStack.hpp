#ifndef LIBMESSAGE_COMPONENTFORMATSTACK_HPP
#define LIBMESSAGE_COMPONENTFORMATSTACK_HPP

#include "proto/SwiftMtComponentDefinition.pb.h"

#include <functional>

namespace message::definition::swift::mt {
    class ComponentFormatStack {
        SwiftMtComponentDefinition _root_component;

        ComponentContent _active_content{};
        std::function<void()> _increment;

        bool _is_at_end = false;

        void move_initial();

        auto compute_advance(SwiftMtComponentDefinition parent, ComponentContent current, std::size_t index) -> std::function<void()>;

        auto count_child_formats(const ComponentContent& format) const -> std::size_t;

    public:
        explicit ComponentFormatStack(SwiftMtComponentDefinition root_component);

        auto total_format_count() const -> std::size_t;
    };

}

#endif //LIBMESSAGE_COMPONENTFORMATSTACK_HPP
