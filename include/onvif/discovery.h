//
// Created by Daanyar on 02/09/2026.
//

#ifndef ONVIF_SCANNER_DISCOVERY_H
#define ONVIF_SCANNER_DISCOVERY_H


#include <string>
#include <vector>

namespace onvif {

    struct DiscoveredDevice {
        std::string xaddr;
        std::string scopes;
    };


    std::vector<DiscoveredDevice> parseProbeMatch(const std::string& xml);

}

#endif //ONVIF_SCANNER_DISCOVERY_H