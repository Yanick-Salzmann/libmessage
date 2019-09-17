#include "WebDefinitionRepository.hpp"
#include <http/HttpClient.hpp>
#include <filesystem>
#include <fstream>
#include <string/conversion.hpp>

namespace message::definition::swift::mt::web {

    LOGGER_IMPL(WebDefinitionRepository);

    std::string WebDefinitionRepository::download_topic(const std::string &file) {
        const auto has_extension = utils::string::to_lower(file).find(".htm") != std::string::npos;

        const auto actual_file = has_extension ? file.substr(0, file.find(".htm")) : file;

        log->debug("Retrieving topic {}", actual_file);
        std::string cached_content{};
        if (load_from_cache(actual_file, cached_content)) {
            return cached_content;
        }

        const auto content = utils::http::HttpClient::download_string(fmt::format("{}/{}.htm", _base_url, actual_file));
        write_to_cache(actual_file, content);

        return content;
    }

    bool WebDefinitionRepository::load_from_cache(const std::string &file, std::string &result) {
        auto root_path = std::filesystem::u8path(_cache_directory);
        root_path /= _service_release;
        root_path /= "web";
        root_path /= file + ".htm";

        std::ifstream input_file(absolute(root_path).string(), std::ios::binary);

        if (input_file.is_open()) {
            log->debug("Reading {} from cached file {}", file, absolute(root_path).string());

            input_file.seekg(0, std::ios::end);
            const auto full_size = static_cast<std::size_t>(input_file.tellg());
            input_file.seekg(0, std::ios::beg);

            std::vector<char> content(full_size);
            input_file.read(content.data(), content.size());

            result = std::string(content.begin(), content.end());
            return true;
        }

        return false;
    }

    WebDefinitionRepository::WebDefinitionRepository(std::string base_url, std::string service_release, std::string _cache_dir) :
            _base_url(std::move(base_url)),
            _service_release(std::move(service_release)),
            _cache_directory(std::move(_cache_dir)) {

    }

    void WebDefinitionRepository::write_to_cache(const std::string &file, const std::string &content) {
        auto root_path = std::filesystem::u8path(_cache_directory);
        root_path /= _service_release;
        root_path /= "web";
        root_path /= file + ".htm";

        log->debug("Caching {} in file {}", file, absolute(root_path).string());

        const auto parent = root_path.parent_path();
        if (!std::filesystem::exists(parent)) {
            std::error_code ec{};
            if(!std::filesystem::create_directories(parent, ec)) {
                log->error("Error creating parent directories: {}", ec.message());
                throw std::runtime_error(ec.message());
            }
        }

        std::ofstream os(absolute(root_path).string(), std::ios::binary);
        os.write(content.c_str(), content.size());
    }
}