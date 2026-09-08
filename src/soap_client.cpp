//
// Created by Daanyar on 02/09/2026.
//

#include "onvif/soap_client.h"
#include <curl/curl.h>

namespace onvif {

    // libcurl uses callbacks, sends chunks of bytes
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp ) {
        size_t totalSize = size * nmemb;
        std::string* response = static_cast<std::string*>(userp);
        response->append(static_cast<char*>(contents), totalSize);
        return totalSize;
    }

    std::string sendSoapRequest(const std::string &url, const std::string &soapBody) {
        CURL* curl = curl_easy_init();

        if (!curl) {
            return "";
        }

        std::string response;

        // set target url
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Post request with soapBody as request body
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, soapBody.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, soapBody.size());

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/soap+xml; charset=utf-8");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // tell curl to call the function each time data arrives and pass the data to the response string
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // reasonable timeout
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

        // perform thr req
        CURLcode res = curl_easy_perform(curl);

        // --- diagnostics ---
        printf("curl_easy_perform result: %s\n", curl_easy_strerror(res));

        long httpCode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        printf("HTTP status code: %ld\n", httpCode);

        printf("response.size() right after perform: %zu\n", response.size());
        // --- end diagnostics ---

        if (res != CURLE_OK) {
            response.clear(); // treat failure as empty response
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        return response;

    }

}
