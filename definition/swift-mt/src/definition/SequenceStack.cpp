#include "SequenceStack.hpp"

namespace message::definition::swift::mt {
    LOGGER_IMPL(SequenceStack);

    void SequenceStack::add_field(const FieldDef &obj_def) {
        if (_sequences.empty()) {
            log->error("Imbalanced sequences, cannot add field {}", obj_def.tag());
            throw std::runtime_error("Sequence imbalanced");
        }

        auto obj = _sequences.top().add_objects();
        obj->set_tag(obj_def.tag());
        obj->mutable_field()->MergeFrom(obj_def);
    }

    void SequenceStack::add_sequence(const SequenceDef &sequence) {
        _sequences.push(sequence);
    }

    SequenceDef SequenceStack::pop_sequence() {
        const auto ret = top();
        _sequences.pop();
        return ret;
    }

    void SequenceStack::add_repetition(const RepetitionDef &repetition_def) {
        if (_sequences.empty()) {
            log->error("Imbalanced sequences, cannot add repetition");
            throw std::runtime_error("Sequence imbalanced");
        }

        auto obj = _sequences.top().add_objects();
        obj->mutable_repetition()->MergeFrom(repetition_def);
    }

    void SequenceStack::add_object(const ObjectDef &object) {
        create_object()->MergeFrom(object);
    }

    ObjectDef *SequenceStack::create_object() {
        if (_sequences.empty()) {
            log->error("Imbalanced sequences, cannot add object");
            throw std::runtime_error("Sequence imbalanced");
        }

        return _sequences.top().add_objects();
    }
}
