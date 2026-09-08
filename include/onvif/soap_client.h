//
// Created by Daanyar on 02/09/2026.
//

#ifndef ONVIF_SCANNER_SOAP_CLIENT_H
#define ONVIF_SCANNER_SOAP_CLIENT_H
#include <string>

#include "discovery.h"


namespace onvif {
    std::string sendSoapRequest(const std::string& url, const std::string& soapBody);
    std::string getDeviceInformation(DiscoveredDevice device);
}

#endif //ONVIF_SCANNER_SOAP_CLIENT_H