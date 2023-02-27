/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 08/03/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "Protocol.hpp"

#include <cassert>
#include <fmt/core.h>
#include <fstream>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>

namespace
{
    // clang-format off
    constexpr std::array<std::string_view, 256> g_toString = {
        "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0a", "0b", "0c", "0d", "0e", "0f",
        "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1a", "1b", "1c", "1d", "1e", "1f",
        "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2a", "2b", "2c", "2d", "2e", "2f",
        "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3a", "3b", "3c", "3d", "3e", "3f",
        "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4a", "4b", "4c", "4d", "4e", "4f",
        "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "5a", "5b", "5c", "5d", "5e", "5f",
        "60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6a", "6b", "6c", "6d", "6e", "6f",
        "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "7a", "7b", "7c", "7d", "7e", "7f",
        "80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "8a", "8b", "8c", "8d", "8e", "8f",
        "90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9a", "9b", "9c", "9d", "9e", "9f",
        "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "a8", "a9", "aa", "ab", "ac", "ad", "ae", "af",
        "b0", "b1", "b2", "b3", "b4", "b5", "b6", "b7", "b8", "b9", "ba", "bb", "bc", "bd", "be", "bf",
        "c0", "c1", "c2", "c3", "c4", "c5", "c6", "c7", "c8", "c9", "ca", "cb", "cc", "cd", "ce", "cf",
        "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "d8", "d9", "da", "db", "dc", "dd", "de", "df",
        "e0", "e1", "e2", "e3", "e4", "e5", "e6", "e7", "e8", "e9", "ea", "eb", "ec", "ed", "ee", "ef",
        "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "fa", "fb", "fc", "fd", "fe", "ff"
    };
    // clang-format on
} // namespace

struct CENTAUR_PROTOCOL_NAMESPACE::Encryption::Impl
{
public:
    inline Impl() = default;
    inline ~Impl()
    {
        if (evpPrivateKey != nullptr)
        {
            EVP_PKEY_free(evpPrivateKey);
            evpPrivateKey = nullptr;
        }

        if (evpPublicKey != nullptr)
        {
            EVP_PKEY_free(evpPublicKey);
            evpPublicKey = nullptr;
        }
    }

    EVP_PKEY *evpPrivateKey { nullptr };
    EVP_PKEY *evpPublicKey { nullptr };

public:
    static auto base16(const std::string &input) -> std::string
    {
        std::string ret;
        for (const auto &i : input)
            ret += g_toString[static_cast<std::size_t>(static_cast<unsigned char>(i))];
        return ret;
    }

    static auto decode16(const std::string &input) -> std::string
    {
        if (input.size() & 0b1)
            return ""; // Do not decode odd size of the input

        std::string ret;
        for (std::size_t i = 0; i < input.size(); i += 2)
        {
            const auto s = input.substr(i, 2);
            ret.push_back(static_cast<char>(std::stoul(s, nullptr, 16)));
        }
        return ret;
    }

    static auto base64(const std::string &input) -> std::string
    {
        BIO *bmem, *b64;
        BUF_MEM *bptr;

        b64  = BIO_new(BIO_f_base64());
        bmem = BIO_new(BIO_s_mem());
        b64  = BIO_push(b64, bmem);
        BIO_write(b64, reinterpret_cast<const void *>(input.c_str()), static_cast<int>(input.length()));
        BIO_flush(b64);
        BIO_get_mem_ptr(b64, &bptr);

        // clang-format off
#if defined(__clang__) || defined(__GNUC__)
        CENTAUR_WARN_PUSH()
        CENTAUR_WARN_OFF("-Wold-style-cast")
#endif /**/
        char *buff = (char *)malloc(bptr->length);
#if defined(__clang__) || defined(__GNUC__)
        CENTAUR_WARN_POP()
#endif /**/
        // clang-format on
        memcpy(buff, bptr->data, bptr->length - 1);
        buff[bptr->length - 1] = 0;

        std::string ret { buff, bptr->length };
        BIO_free_all(b64);
        free(buff);

        return ret;
    }

    static auto decode64(const std::string &input) -> std::string
    {
        BIO *b64, *bmem;
        // clang-format off
#if defined(__clang__) || defined(__GNUC__)
CENTAUR_WARN_PUSH()
CENTAUR_WARN_OFF("-Wold-style-cast")
#endif /**/
        char *buffer = (char *)malloc(input.length()+1);
#if defined(__clang__) || defined(__GNUC__)
CENTAUR_WARN_POP()
#endif /**/
        // clang-format on

        memset(buffer, 0, input.length());
        b64  = BIO_new(BIO_f_base64());
        bmem = BIO_new_mem_buf(reinterpret_cast<const void *>(input.c_str()), -1);
        bmem = BIO_push(b64, bmem);

        BIO_read(bmem, buffer, static_cast<int>(input.length()));
        BIO_free_all(bmem);
        std::string ret { buffer, input.length() };
        free(buffer);
        return ret;
    }
};

