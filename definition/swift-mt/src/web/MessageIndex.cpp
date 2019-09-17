#include "MessageIndex.hpp"
#include "WebDefinitionRepository.hpp"
#include <http/HtmlDocument.hpp>

#include <regex>

namespace message::definition::swift::mt::web {
    LOGGER_IMPL(MessageIndex);

    MessageIndex::MessageIndex(WebDefinitionRepository &repo) : _web_repo(repo) {
        load_from_web_page(_web_repo.download_topic("idx_msg"));
    }

    void MessageIndex::load_from_web_page(const std::string &content) {
        static std::regex message_type_pattern { "MT ([0-9n][0-9][0-9]) (.*)" };

        log->info("Parsing message index...");

        utils::http::HtmlDocument document(content);
        const auto messages = document.select("table.idxtbl tr td a[href]");

        for(const auto& msg_node : messages) {
            const auto link = msg_node.attribute_value("href");
            const auto description = msg_node.text();

            std::smatch type_match{};
            if(!std::regex_match(description, type_match, message_type_pattern)) {
                continue;
            }

            const auto message_type = type_match[1].str();
            const auto detail = type_match[2].str();

            _message_types.emplace_back(detail, message_type, link);

            log->info("@{} -> {}", link, description);
        }

        log->info("Loaded {} message types from message index", _message_types.size());
    }

    void MessageIndex::process_message_definitions() {

    }
}