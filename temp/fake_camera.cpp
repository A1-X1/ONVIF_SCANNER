//
// Created by Daanyar on 01/09/2026.
//


#include <cstdio>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main() {

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket error");
        return 1;
    }

    // allow socket reuse
    int reuse = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    // bind to the WS discovery port
    sockaddr_in local_addr{};
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY; // give it any address
    // set the port to the WS discovery port
    local_addr.sin_port = htons(3702);

    if (bind(sock, (sockaddr*)&local_addr, sizeof(local_addr))) {
        perror("binding error");
        close(sock);
        return 1;
    }

    // join multicast group
    ip_mreq mreq{};
    inet_pton(AF_INET, "239.255.255.250", &mreq.imr_multiaddr);
    mreq.imr_interface.s_addr = INADDR_ANY;

    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        perror("IP_ADD_MEMBERSHIP");
        close(sock);
        return 1;
    }

    printf("Fake camera listening for WS-Discovery probes...\n");

    // Canned ProbeMatch reply (real replies also mirror the RelatesTo MessageID, skipped here for simplicity)
    std::string reply =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<e:Envelope xmlns:e=\"http://www.w3.org/2003/05/soap-envelope\" "
        "xmlns:w=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\" "
        "xmlns:d=\"http://schemas.xmlsoap.org/ws/2005/04/discovery\" "
        "xmlns:dn=\"http://www.onvif.org/ver10/network/wsdl\">"
        "<e:Header>"
        "<w:MessageID>uuid:fake-camera-reply-0001</w:MessageID>"
        "<w:Action>http://schemas.xmlsoap.org/ws/2005/04/discovery/ProbeMatches</w:Action>"
        "</e:Header>"
        "<e:Body>"
        "<d:ProbeMatches>"
        "<d:ProbeMatch>"
        "<w:XAddrs>http://127.0.0.1:8080/onvif/device_service</w:XAddrs>"
        "<d:Types>dn:NetworkVideoTransmitter</d:Types>"
        "<d:Scopes>onvif://www.onvif.org/name/FakeCamera</d:Scopes>"
        "</d:ProbeMatch>"
        "</d:ProbeMatches>"
        "</e:Body>"
        "</e:Envelope>";

    char buffer[8192];

    while (true) {
        sockaddr_in from{};
        socklen_t from_len = sizeof(from);
        ssize_t received = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&from, &from_len);

        if (received < 0) {
            perror("recvfrom");
            continue;
        }
        buffer[received] = '\0';

        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &from.sin_addr, ip_str, sizeof(ip_str));
        printf("\n--- Probe received from %s ---\n%s\n", ip_str, buffer);

        ssize_t sent = sendto(sock, reply.c_str(), reply.size(), 0, (sockaddr*)&from, from_len);

        if (sent < 0) {
            perror("sendto (reply)");
        } else {
            printf("Sent ProbeMatch reply (%zd bytes)\n", sent);
        }

    }

    close(sock);

    return 0;





}
