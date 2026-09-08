//
// Created by Daanyar on 27/08/2026.
//

#ifndef ONVIF_SCANNER_DEVICE_H
#define ONVIF_SCANNER_DEVICE_H

#include <string>
#include <vector>

namespace onvif {

    struct DeviceInformation {
        std::string manufacturer;
        std::string model;
        std::string firmwareVersion;
        std::string serialNumber;
        std::string hardwareId;
    };

    struct Capabilities {
        std::string mediaXAddr;
        std::string deviceXAddr;

        // optional members (empty if dont have)
        std::string ptzXAddr;
        std::string eventsXAddr;
    };

    struct MediaProfile {
        std::string token;
        std::string name;
    };


    DeviceInformation getDeviceInformation(const std::string& xaddr, const std::string& username, const std::string& password);

    Capabilities getCapabilities(const std::string& xaddr, const std::string& username, const std::string& password);

    Capabilities parseCapabilities(const std::string& xml);

    std::vector<MediaProfile> getProfiles(const std::string& mediaXAddr, const std::string& username, const std::string& password);

    std::string getStreamUri(const std::string& mediaXAddr, const std::string& profileToken, const std::string& username, const std::string& password);

    std::string addCredentialsToUri(const std::string& uri, const std::string& username, const std::string& password);


}

#endif //ONVIF_SCANNER_DEVICE_H