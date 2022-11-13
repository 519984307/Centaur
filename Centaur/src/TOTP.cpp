////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Created by Ricardo Romero on 15/04/22.
//  Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//
#include "TOTP.hpp"
#include <bit>
#include <chrono>

#if defined(__clang__) || defined(__GNUC__)
CENTAUR_WARN_PUSH()
CENTAUR_WARN_OFF("-Wold-style-cast")
CENTAUR_WARN_OFF("-Wpadded")
CENTAUR_WARN_OFF("-Wdeprecated-volatile")
CENTAUR_WARN_OFF("-Wweak-vtables")
CENTAUR_WARN_OFF("-Wdocumentation-unknown-command")
CENTAUR_WARN_OFF("-Wreserved-id-macro")
CENTAUR_WARN_OFF("-Wdocumentation-deprecated-sync")
CENTAUR_WARN_OFF("-Wundefined-func-template")
CENTAUR_WARN_OFF("-Wnonportable-system-include-path")
CENTAUR_WARN_OFF("-Wlanguage-extension-token")
#endif /*defined(__clang__) || defined(__GNUC__)*/
#include <openssl/hmac.h>
#if defined(__clang__) || defined(__GNUC__)
CENTAUR_WARN_POP()
#endif /*defined(__clang__) || defined(__GNUC__)*/

namespace
{
    constexpr unsigned int hashSize = EVP_MAX_MD_SIZE;
} // namespace

int CENTAUR_NAMESPACE::getTOTPCode(const std::string &secret)
{
    auto current = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count()) / 30;

    if constexpr (std::endian::native == std::endian::little)
    {
        // This integer needs to be in the big-endian format
        // in order for the authenticator to work
        auto toBE = [](uint64_t e) {
            return ((e & 0xFF00000000000000ull) >> 56) | ((e & 0x00FF000000000000ull) >> 40) | ((e & 0x0000FF0000000000ull) >> 24) | ((e & 0x000000FF00000000ull) >> 8) | ((e & 0x00000000FF000000ull) << 8) | ((e & 0x0000000000FF0000ull) << 24) | ((e & 0x000000000000FF00ull) << 40) | ((e & 0x00000000000000FFull) << 56);
        };
        current = toBE(current);
    }

    unsigned int locHashSize = hashSize;

    unsigned char *hash = ::HMAC(EVP_sha1(),
        secret.c_str(),
        static_cast<int>(secret.size()),
        reinterpret_cast<const unsigned char *>(&current),
        sizeof(current),
        nullptr,
        &locHashSize);

    int offset = hash[locHashSize - 1] & 0xf;

    int binary = ((hash[offset] & 0x7f) << 24) | ((hash[offset + 1] & 0xff) << 16) | ((hash[offset + 2] & 0xff) << 8) | (hash[offset + 3] & 0xff);

    return binary % 1000000;
}
