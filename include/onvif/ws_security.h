//
// Created by Daanyar on 03/09/2026.
//

#ifndef ONVIF_SCANNER_WS_SECURITY_H
#define ONVIF_SCANNER_WS_SECURITY_H

#include <string>

namespace onvif {

    std::string buildSecurityHeader(const std::string& username, const std::string& password);

}


#endif //ONVIF_SCANNER_WS_SECURITY_H