cen::protocol::Encryption::Encryption() :
    pimpl(new Impl {})
{
}
cen::protocol::Encryption::~Encryption() = default;

auto cen::protocol::Encryption::encryptPrivate(const std::string &plainText) -> std::string
{
    int encrypted_size  = EVP_PKEY_size(pimpl->evpPrivateKey);
    auto encrypted_data = new unsigned char[static_cast<unsigned long>(encrypted_size)];

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pimpl->evpPrivateKey, nullptr);
    if (ctx == nullptr)
    {
        delete[] encrypted_data;

        throw std::runtime_error("Failed to create private key encryption context");
    }

    if (EVP_PKEY_encrypt_init(ctx) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        delete[] encrypted_data;

        throw std::runtime_error("Failed to initialize private key encryption");
    }

    if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        delete[] encrypted_data;

        throw std::runtime_error("Failed to set padding mode for private key encryption");
    }

    size_t ciphertext_len;
    if (EVP_PKEY_encrypt(ctx, encrypted_data, &ciphertext_len, reinterpret_cast<const unsigned char *>(plainText.data()), plainText.length()) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        delete[] encrypted_data;

        throw std::runtime_error("Failed to encrypt data with private key");
    }

    EVP_PKEY_CTX_free(ctx);

    std::string ciphertext(reinterpret_cast<char *>(encrypted_data), ciphertext_len);

    delete[] encrypted_data;

    return ciphertext;
}

auto cen::protocol::Encryption::encryptPublic(const std::string &plainText) -> std::string
{
    int encrypted_size  = EVP_PKEY_size(pimpl->evpPublicKey);
    auto encrypted_data = new unsigned char[static_cast<unsigned long>(encrypted_size)];

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pimpl->evpPublicKey, nullptr);
    if (ctx == nullptr)
    {
        delete[] encrypted_data;
        throw std::runtime_error("Failed to create public key encryption context");
    }

    if (EVP_PKEY_encrypt_init(ctx) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        delete[] encrypted_data;
        throw std::runtime_error("Failed to initialize public key encryption");
    }

    if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        delete[] encrypted_data;
        throw std::runtime_error("Failed to set padding mode for public key encryption");
    }

    size_t ciphertext_len;
    if (EVP_PKEY_encrypt(ctx, encrypted_data, &ciphertext_len, reinterpret_cast<const unsigned char *>(plainText.data()), plainText.length()) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        delete[] encrypted_data;
        throw std::runtime_error("Failed to encrypt data with public key");
    }

    EVP_PKEY_CTX_free(ctx);

    std::string ciphertext(reinterpret_cast<char *>(encrypted_data), ciphertext_len);

    delete[] encrypted_data;

    return ciphertext;
}

auto cen::protocol::Encryption::decryptPrivate(const std::string &cipherText) -> std::string
{
    int decrypted_size  = EVP_PKEY_size(pimpl->evpPrivateKey);
    auto decrypted_data = new unsigned char[static_cast<unsigned long>(decrypted_size)];

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pimpl->evpPrivateKey, nullptr);
    if (ctx == nullptr)
    {
        delete[] decrypted_data;
        throw std::runtime_error("Failed to create private key decryption context");
    }

    if (EVP_PKEY_decrypt_init(ctx) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        delete[] decrypted_data;
        throw std::runtime_error("Failed to initialize private key decryption");
    }

    if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        delete[] decrypted_data;
        throw std::runtime_error("Failed to set padding mode for private key decryption");
    }

    size_t plaintext_len;
    if (EVP_PKEY_decrypt(ctx, decrypted_data, &plaintext_len, reinterpret_cast<const unsigned char *>(cipherText.data()), cipherText.length()) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        delete[] decrypted_data;
        throw std::runtime_error("Failed to decrypt data with private key");
    }

    EVP_PKEY_CTX_free(ctx);

    std::string plaintext(reinterpret_cast<char *>(decrypted_data), plaintext_len);

    delete[] decrypted_data;

    return plaintext;
}

