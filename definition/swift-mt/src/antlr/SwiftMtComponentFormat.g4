grammar SwiftMtComponentFormat;

comp:               (char_range | exact_chars | multi_line | max_chars);

opt_comp:           '[' opt_comp_cttnt ']';

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

