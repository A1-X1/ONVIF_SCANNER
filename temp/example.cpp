//
// Created by Daanyar on 08/09/2026.
//

#include "onvif/scanner.h"


// example showing how it runs

int main() {
    onvif::OnvifScanner scanner;
    auto devices = scanner.discover();

    for (auto& device : devices) {
        printf("Found device: %s\n", device.xaddr().c_str());

        // Enter devices credentials
        device.setCredentials("username", "password");

        auto info = device.getDeviceInformation();
        printf("Manufacturer: %s , Model: %s\n", info.manufacturer.c_str(), info.model.c_str());

        auto profiles = device.getProfiles();
        if (!profiles.empty()) {
            std::string uri = device.getStreamUri(profiles[0].token);
            printf("Stream URI: %s\n", uri.c_str());
        }
    }
    return 0;
}
