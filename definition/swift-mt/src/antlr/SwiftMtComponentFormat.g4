grammar SwiftMtComponentFormat;

@parser::header {
#include "proto/SwiftMtComponentDefinition.pb.h"
#include <stack>
}

@parser::members {
    std::stack<ComponentContent> _component_stack;
    SwiftMtComponentDefinition _components;

    ComponentContent& current_comp() {
        return _component_stack.top();
    }

    void new_component() {
        _component_stack.push(ComponentContent{});
    }

    void end_component() {
        const auto comp = _component_stack.top();
        _component_stack.pop();
        if(_component_stack.empty()) {
            _components.add_formats()->MergeFrom(comp);
        } else {
            _component_stack.top().add_formats()->MergeFrom(comp);
        }
    }
}

comp_format:        (comp | opt_comp)+;

comp
@init { new_component(); }
                    : separator? { if($separator.ctx) { current_comp().set_separator_before($separator.text); } } (char_range | exact_chars | multi_line | max_chars) separator?;

opt_comp:           '[' separator? opt_comp_cttnt separator? ']';

opt_comp_cttnt:     (char_range | exact_chars | multi_line | max_chars | opt_comp | comp)+;

char_range:         range chr_class;
exact_chars:        exact_count chr_class;
multi_line:         lines chr_class;
max_chars:          max_range chr_class;

range:              cardinality '-' cardinality;
exact_count:        cardinality '!';
lines:              cardinality '*' cardinality;
max_range:          cardinality;

chr_class:          CHR_NUMERIC | CHR_ALPHA | CHR_ALPHA_NUM |
                    CHR_HEX | CHR_X | CHR_Y | CHR_Z | CHR_DECIMAL |
                    CHR_BLANK;

cardinality:        DIGIT DIGIT?;

separator:          SEPARATOR+;

CHR_NUMERIC:        'n';
CHR_ALPHA:          'a';
CHR_ALPHA_NUM:      'c';
CHR_HEX:            'h';
CHR_X:              'x';
CHR_Y:              'y';
CHR_Z:              'z';
CHR_DECIMAL:        'd';
CHR_BLANK:          'e';

DIGIT:              '0'..'9';
SEPARATOR:          . ;

