//
// Created by Daanyar on 27/08/2026.
//

#ifndef ONVIF_SCANNER_DEVICE_H
#define ONVIF_SCANNER_DEVICE_H

#include <string>

namespace onvif {

    struct DeviceInformation {
        std::string manufacturer;
        std::string model;
        std::string firmwareVersion;
        std::string serialNumber;
        std::string hardwareId;
    };

    DeviceInformation getDeviceInformation(const std::string& xaddr, const std::string& username, const std::string& password);

    std::string getCapabilities(const std::string& xaddr, const std::string& username, const std::string& password);


}

#endif //ONVIF_SCANNER_DEVICE_H