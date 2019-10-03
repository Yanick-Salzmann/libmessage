#ifndef LIBMESSAGE_MESSAGEINDEXPARSER_HPP
#define LIBMESSAGE_MESSAGEINDEXPARSER_HPP

#undef min
#include "proto/SwiftMtMsgDefinition.pb.h"
#include "SequenceStack.hpp"
#include "ComponentFormatStack.hpp"
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

        struct ComponentSeparatorEntry {
            std::string separator_before;
            std::vector<ValueEntry> formats;
            std::string separator_after;
        };

        const std::regex SEQUENCE_REGEX{"(-+> )?((Mandatory )|(Optional ))(Repetitive )?(Subs|S)equence ([^ ]+) ?(.*)?"};
        const std::regex SEQUENCE_END_REGEX{"(-+\\| )?End of (Subs|S)equence ([^ ]+) ?(.*)?"};

        const std::regex REPETITION_START{"-+>\\s*"};
        const std::regex REPETITION_END{"-+\\|\\s*"};

        const std::regex QUALIFIER_OPTIONS{"(([A-Z])(, )?(or )?)"};

        const std::regex COMPONENT_NAME_MATCHER{"([^(]+)?\\(([^)]+)\\)([^(]+)?"};

        web::WebDefinitionRepository& _repository;
        web::MessageIndex& _index;

        std::string _cache_path;

        [[nodiscard]] auto is_cached(const web::MessageIndexEntry& entry) const -> bool;
        void put_in_cache(const web::MessageIndexEntry& entry, const SwiftMtMessageDefinition& definition) const;

        void process_definition(const web::MessageIndexEntry& entry);
        auto parse_definition_from_detail_page(const web::MessageIndexEntry& entry, const std::string& detail_content) -> SwiftMtMessageDefinition;

        static void handle_sequence_start(const std::smatch& match, SequenceStack& sequence_stack);
        static void handle_sequence_end(SequenceStack& sequence_stack, SwiftMtMessageDefinition& msg_def);

        static void handle_repetition_end(const RepetitionDef& repetition, SequenceStack& sequence_stack, SwiftMtMessageDefinition& msg_def);

        static auto fields_from_row(const utils::http::HtmlNode& row, bool is_first_row, std::vector<utils::http::HtmlNode>& nodes) -> std::vector<std::string>;

        static auto parse_field_indices(const std::vector<std::string>& names, RowMetaData& meta_data) -> bool;

        auto handle_common_field(SequenceStack& sequence_stack, RepetitionDef& repetition, bool is_in_repetition, SwiftMtMessageDefinition& msg_def, RowMetaData& meta_data, const utils::http::HtmlNode& row, bool is_first_row) -> bool;

        void handle_field_details(ObjectDef* obj, const std::string& detail_link);

        void load_options(ObjectDef *obj, const utils::http::HtmlDocument &document);
        void load_qualifiers(ObjectDef* obj, const utils::http::HtmlDocument& document);

        void load_component_names(OptionDef* optn, const std::string& components);
        static void load_component_formats(OptionDef* optn, const std::string& format, const std::string& tag);

        auto select_fields_as_string(const utils::http::ISelectable& element, const std::string& selector) -> std::vector<std::string>;
        auto select_fields_as_string(const utils::http::ISelectable& element, const std::string& selector, std::vector<utils::http::HtmlNode>& nodes) -> std::vector<std::string>;

        static auto load_components_by_separator(const std::list<ComponentFormatStack::ComponentFormatEntry>& formats) -> std::vector<ComponentSeparatorEntry>;

        [[nodiscard]] auto convert_children_to_string(const utils::http::HtmlNode& node, bool crlf = true) const -> std::string;

        template<typename T>
        static bool advance_to_next_separator(T& start, const T& end, const std::string& separator) {
            while(start != end && !(*start).is_separator() && (*start).separator() != separator) {
                ++start;
            }

            return start != end;
        }

    public:
        /**
         * Initializes this message index parser with the online repository to use, the index containing
         * all resolved message types that should be processed as well as the cache path where the
         * already parsed definitions will be searched and saved once they are loaded.
         *
         * @param repository    Web repository for the service release to parse from.
         * @param index         Message index containing all available message types
         * @param cache_path    Root path of the cache folder (not including the service release)
         */
        MessageIndexParser(web::WebDefinitionRepository& repository, web::MessageIndex& index, std::string cache_path) : _repository(repository), _index(index), _cache_path(std::move(cache_path)) {}

        /**
         * Processes all definitions found in the message index and generates the
         * JSON representation of the protobuffer definition in the cache path according
         * to service release and message type
         */
        void process_definitions();
    };
}


#endif //LIBMESSAGE_MESSAGEINDEXPARSER_HPP
