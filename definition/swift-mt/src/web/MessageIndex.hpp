#ifndef LIBMESSAGE_MESSAGEINDEX_HPP
#define LIBMESSAGE_MESSAGEINDEX_HPP

#include <string>
#include <list>
#include <log/Log.hpp>

#include "MessageIndexEntry.hpp"

namespace message::definition::swift::mt::web {
    class WebDefinitionRepository;

    class MessageIndex {
        LOGGER;

        WebDefinitionRepository& _web_repo;
        std::list<MessageIndexEntry> _message_types;

        void load_from_web_page(const std::string& content);
        void process_message_definitions();

    public:
        explicit MessageIndex(WebDefinitionRepository& repo);

        [[nodiscard]] const std::list<MessageIndexEntry>& messages() const {
            return _message_types;
        }
    };
}


#endif //LIBMESSAGE_MESSAGEINDEX_HPP
