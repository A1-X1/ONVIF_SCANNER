//
// Created by Daanyar on 08/09/2026.
//

#ifndef ONVIF_SCANNER_XML_UTILS_H
#define ONVIF_SCANNER_XML_UTILS_H

#include <pugixml.hpp>
#include <string>
#include <vector>

namespace onvif {

    pugi::xml_node findChildIgnoringPrefix(pugi::xml_node parent, const std::string& localName);
    std::vector<pugi::xml_node> findChildrenIgnoringPrefix(pugi::xml_node parent, const std::string& localName);

} // namespace onvif

#endif //ONVIF_SCANNER_XML_UTILS_H