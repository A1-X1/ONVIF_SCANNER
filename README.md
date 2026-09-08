## ONVIF SCANNER

This is a library designed for finding ONVIF devices (like cameras) on the same network

- It is designed primarily for learning of low level systems (like sockets, UDP multicast)
- It provides users with a way to easily "connect" to a device and find its relevant URI
- Code layout is modular so further iterations could simply add PTZ (pan, tilt, zoom) function of the cameras with little hassle

## What it does

1. Discovers ONVIF compliant devices on a local network using WS Discovery (incorporated from low level raw UDP multicast)
2. Authenticates requests using a WS Security digest auth
3. Requests device info, capabilities and profiles
4. Retrieves RTSP stream URI with embedded credentials for main profile

Check the temp/example.cpp for basic usage

## Dependencies

- [pugixml](https://pugixml.org/) — XML parsing
- [libcurl](https://curl.se/libcurl/) — HTTP/SOAP transport
- [OpenSSL](https://www.openssl.org/) — SHA1 + Base64 for WS-Security digest authentication


