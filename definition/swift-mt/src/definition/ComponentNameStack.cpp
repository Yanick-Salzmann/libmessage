#include "ComponentNameStack.hpp"

namespace message::definition::swift::mt {
    void ComponentNameStack::process_name(const ComponentName &name) {
        add_non_empty_separator(name.separator_before());
        _names.emplace_back(name);
        add_non_empty_separator(name.separator_after());
    }
}