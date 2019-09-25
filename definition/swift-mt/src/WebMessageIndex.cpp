#include "web/WebDefinitionRepository.hpp"
#include "web/MessageIndex.hpp"
#include "definition/MessageIndexParser.hpp"

namespace message::definition::swift::mt {
    auto main() -> int {
        web::WebDefinitionRepository repo{"https://www2.swift.com/knowledgecentre/rest/v1/publications/usgf_20180720/3.0", "SR2018", "./cache"};
        web::MessageIndex index{repo};

        definition::MessageIndexParser index_parser{repo, index, "./definitions"};
        index_parser.process_definitions();
        return 0;
    }
}

auto main() -> int {
    return message::definition::swift::mt::main();
}