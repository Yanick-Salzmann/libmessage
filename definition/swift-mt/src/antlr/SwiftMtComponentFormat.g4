grammar SwiftMtComponentFormat;

@parser::header {
#include "proto/SwiftMtComponentDefinition.pb.h"
#include "SwiftMtComponentFormatLexer.h"

#include <stack>
#include <vector>
}

@parser::members {
    class DefaultErrorListener : public antlr4::BaseErrorListener {
    private:
        std::vector<std::string>& _errors;
    public:
        explicit DefaultErrorListener(std::vector<std::string>& errors) : _errors(errors) { }

        void syntaxError(Recognizer *recognizer, antlr4::Token * offendingSymbol, size_t line, size_t charPositionInLine,
                               const std::string &msg, std::exception_ptr e) override {
            _errors.push_back(msg);
        }
    };

    DefaultErrorListener _error_listener { _errors };

    std::vector<std::string> _errors;

    bool _is_in_line = false;

    std::stack<ComponentContent> _component_stack;
    std::stack<ComponentContent> _complex_line_stack;

    SwiftMtComponentDefinition _components;
    ComplexLineType _complex_line;

    bool process(std::vector<std::string>& errors) {
        _errors.clear();

        removeErrorListeners();
        addErrorListener(&_error_listener);

        _components = SwiftMtComponentDefinition{};

        comp_format();

        if(!_errors.empty()) {
            errors.insert(errors.end(), _errors.begin(), _errors.end());
            return false;
        }

        return true;
    }

    static bool process_format(const std::string& format, std::vector<std::string>& errors, SwiftMtComponentDefinition& component_format) {
        antlr4::ANTLRInputStream stream{format};
        SwiftMtComponentFormatLexer lexer{&stream};
        antlr4::CommonTokenStream token_stream{&lexer};

        SwiftMtComponentFormatParser parser{&token_stream};
        if(!parser.process(errors)) {
            return false;
        }

        component_format = parser._components;
        return true;
    }

    [[nodiscard]] const std::vector<std::string>& errors() const { return _errors; }

    ComponentContent& current_comp() {
        if (!_is_in_line) {
            return _component_stack.top();
        } else {
            return _complex_line_stack.top();
        }
    }

    void new_component() {
        if (!_is_in_line) {
            _component_stack.push(ComponentContent{});
        } else {
            _complex_line_stack.push(ComponentContent{});
        }
    }

    void end_component() {
        if (!_is_in_line) {
            const auto comp = _component_stack.top();
            _component_stack.pop();
            if(_component_stack.empty()) {
                _components.add_formats()->MergeFrom(comp);
            } else {
                _component_stack.top().add_formats()->mutable_child()->MergeFrom(comp);
            }
        } else {
            const auto comp = _complex_line_stack.top();
            _complex_line_stack.pop();

            if(_complex_line_stack.empty()) {
                _complex_line.add_formats()->MergeFrom(comp);
            } else {
                _complex_line_stack.top().add_formats()->mutable_child()->MergeFrom(comp);
            }
        }
    }

    static auto map_char_set(char chrset) -> ValueEntry::Charset {
        switch(chrset) {
            case 'n': return ValueEntry_Charset_NUMERIC;
            case 'a': return ValueEntry_Charset_ALPHA;
            case 'c': return ValueEntry_Charset_ALPHA_NUMERIC;
            case 'h': return ValueEntry_Charset_HEX;
            case 'x': return ValueEntry_Charset_X;
            case 'y': return ValueEntry_Charset_Y;
            case 'z': return ValueEntry_Charset_Z;
            case 'd': return ValueEntry_Charset_DECIMAL;
            case 'e': return ValueEntry_Charset_BLANK;
            default: throw std::runtime_error("Invalid charset type");
        }
    }
}

comp_format:        ('Empty field' | comp | opt_comp)+;

comp
@init { new_component(); }
@after { end_component(); } :
                        separator? { if($separator.ctx) { current_comp().set_separator_before($separator.text); } }
                        (
                            char_range |
                            exact_chars |
                            multi_line |
                            max_chars |
                            'ISIN' |
                            'N'
                        )
                        separator? { if($separator.ctx) { current_comp().set_separator_after($separator.text); } }
                    ;

