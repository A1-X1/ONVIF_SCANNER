//
// Created by Daanyar on 27/08/2026.
//

#include "onvif/device.h"
#include "onvif/soap_client.h"
#include "onvif/ws_security.h"
#include "onvif/discovery.h"
#include "onvif/xml_utils.h"


namespace onvif {

    // std::move is for efficiency purposes
    OnvifDevice::OnvifDevice(std::string xaddr, std::string scopes)
    : xaddr_(std::move(xaddr)), scopes_(std::move(scopes)) {}

    void OnvifDevice::setCredentials(const std::string& username, const std::string& password) {
        username_ = username;
        password_ = password;
    }

    DeviceInformation OnvifDevice::getDeviceInformation() const {
        std::string securityHeader = buildSecurityHeader(username_, password_);

        std::string body =
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<soap:Envelope xmlns:soap=\"http://www.w3.org/2003/05/soap-envelope\" "
            "xmlns:tds=\"http://www.onvif.org/ver10/device/wsdl\">"
            "<soap:Header>" + securityHeader + "</soap:Header>"
            "<soap:Body><tds:GetDeviceInformation/></soap:Body>"
            "</soap:Envelope>";

        std::string response = sendSoapRequest(xaddr_, body);

        DeviceInformation info;
        pugi::xml_document doc;
        if (!doc.load_string(response.c_str())) return info;

        auto body_node = findChildIgnoringPrefix(doc.first_child(), "Body");
        auto infoResponse = findChildIgnoringPrefix(body_node, "GetDeviceInformationResponse");

        info.manufacturer = findChildIgnoringPrefix(infoResponse, "Manufacturer").text().as_string();
        info.model = findChildIgnoringPrefix(infoResponse, "Model").text().as_string();
        info.firmwareVersion = findChildIgnoringPrefix(infoResponse, "FirmwareVersion").text().as_string();
        info.serialNumber = findChildIgnoringPrefix(infoResponse, "SerialNumber").text().as_string();
        info.hardwareId = findChildIgnoringPrefix(infoResponse, "HardwareId").text().as_string();

        return info;
    }

    // private helper func
    Capabilities OnvifDevice::parseCapabilitiesResponse(const std::string& xml) const {
        Capabilities caps;

        pugi::xml_document doc;
        if (!doc.load_string(xml.c_str())) return caps;

        auto body_node = findChildIgnoringPrefix(doc.first_child(), "Body");
        auto capResponse = findChildIgnoringPrefix(body_node, "GetCapabilitiesResponse");
        auto capabilities = findChildIgnoringPrefix(capResponse, "Capabilities");

        caps.deviceXAddr = findChildIgnoringPrefix(findChildIgnoringPrefix(capabilities, "Device"), "XAddr").text().as_string();
        caps.mediaXAddr = findChildIgnoringPrefix(findChildIgnoringPrefix(capabilities, "Media"), "XAddr").text().as_string();
        caps.ptzXAddr = findChildIgnoringPrefix(findChildIgnoringPrefix(capabilities, "PTZ"), "XAddr").text().as_string();
        caps.eventsXAddr = findChildIgnoringPrefix(findChildIgnoringPrefix(capabilities, "Events"), "XAddr").text().as_string();

        return caps;
    }


    Capabilities OnvifDevice::getCapabilities() const {
        std::string securityHeader = buildSecurityHeader(username_, password_);

        std::string body =
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<soap:Envelope xmlns:soap=\"http://www.w3.org/2003/05/soap-envelope\" "
            "xmlns:tds=\"http://www.onvif.org/ver10/device/wsdl\">"
            "<soap:Header>" + securityHeader + "</soap:Header>"
            "<soap:Body><tds:GetCapabilities><tds:Category>All</tds:Category></tds:GetCapabilities></soap:Body>"
            "</soap:Envelope>";

        std::string response = sendSoapRequest(xaddr_, body);
        return parseCapabilitiesResponse(response);
    }


    std::vector<MediaProfile> OnvifDevice::getProfiles() const {
        std::vector<MediaProfile> profiles;

        Capabilities caps = getCapabilities();
        std::string securityHeader = buildSecurityHeader(username_, password_);

        std::string body =
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<soap:Envelope xmlns:soap=\"http://www.w3.org/2003/05/soap-envelope\" "
            "xmlns:trt=\"http://www.onvif.org/ver10/media/wsdl\">"
            "<soap:Header>" + securityHeader + "</soap:Header>"
            "<soap:Body><trt:GetProfiles/></soap:Body>"
            "</soap:Envelope>";

        std::string response = sendSoapRequest(caps.mediaXAddr, body);

        pugi::xml_document doc;
        if (!doc.load_string(response.c_str())) return profiles;

        auto body_node = findChildIgnoringPrefix(doc.first_child(), "Body");
        auto profilesResponse = findChildIgnoringPrefix(body_node, "GetProfilesResponse");

        for (auto profileNode : findChildrenIgnoringPrefix(profilesResponse, "Profiles")) {
            MediaProfile profile;
            profile.token = profileNode.attribute("token").as_string();
            profile.name = findChildIgnoringPrefix(profileNode, "Name").text().as_string();
            profiles.push_back(profile);
        }

        return profiles;
    }

    // private helper func
    std::string OnvifDevice::embedCredentials(const std::string& uri) const {
        size_t schemeEnd = uri.find("://");
        if (schemeEnd == std::string::npos) return uri;

        std::string scheme = uri.substr(0, schemeEnd + 3);
        std::string rest = uri.substr(schemeEnd + 3);

        return scheme + username_ + ":" + password_ + "@" + rest;
    }

    std::string OnvifDevice::getStreamUri(const std::string& profileToken) const {
        Capabilities caps = getCapabilities();
        std::string securityHeader = buildSecurityHeader(username_, password_);

        std::string body =
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<soap:Envelope xmlns:soap=\"http://www.w3.org/2003/05/soap-envelope\" "
            "xmlns:trt=\"http://www.onvif.org/ver10/media/wsdl\" "
            "xmlns:tt=\"http://www.onvif.org/ver10/schema\">"
            "<soap:Header>" + securityHeader + "</soap:Header>"
            "<soap:Body>"
            "<trt:GetStreamUri>"
            "<trt:StreamSetup><tt:Stream>RTP-Unicast</tt:Stream><tt:Transport><tt:Protocol>RTSP</tt:Protocol></tt:Transport></trt:StreamSetup>"
            "<trt:ProfileToken>" + profileToken + "</trt:ProfileToken>"
            "</trt:GetStreamUri>"
            "</soap:Body>"
            "</soap:Envelope>";

        std::string response = sendSoapRequest(caps.mediaXAddr, body);

        pugi::xml_document doc;
        if (!doc.load_string(response.c_str())) return "";

        auto body_node = findChildIgnoringPrefix(doc.first_child(), "Body");
        auto streamResponse = findChildIgnoringPrefix(body_node, "GetStreamUriResponse");
        auto mediaUri = findChildIgnoringPrefix(streamResponse, "MediaUri");

        std::string rawUri = findChildIgnoringPrefix(mediaUri, "Uri").text().as_string();
        if (rawUri.empty()) return rawUri;

        return embedCredentials(rawUri);
    }


}
