#include "http/HtmlDocument.hpp"

namespace message::utils::http {

    HtmlDocument::HtmlDocument(const std::string &html) {
        _document.parse(html);
    }

    std::list<HtmlNode> HtmlDocument::select(const std::string &selector) const {
        auto selection = _document.find(selector);
        std::list<HtmlNode> ret_list{};

        for(auto i = std::size_t{0}; i < selection.nodeNum(); ++i) {
            ret_list.emplace_back(selection.nodeAt(i));
        }

        return ret_list;
    }

    std::string HtmlNode::text() const {
        return _node.text();
    }

    std::string HtmlNode::tag_name() const {
        return _node.tag();
    }

    std::string HtmlNode::attribute_value(const std::string &name) const {
        return _node.attribute(name);
    }

    std::list<HtmlNode> HtmlNode::select(const std::string &selector) const {
        auto selection = _node.find(selector);

        std::list<HtmlNode> ret_list{};

        for(auto i = std::size_t{0}; i < selection.nodeNum(); ++i) {
            ret_list.emplace_back(selection.nodeAt(i));
        }

        return ret_list;
    }
}