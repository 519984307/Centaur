/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 08/03/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "Protocol.hpp"
/*#include "cryptopp/aes.h"
#include "cryptopp/base64.h"
#include "cryptopp/files.h"
#include "cryptopp/hex.h"
#include "cryptopp/osrng.h"
#include "cryptopp/rsa.h"
#include "cryptopp/sha3.h"*/
#include <cassert>
#include <fmt/core.h>
#include <fmt/os.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>

struct CENTAUR_PROTOCOL_NAMESPACE::Encryption::Impl
{
public:
    inline Impl() = default;
    inline ~Impl()
    {
        if (m_privateKey != nullptr)
        {
            RSA_free(m_privateKey);
            m_privateKey = nullptr;
        }
        if (m_publicKey != nullptr)
        {
            RSA_free(m_publicKey);
            m_publicKey = nullptr;
        }
    }

private:
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

    /*RSA *rsa= NULL;
BIO *keybio ;
keybio = BIO_new_mem_buf(key, -1);
if (keybio==NULL)
{
    printf( "Failed to create key BIO");
    return 0;
}
if(public)
{
    rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa,NULL, NULL);
}
else
{
    rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa,NULL, NULL);
}*/

public:
    inline auto loadPrivateKey(const std::string &file) -> void
    {

        FILE *fp;
#ifdef WIN32
        auto ret = fopen(&fp, file.cstr(), "r+");
        if (ret)
        {
            throw fmt::system_error(ret, "could not open the file: {}", file);
        }
#else
        fp = std::fopen(file.c_str(), "r");
        if (fp == nullptr)
        {
            throw fmt::system_error(errno, "could not open the file: {}", file);
        }
#endif  /*WIN32*/
        /*
                fseek(fp, 0, SEEK_END);
                auto size = ftell(fp);
                fseek(fp, 0, SEEK_SET);

                if (size == -1)
                {

                    fclose(fp);
                    throw fmt::system_error(errno, "could not read the file: {}", file);
                }

                char *fileData = reinterpret_cast<char *>(malloc(static_cast<std::size_t>(size + 1)));
                fread(reinterpret_cast<void *>(fileData), sizeof(char), static_cast<std::size_t>(size), fp);
                fileData[static_cast<std::size_t>(size)] = 0;

                fclose(fp);

                RSA *rsa { nullptr };
                auto keyBIO = BIO_new_mem_buf(fileData, static_cast<int>(size));*/

        m_privateKey = RSA_new();

        if (PEM_read_RSAPrivateKey(fp, &m_privateKey, nullptr, nullptr) == nullptr)
        {
            throw std::runtime_error(fmt::format("invalid key in: {}", file));
        }
    }

    inline auto loadPublicKey(const std::string &file) -> void
    {
        FILE *fp;
#ifdef WIN32
        auto ret = fopen(&fp, file.cstr(), "r+");
        if (ret)
        {
            throw fmt::system_error(ret, "could not open the file: {}", file);
        }
#else
        fp = std::fopen(file.c_str(), "rt");
        if (fp == nullptr)
        {
            throw fmt::system_error(errno, "could not open the file: {}", file);
        }

        fseek(fp, 0, SEEK_END);
        auto size    = static_cast<size_t>(ftell(fp));
        char *buffer = new char[size + 1];
        fseek(fp, 0, SEEK_SET);
        fread(buffer, size, sizeof(char), fp);
        fclose(fp);

        buffer[size] = 0;

        BIO *keyBio = BIO_new_mem_buf(reinterpret_cast<const void *>(buffer), -1);

#endif /*WIN32*/
        m_publicKey = RSA_new();

        if (PEM_read_bio_RSA_PUBKEY(keyBio, &m_publicKey, nullptr, nullptr) == nullptr)
        {
            BIO_free_all(keyBio);
            m_publicKey = nullptr;

            char buf[512];
            ERR_error_string(ERR_get_error(), buf);
            throw std::runtime_error(fmt::format("invalid key in: {}\n{}", file, buf));
        }

        BIO_free_all(keyBio);
    }

