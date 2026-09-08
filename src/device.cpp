//
// Created by Daanyar on 27/08/2026.
//

#include "onvif/device.h"
#include "onvif/soap_client.h"
#include "onvif/ws_security.h"
#include "onvif/discovery.h"
#include "onvif/xml_utils.h"


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

        std::string response = sendSoapRequest(xaddr, body);

        DeviceInformation deviceInfo;
        pugi::xml_document doc;
        if (!doc.load_string(response.c_str())) {
            // returns empty in case of failure
            return deviceInfo;
        }

        auto bodyNode = findChildIgnoringPrefix(doc.first_child(), "Body");
        auto infoResponse = findChildIgnoringPrefix(bodyNode, "GetDeviceInformationResponse");

        deviceInfo.manufacturer = findChildIgnoringPrefix(infoResponse, "Manufacturer").text().as_string();
        deviceInfo.model = findChildIgnoringPrefix(infoResponse, "Model").text().as_string();
        deviceInfo.firmwareVersion = findChildIgnoringPrefix(infoResponse, "FirmwareVersion").text().as_string();
        deviceInfo.serialNumber = findChildIgnoringPrefix(infoResponse, "SerialNumber").text().as_string();
        deviceInfo.hardwareId = findChildIgnoringPrefix(infoResponse, "HardwareId").text().as_string();

        return deviceInfo;

    }

    std::string getCapabilities(const std::string &xaddr, const std::string &username, const std::string &password) {
        std::string securityHeader = buildSecurityHeader(username, password);

        std::string body =
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<soap:Envelope xmlns:soap=\"http://www.w3.org/2003/05/soap-envelope\" "
            "xmlns:tds=\"http://www.onvif.org/ver10/device/wsdl\">"
            "<soap:Header>" + securityHeader + "</soap:Header>"
            "<soap:Body>"
            "<tds:GetCapabilities><tds:Category>All</tds:Category></tds:GetCapabilities>"
            "</soap:Body>"
            "</soap:Envelope>";

        return sendSoapRequest(xaddr, body);
    }
}
