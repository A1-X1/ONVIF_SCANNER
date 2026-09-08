//
// Created by Daanyar on 08/09/2026.
//


#include <pugixml.hpp>
#include <vector>

namespace onvif {

    pugi::xml_node findChildIgnoringPrefix(pugi::xml_node parent, const std::string& localName) {
        for (auto child : parent.children()) {
            std::string name = child.name();
            auto colonPos = name.find(':');
            std::string tag = (colonPos == std::string::npos) ? name : name.substr(colonPos + 1);
            if (tag == localName) {
                return child;
            }
        }
        return pugi::xml_node();
    }

    std::vector<pugi::xml_node> findChildrenIgnoringPrefix(pugi::xml_node parent, const std::string& localName) {
        std::vector<pugi::xml_node> results;
        for (auto child : parent.children()) {
            std::string name = child.name();
            auto colonPos = name.find(':');
            std::string tag = (colonPos == std::string::npos) ? name : name.substr(colonPos + 1);
            if (tag == localName) {
                results.push_back(child);
            }
        }
        return results;
    }


}
