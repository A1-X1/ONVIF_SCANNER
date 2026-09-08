//
// Created by Daanyar on 27/08/2026.
//

#ifndef ONVIF_SCANNER_SCANNER_H
#define ONVIF_SCANNER_SCANNER_H
#include <vector>

#include "device.h"

namespace onvif {

    class OnvifScanner {
    public:
        std::vector<OnvifDevice> discover(int timeoutSeconds = 3);
    };


}

#endif //ONVIF_SCANNER_SCANNER_H