opt_comp
@init {
    new_component();
    current_comp().set_optional(true);
}
@after {
    end_component();
}
                    :
                    separator?
                    '['
                        separator? { if($separator.ctx) { current_comp().set_separator_before($separator.text); } }
                        opt_comp_cttnt
                        separator? { if($separator.ctx) { current_comp().set_separator_after($separator.text); } }
                    ']'
                    separator?;

opt_comp_cttnt:     (char_range | exact_chars | multi_line | max_chars | opt_comp | 'ISIN' | 'N' | comp)+;

char_range locals [ ValueEntry* val = nullptr ]
@init {
    ComponentContentFormat* formats = current_comp().add_formats();
    $ctx->val = formats->mutable_value();
}
                    :
                    range {
                        $ctx->val->mutable_range_count()->set_min_count($range.ctx->rng_min);
                        $ctx->val->mutable_range_count()->set_max_count($range.ctx->rng_max);
                    }
                    chr_class {
                        $ctx->val->set_charset(map_char_set($chr_class.text[0]));
                    };

exact_chars locals [ ValueEntry* val = nullptr ]
@init {
    ComponentContentFormat* formats = current_comp().add_formats();
    $ctx->val = formats->mutable_value();
}
                    :
                    exact_count {
                        $ctx->val->set_exact_count($exact_count.ctx->count);
                    }
                    chr_class{
                        $ctx->val->set_charset(map_char_set($chr_class.text[0]));
                    };

multi_line locals [ ValueEntry* val = nullptr ]
@init {
    ComponentContentFormat* formats = current_comp().add_formats();
    $ctx->val = formats->mutable_value();
}
                    : lines[$ctx->val];

max_chars locals [ ValueEntry* val = nullptr ]
@init {
    ComponentContentFormat* formats = current_comp().add_formats();
    $ctx->val = formats->mutable_value();
}
                    :
                    max_range { $ctx->val->set_max_count($max_range.ctx->max_size); }
                    chr_class {
                        $ctx->val->set_charset(map_char_set($chr_class.text[0]));
                    };

range locals [ int64_t rng_min = 0u, int64_t rng_max = 0u ] :
                    cardinality { $ctx->rng_min = std::stoull($text); } '-' cardinality { $ctx->rng_max = std::stoull($text); };

exact_count locals [ int64_t count = 0u ] :
                    cardinality { $ctx->count = std::stoull($text); } '!';

lines
        [ValueEntry* val]
        locals [ int64_t line_count = 0u, int64_t line_size = 0u ] :
                    cardinality { $ctx->line_count = std::stoull($text); } '*'
                    (
                        (
                            cardinality {
                                val->mutable_line_count()->set_num_lines($ctx->line_count);
                                val->mutable_line_count()->set_line_size(std::stoull($text));
                            }

                            chr_class {
                                val->set_charset(map_char_set($chr_class.text[0]));
                            }
                        )
                        |
                        complex_line {
                            val->mutable_line_count()->set_num_lines($ctx->line_count);
                            val->mutable_line_count()->mutable_complex_type()->MergeFrom(_complex_line);
                        }
                    );

max_range locals [ int64_t max_size = 0u ] :
                    cardinality { $ctx->max_size = std::stoull($text); };

complex_line
@init {
    _complex_line = ComplexLineType{};
    _is_in_line = true;
}
@after {
    _is_in_line = false;
}
                    : '(' comp_format ')';

chr_class:          CHR_NUMERIC | CHR_ALPHA | CHR_ALPHA_NUM |
                    CHR_HEX | CHR_X | CHR_Y | CHR_Z | CHR_DECIMAL |
                    CHR_BLANK;

cardinality:        DIGIT DIGIT*;

separator:          SEPARATOR | CRLF;

CHR_NUMERIC:        'n';
CHR_ALPHA:          'a';
CHR_ALPHA_NUM:      'c';
CHR_HEX:            'h';
CHR_X:              'x';
CHR_Y:              'y';
CHR_Z:              'z';
CHR_DECIMAL:        'd';
CHR_BLANK:          'e';

CRLF                '\r' '\n';
DIGIT:              '0'..'9';
SEPARATOR:          . ;

