//
// Created by Daanyar on 27/08/2026.
//

#include "onvif/scanner.h"

#include <unistd.h>
#include <vector>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "onvif/device.h"
#include "onvif/discovery.h"

namespace onvif {

    std::vector<OnvifDevice> OnvifScanner::discover(int timeoutSeconds) {
        std::vector<OnvifDevice> devices;

        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
            return devices;
        }

        sockaddr_in local_addr{};
        local_addr.sin_family = AF_INET;
        local_addr.sin_addr.s_addr = INADDR_ANY;
        local_addr.sin_port = 0;
        if (bind(sock, (sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
            close(sock);
            return devices;
        }

        timeval tv{};
        tv.tv_sec = timeoutSeconds;
        tv.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        std::string probe =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<e:Envelope xmlns:e=\"http://www.w3.org/2003/05/soap-envelope\" "
        "xmlns:w=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\" "
        "xmlns:d=\"http://schemas.xmlsoap.org/ws/2005/04/discovery\" "
        "xmlns:dn=\"http://www.onvif.org/ver10/network/wsdl\">"
        "<e:Header>"
        "<w:MessageID>uuid:12345678-1234-1234-1234-123456789abc</w:MessageID>"
        "<w:To e:mustUnderstand=\"1\">urn:schemas-xmlsoap-org:ws:2005:04:discovery</w:To>"
        "<w:Action e:mustUnderstand=\"1\">http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe</w:Action>"
        "</e:Header>"
        "<e:Body><d:Probe><d:Types>dn:NetworkVideoTransmitter</d:Types></d:Probe></e:Body>"
        "</e:Envelope>";

        sockaddr_in dest{};
        dest.sin_family = AF_INET;

        // should be to the onvif port
        dest.sin_port = htons(3702);

        // UDP multicast address
        inet_pton(AF_INET, "239.255.255.250", &dest.sin_addr);

        sendto(sock, probe.c_str(), probe.size(), 0, (sockaddr*)&dest, sizeof(dest));

        // response buffer
        char buffer[8192];
        while (true) {
            sockaddr_in from{};
            socklen_t from_len = sizeof(from);
            ssize_t received = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&from, &from_len);
            if (received < 0) break;

            std::string response(buffer, received);
            auto discovered = parseProbeMatch(response);
            for (auto& d : discovered) {
                devices.emplace_back(d.xaddr, d.scopes);
            }
        }

        close(sock);
        return devices;

    }
}
