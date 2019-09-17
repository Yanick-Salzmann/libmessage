#ifndef LIBMESSAGE_MESSAGEINDEXENTRY_HPP
#define LIBMESSAGE_MESSAGEINDEXENTRY_HPP

#include <string>

namespace message::definition::swift::mt::web {
    class MessageIndexEntry {
        std::string _description;
        std::string _message_type;
        std::string _detail_link;

    public:
        MessageIndexEntry() = default;

        MessageIndexEntry(std::string description, std::string message_type, std::string detail_link) :
                _description(std::move(description)),
                _message_type(std::move(message_type)),
                _detail_link(std::move(detail_link)) {

        }

        [[nodiscard]] std::string description() const {
            return _description;
        }

        [[nodiscard]] std::string message_type() const {
            return _message_type;
        }

        [[nodiscard]] std::string detail_link() const {
            return _detail_link;
        }
    };
}

#endif //LIBMESSAGE_MESSAGEINDEXENTRY_HPP
