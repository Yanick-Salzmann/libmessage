#ifndef LIBMESSAGE_HTMLDOCUMENT_HPP
#define LIBMESSAGE_HTMLDOCUMENT_HPP

#include <gq/Document.h>
#include <gq/Node.h>
#include <string>
#include <memory>
#include <list>

namespace message::utils::http {
    class HtmlNode;

    class ISelectable {
    public:
        virtual ~ISelectable() = default;

        [[nodiscard]] virtual std::list<HtmlNode> select(const std::string& selector) const = 0;
    };

    class HtmlNode : public ISelectable {
        mutable CNode _node; // gumbo-query has very poor const/non-const declarations

    public:
        explicit HtmlNode(const CNode& node) : _node(node) {}

        [[nodiscard]] std::string text() const;
        [[nodiscard]] std::string tag_name() const;

        std::string attribute_value(const std::string& name) const;

        [[nodiscard]] std::list<HtmlNode> select(const std::string& selector) const override;

        [[nodiscard]] std::list<HtmlNode> children() const;

        bool is_node() const { return !tag_name().empty(); }
        bool is_text() const { return tag_name().empty(); }
    };

    class HtmlDocument : public ISelectable {
        mutable CDocument _document;

    public:
        explicit HtmlDocument(const std::string& html);

        [[nodiscard]] std::list<HtmlNode> select(const std::string& selector) const override;
    };
}


#endif //LIBMESSAGE_HTMLDOCUMENT_HPP
