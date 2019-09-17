#ifndef LIBMESSAGE_MESSAGEINDEXPARSER_HPP
#define LIBMESSAGE_MESSAGEINDEXPARSER_HPP

#include "proto/SwiftMtMsgDefinition.pb.h"
#include "SequenceStack.hpp"
#include <log/Log.hpp>

#include <list>
#include <regex>
#include <http/HtmlDocument.hpp>

namespace message::definition::swift::mt::web {
    class MessageIndex;
    class MessageIndexEntry;
    class WebDefinitionRepository;
}

namespace message::definition::swift::mt::definition {
    class MessageIndexParser {
        LOGGER;

        struct RowMetaData {
            uint32_t idx_tag = 0, idx_name = 0, idx_link = 0, idx_optnl = 0;
            uint32_t max_field_index = 0;
        };

        const std::regex SEQUENCE_REGEX{"(-+> )?((Mandatory )|(Optional ))(Repetitive )?(Subs|S)equence ([^ ]+) ?(.*)?"};
        const std::regex SEQUENCE_END_REGEX{"(-+\\| )?End of (Subs|S)equence ([^ ]+) ?(.*)?"};

        const std::regex REPETITION_START{"-+>\\s*"};
        const std::regex REPETITION_END{"-+\\|\\s*"};

        const std::regex QUALIFIER_OPTIONS{"(([A-Z])(, )?(or )?)"};

        web::WebDefinitionRepository& _repository;
        web::MessageIndex& _index;

        std::string _cache_path;

        [[nodiscard]] bool is_cached(const web::MessageIndexEntry& entry) const;
        void put_in_cache(const web::MessageIndexEntry& entry, const SwiftMtMessageDefinition& definition) const;

        void process_definition(const web::MessageIndexEntry& entry);
        SwiftMtMessageDefinition parse_definition_from_detail_page(const web::MessageIndexEntry& entry, const std::string& detail_content);

        static void handle_sequence_start(const std::smatch& match, SequenceStack& sequence_stack);
        static void handle_sequence_end(SequenceStack& sequence_stack, SwiftMtMessageDefinition& msg_def);

        static void handle_repetition_end(const RepetitionDef& repetition, SequenceStack& sequence_stack, SwiftMtMessageDefinition& msg_def);

        static std::vector<std::string> fields_from_row(const utils::http::HtmlNode& row, bool is_first_row, std::vector<utils::http::HtmlNode>& nodes);

        static bool parse_field_indices(const std::vector<std::string>& names, RowMetaData& meta_data);

        bool handle_common_field(SequenceStack& sequence_stack, RepetitionDef& repetition, bool is_in_repetition, SwiftMtMessageDefinition& msg_def, RowMetaData& meta_data, const utils::http::HtmlNode& row, bool is_first_row);

        void handle_field_details(ObjectDef* obj, const std::string& detail_link);

        void load_component_formats(ObjectDef* obj, const utils::http::HtmlDocument& document);
        void load_qualifiers(ObjectDef* obj, const utils::http::HtmlDocument& document);

        std::vector<std::string> select_fields_as_string(const utils::http::ISelectable& element, const std::string& selector);

    public:
        MessageIndexParser(web::WebDefinitionRepository& repository, web::MessageIndex& index, std::string cache_path) : _repository(repository), _index(index), _cache_path(std::move(cache_path)) {}

        void process_definitions();
    };
}


#endif //LIBMESSAGE_MESSAGEINDEXPARSER_HPP