auto cen::protocol::Encryption::decryptPublic(const std::string &cipherText) -> std::string
{
    int decrypted_size  = EVP_PKEY_size(pimpl->evpPublicKey);
    auto decrypted_data = new unsigned char[static_cast<unsigned long>(decrypted_size)];

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pimpl->evpPublicKey, nullptr);
    if (ctx == nullptr)
    {
        delete[] decrypted_data;
        throw std::runtime_error("Failed to create public key decryption context");
    }

    if (EVP_PKEY_decrypt_init(ctx) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        delete[] decrypted_data;
        throw std::runtime_error("Failed to initialize public key decryption");
    }

    if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        delete[] decrypted_data;
        throw std::runtime_error("Failed to set padding mode for public key decryption");
    }

    ERR_load_crypto_strings();
    ERR_print_errors_fp(stdout);

    size_t plaintext_len = 0;
    if (int i = EVP_PKEY_decrypt(ctx, decrypted_data, &plaintext_len, reinterpret_cast<const unsigned char *>(cipherText.data()), cipherText.length()); i <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        delete[] decrypted_data;
        throw std::runtime_error("Failed to decrypt data with public key");
    }

    EVP_PKEY_CTX_free(ctx);

    std::string plaintext(reinterpret_cast<char *>(decrypted_data), plaintext_len);

    delete[] decrypted_data;

    return plaintext;
}

auto cen::protocol::Encryption::generateRSAPrivatePublicKeys(const std::string &privateFile, const std::string &publicFile, const std::string &passphrase) -> void
{
    EVP_PKEY_CTX *pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    if (!pctx)
    {
        throw std::runtime_error("Failed to create context for RSA key generation");
    }

    if (EVP_PKEY_keygen_init(pctx) <= 0 || EVP_PKEY_CTX_set_rsa_keygen_bits(pctx, 2048) <= 0)
    {
        EVP_PKEY_CTX_free(pctx);
        throw std::runtime_error("Failed to initialize RSA key generation parameters");
    }

    EVP_PKEY *pkey = nullptr;
    if (EVP_PKEY_keygen(pctx, &pkey) <= 0)
    {
        EVP_PKEY_CTX_free(pctx);
        throw std::runtime_error("Failed to generate RSA key pair");
    }

    EVP_PKEY_CTX_free(pctx);

    // Write private key to file
    std::ofstream privateKeyOfs(privateFile);
    if (!privateKeyOfs.is_open())
    {
        EVP_PKEY_free(pkey);
        throw std::runtime_error("Failed to open private key file for writing");
    }
    BIO *privateKeyBio       = BIO_new(BIO_s_mem());
    const EVP_CIPHER *cipher = EVP_aes_256_cbc();
    if (PEM_write_bio_PrivateKey(privateKeyBio,
            pkey,
            cipher,
            reinterpret_cast<unsigned char *>(const_cast<char *>(passphrase.c_str())),
            static_cast<int>(passphrase.size()),
            nullptr, nullptr)
        == 0)
    {
        BIO_free(privateKeyBio);
        privateKeyOfs.close();
        EVP_PKEY_free(pkey);
        throw std::runtime_error("Failed to write private key to file");
    }
    char *privateKeyData = nullptr;
    long privateKeyLen   = BIO_get_mem_data(privateKeyBio, &privateKeyData);
    privateKeyOfs.write(privateKeyData, privateKeyLen);
    privateKeyOfs.close();
    BIO_free(privateKeyBio);

    // Write public key to file
    std::ofstream publicKeyOfs(publicFile);
    if (!publicKeyOfs.is_open())
    {
        EVP_PKEY_free(pkey);
        throw std::runtime_error("Failed to open public key file for writing");
    }
    BIO *publicKeyBio = BIO_new(BIO_s_mem());
    if (PEM_write_bio_PUBKEY(publicKeyBio, pkey) == 0)
    {
        BIO_free(publicKeyBio);
        publicKeyOfs.close();
        EVP_PKEY_free(pkey);
        throw std::runtime_error("Failed to write public key to file");
    }
    char *pubKeyData  = nullptr;
    long publicKeyLen = BIO_get_mem_data(publicKeyBio, &pubKeyData);
    publicKeyOfs.write(pubKeyData, publicKeyLen);
    publicKeyOfs.close();
    BIO_free(publicKeyBio);

    EVP_PKEY_free(pkey);
}

