//
// Created by Daanyar on 02/09/2026.
//

#include "onvif/discovery.h"
#include <pugixml.hpp>

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

std::vector<onvif::DiscoveredDevice> onvif::parseProbeMatch(const std::string &xml) {
    std::vector<DiscoveredDevice> devices;

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(xml.c_str());

    if (!result) {
        return devices;
    }

    auto body = findChildIgnoringPrefix(doc.first_child(), "Body");
    auto probeMatches = findChildIgnoringPrefix(body, "ProbeMatches");

    for (auto match : probeMatches.children()) {
        DiscoveredDevice device;
        device.xaddr = findChildIgnoringPrefix(match, "XAddrs").text().as_string();
        device.scopes = findChildIgnoringPrefix(match, "Scopes").text().as_string();

        devices.push_back(device);
    }

    return devices;
}
