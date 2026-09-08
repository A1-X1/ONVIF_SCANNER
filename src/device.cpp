//
// Created by Daanyar on 27/08/2026.
//

#include "onvif/device.h"
#include "onvif/soap_client.h"
#include "onvif/ws_security.h"
#include <pugixml.hpp>

namespace onvif {
    DeviceInformation getDeviceInformation(const std::string &xaddr, const std::string &username, const std::string &password) {
        std::string securityHeader = buildSecurityHeader(username, password);

        std::string body =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<soap:Envelope xmlns:soap=\"http://www.w3.org/2003/05/soap-envelope\" "
        "xmlns:tds=\"http://www.onvif.org/ver10/device/wsdl\">"
        "<soap:Header>" + securityHeader + "</soap:Header>"
        "<soap:Body>"
        "<tds:GetDeviceInformation/>"
        "</soap:Body>"
        "</soap:Envelope>";
    }
}