auto cen::protocol::Encryption::loadKeyPair(const std::string &privateFile, const std::string &publicFile, const std::string &passphrase) -> void
{
    try
    {
        pimpl->evpPrivateKey = [&]() -> EVP_PKEY * {
            std::ifstream ifs(privateFile);
            if (!ifs.is_open())
            {
                throw std::runtime_error("Failed to open private key file for reading");
            }
            std::string privateKeyStr((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            ifs.close();

            BIO *privateKeyBio = BIO_new_mem_buf(reinterpret_cast<const void *>(privateKeyStr.c_str()), -1);
            if (privateKeyBio == nullptr)
            {
                throw std::runtime_error("Failed to create BIO for private key data");
            }

            EVP_PKEY *pkey = PEM_read_bio_PrivateKey(privateKeyBio, nullptr, nullptr, const_cast<char *>(passphrase.c_str()));
            if (pkey == nullptr)
            {
                BIO_free(privateKeyBio);
                throw std::runtime_error("Failed to read private key from file");
            }

            BIO_free(privateKeyBio);

            return pkey;
        }();

        pimpl->evpPublicKey = [&]() -> EVP_PKEY * {
            std::ifstream ifs(publicFile);
            if (!ifs.is_open())
            {
                throw std::runtime_error("Failed to open public key file for reading");
            }
            std::string publicKeyStr((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            ifs.close();

            BIO *publicKeyBio = BIO_new_mem_buf(reinterpret_cast<const void *>(publicKeyStr.c_str()), -1);
            if (publicKeyBio == nullptr)
            {
                throw std::runtime_error("Failed to create BIO for public key data");
            }

            EVP_PKEY *pkey = PEM_read_bio_PUBKEY(publicKeyBio, nullptr, nullptr, nullptr);
            if (pkey == nullptr)
            {
                BIO_free(publicKeyBio);
                throw std::runtime_error("Failed to read public key from file");
            }

            BIO_free(publicKeyBio);

            return pkey;
        }();
    } catch (...)
    {
        throw;
    }
}

auto cen::protocol::Encryption::toBase64(const std::string &str) -> std::string
{
    // Create a memory buffer BIO for input.
    BIO *inputBio = BIO_new_mem_buf(str.c_str(), static_cast<int>(str.length()));

    // Create a base64 BIO for encoding.
    BIO *base64Bio = BIO_new(BIO_f_base64());
    BIO_set_flags(base64Bio, BIO_FLAGS_BASE64_NO_NL);

    // Create a memory buffer BIO for output.
    BIO *outputBio = BIO_new(BIO_s_mem());

    // Chain the BIOs together.
    BIO_push(base64Bio, outputBio);
    BIO_push(base64Bio, inputBio);

    // Encode the input to base64.
    BIO_write(base64Bio, str.c_str(), static_cast<int>(str.length()));
    BIO_flush(base64Bio);

    // Read the encoded output from the memory buffer BIO.
    char *encodedOutput = nullptr;
    long outputLength   = BIO_get_mem_data(outputBio, &encodedOutput);

    // Construct the output string from the encoded output.
    std::string base64Str(encodedOutput, static_cast<std::size_t>(outputLength));

    // Clean up the BIOs.
    BIO_free_all(inputBio);
    // BIO_free_all(base64Bio);
    //    BIO_free_all(outputBio);

    return base64Str;
}

auto cen::protocol::Encryption::toBase16(const std::string &str) -> std::string
{
    std::string ret;
    for (const auto &i : str)
        ret += g_toString[static_cast<std::size_t>(static_cast<unsigned char>(i))];
    return ret;
}

auto cen::protocol::Encryption::fromBase64(const std::string &str) -> std::string
{
    // Create a memory buffer BIO for input.
    BIO *inputBio = BIO_new_mem_buf(str.c_str(), static_cast<int>(str.length()));

    // Create a base64 BIO for decoding.
    BIO *base64Bio = BIO_new(BIO_f_base64());
    BIO_set_flags(base64Bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_set_flags(inputBio, BIO_FLAGS_BASE64_NO_NL);
    BIO_set_close(base64Bio, BIO_CLOSE);

    // Create a memory buffer BIO for output.
    BIO *outputBio = BIO_new(BIO_s_mem());

    // Chain the BIOs together.
    BIO_push(base64Bio, inputBio);
    BIO_push(base64Bio, outputBio);

    // Decode the base64 input to plaintext.
    char buffer[512];
    int readBytes = 0;
    std::string decodedStr;
    while ((readBytes = BIO_read(base64Bio, buffer, 512)) > 0)
    {
        decodedStr += std::string(buffer, static_cast<std::size_t>(readBytes));
    }

    // Clean up the BIOs.
    BIO_free_all(base64Bio);

    return decodedStr;
}

auto cen::protocol::Encryption::fromBase16(const std::string &str) -> std::string
{
    if (str.size() & 0b1)
        return ""; // Do not decode odd size of the input

    std::string ret;
    for (std::size_t i = 0; i < str.size(); i += 2)
    {
        const auto s = str.substr(i, 2);
        ret.push_back(static_cast<char>(std::stoul(s, nullptr, 16)));
    }
    return ret;
}
