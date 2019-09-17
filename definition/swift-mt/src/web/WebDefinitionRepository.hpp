#ifndef LIBMESSAGE_WEBDEFINITIONREPOSITORY_HPP
#define LIBMESSAGE_WEBDEFINITIONREPOSITORY_HPP

#include <string>
#include <http/HttpClient.hpp>
#include <log/Log.hpp>

namespace message::definition::swift::mt::web {
    class WebDefinitionRepository {
        LOGGER;

        std::string _base_url;
        std::string _service_release;
        std::string _cache_directory;

        bool load_from_cache(const std::string& file, std::string& result);
        void write_to_cache(const std::string& file, const std::string& content);

    public:
        WebDefinitionRepository(std::string base_url, std::string service_release, std::string _cache_dir);

        std::string download_topic(const std::string& file);

        [[nodiscard]] std::string service_release() const {
            return _service_release;
        }
    };
}


#endif //LIBMESSAGE_WEBDEFINITIONREPOSITORY_HPP