    auto encryptPrivate(const std::string &plainText, const BinaryBase base) -> std::string
    {
        std::string encrypt_text;
        // BIO *keybio = BIO_new_mem_buf(reinterpret_cast<const void *>(privateKey.c_str()), -1);
        // RSA *rsa    = RSA_new();
        //  rsa         = PEM_read_bio_RSAPrivateKey(keybio, &rsa, nullptr, nullptr);
        //  if (!rsa)
        //  {
        //      BIO_free_all(keybio);
        //     return std::string("");
        // }

        // Get the maximum length of the data block that RSA can process at a time
        int key_len   = RSA_size(m_privateKey);
        int block_len = key_len - 11; // Because the filling method is RSA_PKCS1_PADDING, so you need to subtract 11 from the key_len

        // Apply for memory: store encrypted ciphertext data
        char *sub_text = new char[static_cast<size_t>(key_len) + 1ull];
        memset(sub_text, 0, static_cast<size_t>(key_len) + 1ull);
        int ret = 0;
        int pos = 0;
        std::string sub_str;
        // Encrypt the data in segments (the return value is the length of the encrypted data)
        while (pos < static_cast<int>(plainText.size()))
        {
            sub_str = plainText.substr(static_cast<unsigned long>(pos), static_cast<unsigned long>(block_len));
            memset(sub_text, 0, static_cast<size_t>(key_len) + 1ull);
            ret = RSA_private_encrypt(static_cast<int>(sub_str.length()), reinterpret_cast<const unsigned char *>(sub_str.c_str()), reinterpret_cast<unsigned char *>(sub_text), m_privateKey, RSA_PKCS1_PADDING);
            if (ret >= 0)
            {
                encrypt_text.append(std::string(sub_text, static_cast<unsigned long>(ret)));
            }
            else
                break;
            pos += block_len;
        }

        // release memory
        delete[] sub_text;
        //  BIO_free_all(keybio);
        //  RSA_free(rsa);

        switch (base)
        {

            case BinaryBase::Base64:
                return base64(encrypt_text);
            case BinaryBase::Base16:
                return "";
        }

        return encrypt_text;
    }

    auto encryptPublic(const std::string &plainText, const BinaryBase base) -> std::string
    {
        std::string encrypt_text;
        // Get the maximum length of the data block that RSA can process at a time
        int key_len   = RSA_size(m_publicKey);
        int block_len = key_len - 11; // Because the filling method is RSA_PKCS1_PADDING, so you need to subtract 11 from the key_len

        // Apply for memory: store encrypted ciphertext data
        char *sub_text = new char[static_cast<size_t>(key_len) + 1ull];
        memset(sub_text, 0, static_cast<size_t>(key_len) + 1);
        int ret = 0;
        int pos = 0;
        std::string sub_str;
        // Encrypt the data in segments (the return value is the length of the encrypted data)
        while (pos < static_cast<int>(plainText.length()))
        {
            sub_str = plainText.substr(static_cast<unsigned long>(pos), static_cast<unsigned long>(block_len));
            memset(sub_text, 0, static_cast<size_t>(key_len) + 1ull);
            ret = RSA_public_encrypt(static_cast<int>(sub_str.length()), reinterpret_cast<const unsigned char *>(sub_str.c_str()), reinterpret_cast<unsigned char *>(sub_text), m_publicKey, RSA_PKCS1_PADDING);
            if (ret >= 0)
            {
                encrypt_text.append(std::string(sub_text, static_cast<unsigned long>(ret)));
            }
            else
                break;
            pos += block_len;
        }

        // release memory
        delete[] sub_text;

        switch (base)
        {

            case BinaryBase::Base64:
                return base64(encrypt_text);
            case BinaryBase::Base16:
                return "";
        }

        return encrypt_text;
    }

