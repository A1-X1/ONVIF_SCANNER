//
// Created by Daanyar on 27/08/2026.
//

#include <cstdio>
#include <cstring>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../include/onvif/discovery.h"

int main() {
    // create the UDP socket
    // first param is for ipv4 since WS discovery is there
    // second param is the type, either UDP or TCP, in our case for the probe it is UDP
    // third param is for the protocol
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    // error handling
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    // bind to any port locally for replies
    // selects correct internet family
    // OS selects any free port since set to 0
    sockaddr_in local_addr{};
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = 0;

    // check if can bind successfully and cast to sockaddr type
    if (bind(sock, (sockaddr*)&local_addr, sizeof(local_addr)) != 0) {
        // incase of error cleanup by closing the socket
        close(sock);
        perror("bind");
        return 1;
    }

    // set a receiving timeout
    timeval tv{};
    tv.tv_sec = 3;
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
        "<e:Body>"
        "<d:Probe><d:Types>dn:NetworkVideoTransmitter</d:Types></d:Probe>"
        "</e:Body>"
        "</e:Envelope>";

    sockaddr_in dest{};
    dest.sin_family = AF_INET; // ipv4 again
    // the default port used for WS Discovery on local netwoek
    // also htons ensures that the code is portable due to architecture differences (big vs little endidan)
    dest.sin_port = htons(3702);
    std::string multicast_addr = "239.255.255.250";
    inet_pton(AF_INET, multicast_addr.c_str(), &dest.sin_addr);

    // sending the message out to the destination
    ssize_t sent = sendto(sock, probe.c_str(), probe.size(), 0, (sockaddr*) &dest, sizeof(dest));

    if (sent < 0) {
        perror("sending err");
        return 1;
    }

    printf("sent (%zd bytes): Listening for replies", sent);

    char buffer[8192];
    // c style api call thus we need to allocate memory beforehand
    // subtract a byte from length for null terminator
    while (true) {
        sockaddr_in from{};
        socklen_t from_len = sizeof(from);
        ssize_t received = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&from, &from_len );

        if (received < 0) {
            printf("\nNo more responses (timeout).\n");
            break;
        }
        buffer[received] = '\0';
        // creating a variable to hold the ip address
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &from.sin_addr, ip_str, sizeof(ip_str));
        printf("\n--- Reply from %s ---\n%s\n", ip_str, buffer);

        // copies received into the buffer and creates a string called response, easier to work with than a buffer
        std::string response(buffer, received);
        auto devices = onvif::parseProbeMatch(response);
        for (auto& device : devices) {
            printf("Discovered device — XAddr: %s | Scopes: %s\n", device.xaddr.c_str(), device.scopes.c_str());
        }



    }




    close(sock);
    return 0;

}