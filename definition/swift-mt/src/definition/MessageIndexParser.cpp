#include "web/MessageIndexEntry.hpp"
#include "web/MessageIndex.hpp"
#include "web/WebDefinitionRepository.hpp"
#include "MessageIndexParser.hpp"
#include "http/HtmlDocument.hpp"
#include "SequenceStack.hpp"
#include "ComponentFormatStack.hpp"
#include "ComponentNameStack.hpp"

#include <google/protobuf/util/json_util.h>
#include <fstream>

#ifndef USE_EXTERNAL_FILESYSTEM
#include <filesystem>
namespace fs = std::filesystem;
#else

#include <ghc/filesystem.hpp>

namespace fs = ghc::filesystem;
#endif

#include <string/conversion.hpp>
#include <proto/SwiftMtComponentDefinition.pb.h>

#undef ERROR

#include <antlr/SwiftMtComponentFormatParser.h>
#include <string/comparison.hpp>

namespace message::definition::swift::mt::definition {
    LOGGER_IMPL(MessageIndexParser);

    void MessageIndexParser::process_definitions() {
        const auto &messages = _index.messages();
        std::for_each(messages.begin(), messages.end(), [=](const auto &msg) { process_definition(msg); });
    }

    void MessageIndexParser::process_definition(const web::MessageIndexEntry &entry) {
        log->info("Processing message definition MT{}", entry.message_type());
        if (is_cached(entry)) {
            log->info("Definition is already in cache, skipping...");
            return;
        }

        const auto message_detail = _repository.download_topic(entry.detail_link());
        const auto definition = parse_definition_from_detail_page(entry, message_detail);
        put_in_cache(entry, definition);
    }

    void MessageIndexParser::put_in_cache(const web::MessageIndexEntry &entry, const SwiftMtMessageDefinition &definition) const {
        fs::path cache_path{_cache_path};
        cache_path /= _repository.service_release();
        cache_path /= fmt::format("mt_{}.json", entry.message_type());

        const auto parent_dir = cache_path.parent_path();
        if (!exists(parent_dir)) {
            create_directories(parent_dir);
        }

        std::string json_definition{};
        google::protobuf::util::JsonOptions json_options{};
        json_options.add_whitespace = true;
        google::protobuf::util::MessageToJsonString(definition, &json_definition, json_options);

        std::ofstream os{absolute(cache_path).string(), std::ios::binary};
        os.write(json_definition.c_str(), json_definition.size());
    }

    auto MessageIndexParser::is_cached(const web::MessageIndexEntry &entry) const -> bool {
        fs::path cache_path{_cache_path};
        cache_path /= _repository.service_release();
        cache_path /= fmt::format("mt_{}.json", entry.message_type());

        return exists(cache_path);
    }

    auto MessageIndexParser::parse_definition_from_detail_page(const web::MessageIndexEntry &entry, const std::string &detail_content) -> SwiftMtMessageDefinition {
        utils::http::HtmlDocument document{detail_content};

        const auto rows = document.select("table.fmttable tr");

        SwiftMtMessageDefinition message_def{};
        SequenceStack sequence_stack{};
        RepetitionDef current_repetition{};
        auto is_in_repetition = false;
        auto is_first_row = true;

        RowMetaData meta_data{};

        for (const auto &row : rows) {
            const auto content = row.text();
            std::smatch seq_match;

            if (std::regex_match(content, seq_match, SEQUENCE_REGEX)) {
                handle_sequence_start(seq_match, sequence_stack);
                continue;
            }

            if (std::regex_match(content, SEQUENCE_END_REGEX)) {
                handle_sequence_end(sequence_stack, message_def);
                continue;
            }

            if (std::regex_match(content, REPETITION_START)) {
                current_repetition = {};
                is_in_repetition = true;
                continue;
            }

            if (std::regex_match(content, REPETITION_END)) {
                if (is_in_repetition) {
                    handle_repetition_end(current_repetition, sequence_stack, message_def);
                }

                is_in_repetition = false;
                continue;
            }

            handle_common_field(sequence_stack, current_repetition, is_in_repetition, message_def, meta_data, row, is_first_row);
            is_first_row = false;
        }

        return message_def;
    }