    inline auto decryptPublic(const std::string &cipherText0, const BinaryBase base) -> std::string
    {
        std::string decrypt_text;
        const std::string cipherDecoded = [&base, &cipherText0]() -> std::string {
            switch (base)
            {
                case BinaryBase::Base64:
                    return decode64(cipherText0);
                case BinaryBase::Base16:
                    return "";
            }
        }();
        // Get the maximum length of RSA single processing
        int len        = RSA_size(m_publicKey);
        char *sub_text = new char[static_cast<size_t>(len) + 1ull];
        memset(sub_text, 0, static_cast<size_t>(len) + 1ull);
        int ret = 0;
        std::string sub_str;
        int pos = 0;
        // Decrypt the ciphertext in segments
        while (pos < static_cast<int>(cipherDecoded.length()))
        {
            sub_str = cipherDecoded.substr(static_cast<unsigned long>(pos), static_cast<unsigned long>(len));
            memset(sub_text, 0, static_cast<size_t>(len) + 1ull);
            ret = RSA_public_decrypt(static_cast<int>(sub_str.length()), reinterpret_cast<const unsigned char *>(sub_str.c_str()), reinterpret_cast<unsigned char *>(sub_text), m_publicKey, RSA_PKCS1_PADDING);
            if (ret >= 0)
            {
                decrypt_text.append(std::string(sub_text, static_cast<unsigned long>(ret)));
                // printf("pos:%d, sub: %s\n", pos, sub_text);
                pos += len;
            }
            else
                break;
        }

        // release memory
        delete[] sub_text;

        return decrypt_text;
    }

    auto decryptPrivate(const std::string &cipherText0, const BinaryBase base) -> std::string
    {
        std::string decrypt_text;

        const std::string cipherDecoded = [&base, &cipherText0]() -> std::string {
            switch (base)
            {
                case BinaryBase::Base64:
                    return decode64(cipherText0);
                case BinaryBase::Base16:
                    return "";
            }
        }();

        // Get the maximum length of RSA single processing
        int key_len    = RSA_size(m_privateKey);
        char *sub_text = new char[static_cast<size_t>(key_len) + 1ull];
        memset(sub_text, 0, static_cast<size_t>(key_len) + 1ull);
        int ret = 0;
        std::string sub_str;
        int pos = 0;
        // Decrypt the ciphertext in segments
        while (pos < static_cast<int>(cipherDecoded.length()))
        {
            sub_str = cipherDecoded.substr(static_cast<unsigned long>(pos), static_cast<unsigned long>(key_len));
            memset(sub_text, 0, static_cast<size_t>(key_len) + 1ull);
            ret = RSA_private_decrypt(static_cast<int>(sub_str.length()), reinterpret_cast<const unsigned char *>(sub_str.c_str()), reinterpret_cast<unsigned char *>(sub_text), m_privateKey, RSA_PKCS1_PADDING);
            if (ret >= 0)
            {
                decrypt_text.append(std::string(sub_text, static_cast<unsigned long>(ret)));
                // printf("pos:%d, sub: %s\n", pos, sub_text);
                pos += key_len;
            }
            else
                break;
        }
        // release memory
        delete[] sub_text;

        return decrypt_text;
    }

private:
    RSA *m_privateKey { nullptr };
    RSA *m_publicKey { nullptr };
    //    CryptoPP::RSA::PublicKey m_publicKey;
    //    CryptoPP::RSA::PrivateKey m_privateKey;
};

cen::protocol::Encryption::Encryption() :
    pimpl(new Impl {})
{
}
cen::protocol::Encryption::~Encryption() = default;

auto CENTAUR_PROTOCOL_NAMESPACE::Encryption::loadPrivateKey(const std::string &file) -> void
{
    pimpl->loadPrivateKey(file);
}

auto CENTAUR_PROTOCOL_NAMESPACE::Encryption::loadPublicKey(const std::string &file) -> void
{
    pimpl->loadPublicKey(file);
}

auto cen::protocol::Encryption::encryptPrivate(const std::string &plainText, const cen::protocol::Encryption::BinaryBase base) -> std::string
{
    return pimpl->encryptPrivate(plainText, base);
}
auto cen::protocol::Encryption::encryptPublic(const std::string &plainText, const cen::protocol::Encryption::BinaryBase base) -> std::string
{
    return pimpl->encryptPublic(plainText, base);
}

auto cen::protocol::Encryption::decryptPrivate(const std::string &cipherText, const cen::protocol::Encryption::BinaryBase base) -> std::string
{
    return pimpl->decryptPrivate(cipherText, base);
}

auto cen::protocol::Encryption::decryptPublic(const std::string &cipherText, const cen::protocol::Encryption::BinaryBase base) -> std::string
{
    return pimpl->decryptPublic(cipherText, base);
}
