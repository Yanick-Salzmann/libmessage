#ifndef LIBMESSAGE_SEQUENCESTACK_HPP
#define LIBMESSAGE_SEQUENCESTACK_HPP

#include "proto/SwiftMtMsgDefinition.pb.h"
#include "log/Log.hpp"

#include <stack>

namespace message::definition::swift::mt {
    class SequenceStack {
        LOGGER;

        std::stack<SequenceDef> _sequences;

    public:
        void add_field(const FieldDef& obj_def);
        void add_repetition(const RepetitionDef& repetition_def);
        void add_sequence(const SequenceDef& sequence);
        void add_object(const ObjectDef& object);

        ObjectDef* create_object();

        [[nodiscard]] SequenceDef& top() {
            return _sequences.top();
        }

        [[nodiscard]] SequenceDef pop_sequence();

        [[nodiscard]] bool is_empty() const {
            return _sequences.empty();
        }
    };
}


#endif //LIBMESSAGE_SEQUENCESTACK_HPP