    auto MessageIndexParser::parse_field_indices(const std::vector<std::string> &names, RowMetaData &meta_data) -> bool {
        int32_t tag_idx = -1;
        int32_t name_idx = -1;
        int32_t link_idx = -1;
        int32_t optnl_idx = -1;

        for (auto i = std::size_t{0}; i < names.size(); ++i) {
            const auto &name = names[i];
            if (name.find("Tag") != std::string::npos) {
                tag_idx = static_cast<int32_t>(i);
                continue;
            }

            if (name.find("Detailed Field Name") != std::string::npos) {
                name_idx = static_cast<int32_t>(i);
                link_idx = name_idx;
                continue;
            }

            if (name.find("Generic Field Name") != std::string::npos) {
                if (name_idx < 0) {
                    name_idx = static_cast<int32_t>(i);
                    link_idx = name_idx;
                }
            }

            if (name.find("Field Name") != std::string::npos) {
                if (name_idx < 0) {
                    name_idx = static_cast<int32_t>(i);
                    link_idx = name_idx;
                }
            }

            if (name.find("Status") != std::string::npos) {
                optnl_idx = static_cast<int32_t>(i);
                continue;
            }
        }

        meta_data.idx_tag = tag_idx;
        meta_data.idx_name = name_idx;
        meta_data.idx_link = link_idx;
        meta_data.idx_optnl = optnl_idx;
        meta_data.max_field_index = std::max({tag_idx, name_idx, link_idx, optnl_idx});

        return tag_idx >= 0 && name_idx >= 0 && link_idx >= 0 && optnl_idx >= 0;
    }

    void MessageIndexParser::handle_sequence_start(const std::smatch &match, SequenceStack &sequence_stack) {
        const auto is_mandatory = match[3].matched;
        const auto is_repetitive = match[5].matched;
        const auto seq_tag = match[7].str();
        const auto seq_name = match[8].matched ? match[8].str() : "<unknown>";
        SequenceDef seq_def{};
        seq_def.set_tag(seq_tag);
        seq_def.set_name(seq_name);
        seq_def.set_mandatory(is_mandatory),
                seq_def.set_repeatable(is_repetitive);

        sequence_stack.add_sequence(seq_def);

        log->debug("Sequence {} ({}): Repetitive: {}, Mandatory: {}", seq_tag, seq_name, is_repetitive, is_mandatory);
    }

    void MessageIndexParser::handle_sequence_end(SequenceStack &sequence_stack, SwiftMtMessageDefinition &msg_def) {
        const auto full_seq = sequence_stack.pop_sequence();
        if (!sequence_stack.is_empty()) {
            auto obj = sequence_stack.top().add_objects();
            obj->set_tag(full_seq.tag());
            obj->mutable_sequence()->MergeFrom(full_seq);
        } else {
            ObjectDef obj_def{};
            obj_def.set_tag(full_seq.tag());
            obj_def.mutable_sequence()->MergeFrom(full_seq);
            msg_def.add_objects()->MergeFrom(obj_def);
        }
    }

    void MessageIndexParser::handle_repetition_end(const RepetitionDef &repetition, SequenceStack &sequence_stack, SwiftMtMessageDefinition &msg_def) {
        if (sequence_stack.is_empty()) {
            if (repetition.objects_size() > 1) {
                msg_def.add_objects()->mutable_repetition()->MergeFrom(repetition);
            } else {
                msg_def.add_objects()->MergeFrom(repetition.objects(0));
            }
        } else {
            if (repetition.objects_size() > 1) {
                sequence_stack.add_repetition(repetition);
            } else {
                sequence_stack.add_object(repetition.objects(0));
            }
        }
    }

    auto MessageIndexParser::fields_from_row(const utils::http::HtmlNode &row, bool is_first_row, std::vector<utils::http::HtmlNode> &nodes) -> std::vector<std::string> {
        const auto fields = row.select(is_first_row ? "th" : "td");
        if (fields.empty()) {
            return {};
        }

        std::vector<std::string> field_contents;
        std::transform(fields.begin(), fields.end(), std::back_inserter(field_contents), [](const auto &node) { return node.text(); });

        nodes.insert(nodes.end(), fields.begin(), fields.end());

        return field_contents;
    }

    auto MessageIndexParser::handle_common_field(SequenceStack &sequence_stack, RepetitionDef &repetition, bool is_in_repetition, SwiftMtMessageDefinition &msg_def,
                                                 RowMetaData &meta_data, const utils::http::HtmlNode &row, bool is_first_row) -> bool {
        std::vector<utils::http::HtmlNode> nodes;
        std::vector<std::string> field_contents = fields_from_row(row, is_first_row, nodes);
        if (field_contents.empty()) {
            return true;
        }

        if (is_first_row) {
            if (!parse_field_indices(field_contents, meta_data)) {
                log->warn("Error parsing field indices");
                return false;
            }

            return true;
        }

        if (field_contents.size() <= meta_data.max_field_index) {
            log->warn("Cannot parse row {}", row.text());
            return false;
        }

        const auto tag = field_contents[meta_data.idx_tag];
        const auto name = field_contents[meta_data.idx_name];
        const auto link = nodes[meta_data.idx_link].select("a").front().attribute_value("href");

        ObjectDef *obj = nullptr;
        if (is_in_repetition) {
            obj = repetition.add_objects();
        } else {
            if (sequence_stack.is_empty()) {
                obj = msg_def.add_objects();
            } else {
                obj = sequence_stack.create_object();
            }
        }

        obj->set_tag(tag);
        obj->mutable_field()->set_tag(tag);
        obj->mutable_field()->set_name(name);
        handle_field_details(obj, _repository.download_topic(link));
        return true;
    }

