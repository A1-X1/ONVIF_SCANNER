//
// Created by Daanyar on 02/09/2026.
//

#include "onvif/discovery.h"
#include <pugixml.hpp>


std::vector<onvif::DiscoveredDevice> onvif::parseProbeMatch(const std::string &xml) {
    std::vector<DiscoveredDevice> devices;

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(xml.c_str());

    if (!result) {
        return devices;
    }

    auto body = doc.child("e:Envelope").child("e:Body");
    auto probeMatches = body.child("d:ProbeMatches");

    for (auto match : probeMatches.children("d:ProbeMatch")) {
        DiscoveredDevice device;
        device.xaddr = match.child("w:XAddrs").text().as_string();
        device.scopes = match.child("d:Scopes").text().as_string();

        devices.push_back(device);
    }

    return devices;
}
