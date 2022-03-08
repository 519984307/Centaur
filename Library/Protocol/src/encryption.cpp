/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 08/03/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "cryptopp/aes.h"
#include "cryptopp/files.h"
#include "cryptopp/osrng.h"
#include "cryptopp/rsa.h"
#include "protocol.hpp"

auto CENTAUR_PROTOCOL_NAMESPACE::Encryption::generatePrivateKey(const std::string &file) -> void
{
    using namespace CryptoPP;
    AutoSeededX917RNG<CryptoPP::AES> prng;
    RSA::PrivateKey pk;
    pk.GenerateRandomWithKeySize(prng, 4096);

    ByteQueue queue;
    pk.Save(queue);

    FileSink stream(file.c_str());
    queue.CopyTo(stream);
    stream.MessageEnd();
}

auto CENTAUR_PROTOCOL_NAMESPACE::Encryption::generatePublicKey(const std::string &file, const std::string &privateKeyFile) -> void
{
}