    void MessageIndexParser::handle_field_details(ObjectDef *obj, const std::string &detail_link) {
        utils::http::HtmlDocument doc{detail_link};
        load_options(obj, doc);
        load_qualifiers(obj, doc);
    }

    void MessageIndexParser::load_options(ObjectDef *obj, const utils::http::HtmlDocument &document) {
        auto field_def = obj->mutable_field();

        const auto format_rows = document.select("table.formattable tr");

        for (const auto &row : format_rows) {
            std::vector<utils::http::HtmlNode> nodes;
            const auto fields = select_fields_as_string(row, "td", nodes);

            std::string option{};
            std::string format{};
            const auto first_field = fields[0];

            if (first_field.find("Option ") == 0) {
                option = fields[0];
                option = option.substr(std::string{"Option "}.length());
                format = convert_children_to_string(nodes[1]);
            } else {
                option = obj->tag().substr(2);
                format = convert_children_to_string(nodes[0]);
            }

            auto opt = field_def->add_options();
            opt->set_option(option);
            opt->set_full_format(format);

            load_component_names(opt, convert_children_to_string(nodes.back()));
            load_component_formats(opt, format, field_def->tag());
        }
    }

    void MessageIndexParser::load_qualifiers(ObjectDef *obj, const utils::http::HtmlDocument &document) {
        using utils::string::contains_ignore_case;

        const auto headers = select_fields_as_string(document, "table.qualifiertab tr th");
        if (headers.empty()) {
            return;
        }

        const auto itr_desc = std::find_if(headers.begin(), headers.end(), [](const auto &header) { return contains_ignore_case(header, "description"); });
        const auto itr_qlfr = std::find_if(headers.begin(), headers.end(), [](const auto &header) { return contains_ignore_case(header, "qualifier") && !contains_ignore_case(header, "description"); });
        const auto itr_optns = std::find_if(headers.begin(), headers.end(), [](const auto &header) { return contains_ignore_case(header, "options"); });

        if (itr_qlfr == headers.end()) {
            log->warn("No column named 'Qualifier' found in field qualifier table");
            return;
        }

        if (itr_optns == headers.end()) {
            log->warn("No column named 'Options' found in field qualifier table");
            return;
        }

        auto idx_desc = itr_desc != headers.end() ? static_cast<int32_t>(std::distance(headers.begin(), itr_desc)) : -1;
        auto idx_qlfr = std::distance(headers.begin(), itr_qlfr);
        auto idx_optns = std::distance(headers.begin(), itr_optns);

        const auto qlfr_rows = document.select("table.qualifiertab tr");
        for (const auto &row : qlfr_rows) {
            const auto values = select_fields_as_string(row, "td");
            if (static_cast<ptrdiff_t>(values.size()) <= std::max<ptrdiff_t>(idx_desc, idx_qlfr)) {
                continue;
            }

            auto *qlfr_def = obj->mutable_field()->add_qualifiers();
            qlfr_def->set_qualifier(values[idx_qlfr]);

            auto options = values[idx_optns];
            std::smatch match;
            while (std::regex_search(options, match, QUALIFIER_OPTIONS)) {
                if (!match[2].matched) {
                    continue;
                }

                *qlfr_def->add_options() = match[2].str();
                options = match.suffix();
            }

            if (idx_desc >= 0) {
                qlfr_def->set_description(values[idx_desc]);
            }
        }
    }

    auto MessageIndexParser::select_fields_as_string(const utils::http::ISelectable &element, const std::string &selector) -> std::vector<std::string> {
        const auto nodes = element.select(selector);
        std::vector<std::string> fields{};
        std::transform(nodes.begin(), nodes.end(), std::back_inserter(fields), [](const auto &node) { return node.text(); });
        return fields;
    }

    auto MessageIndexParser::convert_children_to_string(const utils::http::HtmlNode &node, bool crlf) const -> std::string {
        std::stringstream strm;
        const auto children = node.children();
        for (const auto &child : children) {
            const auto tag = utils::string::to_lower(child.tag_name());
            if (tag == "br") {
                strm << (crlf ? "\r\n" : "\n");
            } else if (!tag.empty()) {
                strm << convert_children_to_string(child, crlf);
            } else {
                strm << child.text();
            }
        }

        return strm.str();
    }

