//
// Created by Daanyar on 02/09/2026.
//

#include "onvif/discovery.h"
#include "onvif/xml_utils.h"
#include <pugixml.hpp>




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
