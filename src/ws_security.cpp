//
// Created by Daanyar on 03/09/2026.
//

# include "onvif/ws_security.h";

#include <openssl/sha.h>
#include <openssl/evp.h>
#include <ctime>
#include <cstring>
#include <random>

namespace onvif {

    static std::string generateNonceBytes(size_t length) {
        std::string nonce(length, '\0');
        std::random_device rd;
        for (size_t i = 0; i < length; i++) {
            // generate
            nonce[i] = static_cast<char>(rd() % 256);
        }
        return nonce;

    }

    static std::string currentUtcTimestamp() {
        time_t now = time(nullptr);
        tm utcTime{};
        gmtime_r(&now, &utcTime);
        char buf[32];
        strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &utcTime);
        return std::string(buf);
    }

    static std::string base64Encode(const std::string& data) {
        int outlen = 4 * ((data.size() + 2) / 3);
        std::string encoded(outlen, '\0');
        int actualLen = EVP_EncodeBlock(
            reinterpret_cast<unsigned char *>(&encoded[0]),
            reinterpret_cast<const unsigned char *>(data.data()),
            static_cast<int>(data.size())
            );
        encoded.resize(actualLen);
        return encoded;
    }

    static std::string sha1Raw(const std::string& data) {
        unsigned char hash[SHA_DIGEST_LENGTH];
        SHA1(reinterpret_cast<const unsigned char *>(data.data()), data.size(), hash);
        return std::string(reinterpret_cast<char*>(hash), SHA_DIGEST_LENGTH);
    }

    std::string buildSecurityHeader(const std::string& username, const std::string& password) {
        std::string nonceBytes = generateNonceBytes(16);
        std::string created = currentUtcTimestamp();

        std::string toHash = nonceBytes + created + password;
        std::string digestRaw = sha1Raw(toHash);
        std::string digestBase64 = base64Encode(digestRaw);

        std::string nonceBase64 = base64Encode(nonceBytes);

        std::string header =
        "<wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\" "
        "xmlns:wsu=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\">"
        "<wsse:UsernameToken>"
        "<wsse:Username>" + username + "</wsse:Username>"
        "<wsse:Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">"
        + digestBase64 + "</wsse:Password>"
        "<wsse:Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">"
        + nonceBase64 + "</wsse:Nonce>"
        "<wsu:Created>" + created + "</wsu:Created>"
        "</wsse:UsernameToken>"
        "</wsse:Security>";

        return header;
    }
}