    void MessageIndexParser::load_component_names(OptionDef *optn, const std::string &components) {
        std::smatch match;
        auto remainder = components;

        while (std::regex_search(remainder, match, COMPONENT_NAME_MATCHER)) {
            auto *name = optn->add_component_names();
            name->set_name(match[2].str());
            if (match[1].matched) {
                name->set_separator_before(match[1].str());
            }

            if (match[3].matched) {
                name->set_separator_after(match[3].str());
            }

            remainder = match.suffix();
        }
    }

    auto MessageIndexParser::select_fields_as_string(const utils::http::ISelectable &element, const std::string &selector,
                                                     std::vector<utils::http::HtmlNode> &nodes) -> std::vector<std::string> {
        const auto element_nodes = element.select(selector);
        nodes.insert(nodes.end(), element_nodes.begin(), element_nodes.end());
        std::vector<std::string> fields{};
        std::transform(nodes.begin(), nodes.end(), std::back_inserter(fields), [](const auto &node) { return node.text(); });
        return fields;
    }

    void MessageIndexParser::load_component_formats(OptionDef *optn, const std::string &format, const std::string &tag) {
        SwiftMtComponentDefinition comp_def{};
        std::vector<std::string> errors{};

        if (!SwiftMtComponentFormatParser::process_format(format, errors, comp_def)) {
            log->error("Error processing component format: {}", utils::string::join(errors, ", "));
            return;
        }

        for (const auto &def : comp_def.formats()) {
            optn->add_component_formats()->MergeFrom(def);
        }

        if (tag.substr(0, 2) == "16") {
            log->info("Matched format: {}", tag);
            return;
        }

        ComponentFormatStack format_stack{comp_def};
        ComponentNameStack name_stack{optn->component_names()};

        const auto &formats = format_stack.format_list();
        const auto &names = name_stack.names();

        const auto num_non_separator_formats = std::count_if(formats.begin(), formats.end(), [](const auto &element) { return !element.is_separator(); });
        const auto num_non_separator_names = std::count_if(names.begin(), names.end(), [](const auto &names) { return !names.is_separator(); });

        if (num_non_separator_formats == num_non_separator_names) {
            log->info("Matched format (phase 1): {}", optn->option());
            return;
        }

        const auto format_entries = load_components_by_separator(formats);
        if (format_entries.size() == num_non_separator_names) {
            log->info("Matched format (phase 2): {}", optn->option());
            return;
        }

        std::vector<ComponentFormatStack::ComponentFormatEntry> top_level_formats{};
        std::copy_if(formats.begin(), formats.end(), std::back_inserter(top_level_formats), [](const auto &entry) { return entry.depth() == 0; });
        const auto num_non_separator_top_level_formats = std::count_if(top_level_formats.begin(), top_level_formats.end(), [](const auto &element) { return !element.is_separator(); });

        if (num_non_separator_top_level_formats == num_non_separator_names) {
            log->info("Matched format (phase 3): {}", optn->option());
            return;
        }

        auto next_name_separator = std::find_if(names.begin(), names.end(), [](const auto &name) { return name.is_separator(); });
        auto last_name_iterator = names.begin();
        auto last_format_iterator = formats.begin();

        uint32_t num_by_separator_formats = 0U;
        auto has_non_single_components = false;

        while(next_name_separator != names.end()) {
            auto part_size = std::distance(last_name_iterator, next_name_separator);
            if(part_size == 0) {
                if(!advance_to_next_separator(last_format_iterator, formats.end(), (*next_name_separator).separator())) {
                    break;
                }

                continue;
            } else if(part_size == 1) {
                ++num_by_separator_formats;
            } else {
                has_non_single_components = true;
            }

            last_name_iterator = ++next_name_separator;
            next_name_separator = std::find_if(next_name_separator, names.end(), [](const auto& name) { return name.is_separator(); });
        }

        if(num_by_separator_formats == num_non_separator_formats && !has_non_single_components) {
            log->info("Matched format (phase 4): {}", optn->option());
            return;
        }

        log->warn("Not matched: {} <-> {}", num_non_separator_formats, num_non_separator_names);
    }

    auto MessageIndexParser::load_components_by_separator(const std::list<ComponentFormatStack::ComponentFormatEntry> &formats) -> std::vector<ComponentSeparatorEntry> {
        auto has_separator_first = false;
        ComponentSeparatorEntry cur_entry{};

        std::vector<ComponentSeparatorEntry> entries{};

        for (const auto &element : formats) {
            if (element.is_separator()) {
                if (!has_separator_first) {
                    cur_entry.separator_before = element.separator();
                    has_separator_first = true;
                    continue;
                }

                cur_entry.separator_after = element.separator();
                entries.emplace_back(cur_entry);

                has_separator_first = false;
                cur_entry = {};
                continue;
            }

            cur_entry.formats.emplace_back(element.value_entry());
        }

        if (!formats.empty() && has_separator_first) {
            entries.emplace_back(cur_entry);
        }

        return entries;
    }
}