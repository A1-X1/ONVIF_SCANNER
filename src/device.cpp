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

    Capabilities getCapabilities(const std::string &xaddr, const std::string &username, const std::string &password) {
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

        std::string response = sendSoapRequest(xaddr, body);
        return parseCapabilities(response);
    }

    Capabilities parseCapabilities(const std::string &xml) {
        Capabilities caps;

        pugi::xml_document doc;
        if (!doc.load_string(xml.c_str())) {
            return caps;
        }

        auto body = findChildIgnoringPrefix(doc.first_child(), "Body");
        auto response = findChildIgnoringPrefix(body, "GetCapabilitiesResponse");
        auto capabilities = findChildIgnoringPrefix(response, "Capabilities");

        auto device = findChildIgnoringPrefix(capabilities, "Device");
        caps.deviceXAddr = findChildIgnoringPrefix(device, "XAddr").text().as_string();

        auto media = findChildIgnoringPrefix(capabilities, "Media");
        caps.mediaXAddr = findChildIgnoringPrefix(media, "XAddr").text().as_string();

        auto ptz = findChildIgnoringPrefix(capabilities, "PTZ");
        caps.ptzXAddr = findChildIgnoringPrefix(ptz, "XAddr").text().as_string();

        auto events = findChildIgnoringPrefix(capabilities, "Events");
        caps.eventsXAddr = findChildIgnoringPrefix(events, "XAddr").text().as_string();

        return caps;
    }

    std::vector<MediaProfile> getProfiles(const std::string& mediaXAddr, const std::string& username, const std::string& password) {
        std::vector<MediaProfile> profiles;

        std::string securityHeader = buildSecurityHeader(username, password);
        std::string body =
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<soap:Envelope xmlns:soap=\"http://www.w3.org/2003/05/soap-envelope\" "
            "xmlns:trt=\"http://www.onvif.org/ver10/media/wsdl\">"
            "<soap:Header>" + securityHeader + "</soap:Header>"
            "<soap:Body>"
            "<trt:GetProfiles/>"
            "</soap:Body>"
            "</soap:Envelope>";

        std::string response = sendSoapRequest(mediaXAddr, body);

        pugi::xml_document doc;
        if (!doc.load_string(response.c_str())) {
            return profiles;
        }

        auto responseBody = findChildIgnoringPrefix(doc.first_child(), "Body");
        auto profilesResponse = findChildIgnoringPrefix(responseBody, "GetProfilesResponse");

        // loops each profile listed
        for (auto profileNode : findChildrenIgnoringPrefix(profilesResponse, "Profiles")) {
            MediaProfile profile;
            // token is an attribute
            profile.token = profileNode.attribute("token").as_string();
            profile.name = findChildIgnoringPrefix(profileNode, "Name").text().as_string();
            profiles.push_back(profile);
        }

        return profiles;
    }

    std::string getStreamUri(const std::string& mediaXAddr, const std::string& profileToken, const std::string& username, const std::string& password) {
        std::string securityHeader = buildSecurityHeader(username, password);

        std::string body =
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<soap:Envelope xmlns:soap=\"http://www.w3.org/2003/05/soap-envelope\" "
            "xmlns:trt=\"http://www.onvif.org/ver10/media/wsdl\" "
            "xmlns:tt=\"http://www.onvif.org/ver10/schema\">"
            "<soap:Header>" + securityHeader + "</soap:Header>"
            "<soap:Body>"
            "<trt:GetStreamUri>"
            "<trt:StreamSetup>"
            "<tt:Stream>RTP-Unicast</tt:Stream>"
            "<tt:Transport><tt:Protocol>RTSP</tt:Protocol></tt:Transport>"
            "</trt:StreamSetup>"
            "<trt:ProfileToken>" + profileToken + "</trt:ProfileToken>"
            "</trt:GetStreamUri>"
            "</soap:Body>"
            "</soap:Envelope>";

        std::string response = sendSoapRequest(mediaXAddr, body);

        pugi::xml_document doc;
        if (!doc.load_string(response.c_str())) {
            return "";
        }

        auto responseBody = findChildIgnoringPrefix(doc.first_child(), "Body");
        auto streamUriResponse = findChildIgnoringPrefix(responseBody, "GetStreamUriResponse");
        auto mediaUri = findChildIgnoringPrefix(streamUriResponse, "MediaUri");

        return findChildIgnoringPrefix(mediaUri, "Uri").text().as_string();
    }

    std::string addCredentialsToUri(const std::string& uri, const std::string& username, const std::string& password) {
        size_t schemeEnd = uri.find("://");
        if (schemeEnd == std::string::npos) {
            return uri;
        }

        std::string scheme = uri.substr(0, schemeEnd + 3);
        std::string rest = uri.substr(schemeEnd + 3);

        return scheme + username + ":" + password + "@" + rest;
    }


}
