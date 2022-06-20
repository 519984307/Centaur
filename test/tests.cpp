
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <Protocol.hpp>
#include <catch2/catch.hpp>
#include <chrono>
#include <iostream>
#include <uuid.hpp>

TEST_CASE("UUID Construction")
{
    CHECK_THROWS_WITH(cen::uuid(""), Catch::Contains("empty"));
    CHECK_THROWS_WITH(cen::uuid("12121212-1212-1212-1212-141414141414}"), Catch::Contains("missing '{"));
    CHECK_THROWS_WITH(cen::uuid("{12121212-1212-1212-1212-141414141414"), Catch::Contains("missing '}"));
    CHECK_THROWS_WITH(cen::uuid("{12121212-1212-1212-122-14141414114}"), Catch::Contains("invalid size"));
    CHECK_THROWS_WITH(cen::uuid("{12121212-1212-1212-1223-14141414114ad}"), Catch::Contains("invalid size"));
    CHECK_THROWS_WITH(cen::uuid("{6f121212-1212-1212-1224-014m41414114}"), Catch::Contains("invalid character"));
    CHECK_THROWS_WITH(cen::uuid("{6f121212-1212 1212-1224-014b41414114}"), Catch::Contains("invalid character"));
    CHECK_THROWS_WITH(cen::uuid("{6f12121-0-1212-1212-1224-014C-141114}"), Catch::Contains("invalid format"));
    CHECK_NOTHROW(cen::uuid("{6f12121d-1212-1212-1224-014C14a11c14}"));
    CHECK_NOTHROW(cen::uuid("{abcdefAB-CDEF-0123-4567-890000000000}"));
    CHECK_NOTHROW(cen::uuid::generate<std::mt19937_64>());

    cen::uuid uuid("{abcdefAB-CDEF-0123-4567-890000000000}");
    CHECK(uuid.to_string() == "{abcdefab-cdef-0123-4567-890000000000}");
    CHECK(uuid.to_string(true) == "{ABCDEFAB-CDEF-0123-4567-890000000000}");
}

TEST_CASE("UUID Benchmark")
{
    BENCHMARK_ADVANCED("UUID Constructor")
    (Catch::Benchmark::Chronometer meter)
    {
        std::vector<Catch::Benchmark::storage_for<cen::uuid>> uuids(static_cast<std::size_t>(meter.runs()));
        meter.measure([&](std::size_t i) { uuids[i].construct("{6f12121d-1212-1212-1224-014C14a11c14}"); });
    };

    BENCHMARK("UUID Generator mt19937")
    {
        [[maybe_unused]] auto uuid = cen::uuid::generate<std::mt19937>();
    };

    BENCHMARK("UUID Generator mt19937_64")
    {
        [[maybe_unused]] auto uuid = cen::uuid::generate<std::mt19937_64>();
    };

    BENCHMARK("UUID Generator ranlux48")
    {
        [[maybe_unused]] auto uuid = cen::uuid::generate<std::ranlux48>();
    };

    BENCHMARK("UUID Generator and stringify mt19937_64")
    {
        [[maybe_unused]] auto uuid   = cen::uuid::generate<std::mt19937_64>();
        [[maybe_unused]] auto string = cen::uuid::generate<std::mt19937_64>().to_string();
    };
}

TEST_CASE("Field Protocols Types and names")
{
    using namespace CENTAUR_NAMESPACE;
    protocol::Field<int> fd("double");
    CHECK(fd.name() == "double");
    STATIC_REQUIRE(std::is_same_v<protocol::Field<int>::field_type, int>);
    fd() = 123;
    CHECK(fd() == 123);
}

TEST_CASE("Field Protocols")
{
    using namespace CENTAUR_NAMESPACE;
    protocol::ProtocolJSONGenerator pjg;
    protocol::Field<double> fd { "double" };
    protocol::Field<bool> fbf { "f_boolean" };
    protocol::Field<bool> fbt { "t_boolean" };
    protocol::Field<int64_t> fi { "signed64" };
    protocol::Field<uint32_t> fu { "unsigned" };
    protocol::Field<std::string> fs { "string" };

    fd()  = 32.455;
    fbf() = false;
    fbt() = true;
    fi()  = -123;
    fu()  = 124;
    fs()  = "this is a string";

    pjg.addField(std::addressof(fd));
    pjg.addField(std::addressof(fbf));
    pjg.addField(std::addressof(fbt));
    pjg.addField(std::addressof(fi));
    pjg.addField(std::addressof(fu));
    pjg.addField(std::addressof(fs));

    auto str = pjg.getJSON();

    CHECK(str == R"({"data":{"double":32.45500000,"f_boolean":false,"t_boolean":true,"signed64":-123,"unsigned":124,"string":"this is a string"}})");
}

TEST_CASE("Field protocols from JSON")
{
    using namespace CENTAUR_NAMESPACE;
    protocol::ProtocolJSONGenerator pjg;
    protocol::Field<double> fd { "double" };
    protocol::Field<bool> fb { "boolean" };
    protocol::Field<int64_t> fi { "signed64" };
    protocol::Field<uint32_t> fu { "unsigned" };
    protocol::Field<std::string> fs { "string" };

    pjg.addField(std::addressof(fd));
    pjg.addField(std::addressof(fb));
    pjg.addField(std::addressof(fi));
    pjg.addField(std::addressof(fu));
    pjg.addField(std::addressof(fs));

    CHECK_THROWS(pjg.fromJSON(R"({"data":{"double":1.3,"boolean":false,"signed64":-10,"unsigned":50,"string":"this is a string"})"));
    CHECK_NOTHROW(pjg.fromJSON(R"({"data":{"double":1.3,"boolean":false,"signed64":-10,"unsigned":50,"string":"this is a string"}})"));

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
#endif /*__clang__*/
    CHECK(fd() == 1.3);
#ifdef __clang__
#pragma clang diagnostic pop
#endif /*__clang__*/
    CHECK(fb() == false);
    CHECK(fi() == -10);
    CHECK(fu() == 50);
    CHECK(fs() == "this is a string");
}

TEST_CASE("Communication JSON Protocols. Accept Connection")
{
    using namespace CENTAUR_NAMESPACE;
    protocol::message::Protocol_AcceptConnection pac;

    pac.uuid()           = "{afc31b50-481a-0dce-40b5-4bd59f1118ad}";
    pac.name()           = "Connection test";
    pac.implementation() = "exchange";

    CHECK(pac.json() == R"({"data":{"uuid":"{afc31b50-481a-0dce-40b5-4bd59f1118ad}","name":"Connection test","implementation":"exchange"}})");

    protocol::message::Protocol_AcceptConnection pacRes;
    pacRes.fromJson(pac.json().c_str());

    CHECK(pacRes.uuid() == "{afc31b50-481a-0dce-40b5-4bd59f1118ad}");
    CHECK(pacRes.name() == "Connection test");
    CHECK(pacRes.implementation() == "exchange");
}

TEST_CASE("Communication JSON Protocols. Accepted Connection")
{
    using namespace CENTAUR_NAMESPACE;

    protocol::message::Protocol_AcceptedConnection padc;
    padc.uuid()   = "{afc31b50-481a-0dce-40b5-4bd59f1118ad}";
    padc.status() = static_cast<int>(protocol::message::Protocol_AcceptedConnection::Status::uuidAlreadySet);

    CHECK(padc.json() == R"({"data":{"uuid":"{afc31b50-481a-0dce-40b5-4bd59f1118ad}","status":1}})");

    protocol::message::Protocol_AcceptedConnection padcRes;
    padcRes.fromJson(padc.json().c_str());
    CHECK(padcRes.uuid() == "{afc31b50-481a-0dce-40b5-4bd59f1118ad}");
    CHECK(padcRes.status() == 1);
}

TEST_CASE("Protocol: Packed ProtocolHedare")
{
    // JUST TO CHECK THAT THERE IS NO PADDING ON THE STRUCT
    const std::size_t size = sizeof(cen::protocol::ProtocolHeader);
    CHECK(size == 104);
}

TEST_CASE("Protocol: HASH Generation")
{
    // Text taken from https://en.wikipedia.org/wiki/SHA-2
    const std::string text = "SHA-2 (Secure Hash Algorithm 2) is a set of cryptographic hash functions designed by the United States National Security Agency (NSA) and first published in 2001.[3][4] "
                             "They are built using the Merkle–Damgård construction, from a one-way compression function itself built using the Davies–Meyer structure from a specialized block cipher.";

    // Hash calculated in: https://emn178.github.io/online-tools/sha256.html
    const int8_t hash[65] = { "b15875a8710ab7ac96fb5b5623041fa46a3acd015ddf05c71b6b98f847f944b8" };

    cen::protocol::ProtocolHeader ph {};
    cen::protocol::Generator::hash(&ph, text);

    // Test basic
    CHECK(memcmp(hash, ph.hash, sizeof(ph.hash) / sizeof(ph.hash[0])) == 0);

    // Test the Generator::testHash
    cen::protocol::ProtocolHeader test {};
    memcpy(test.hash, hash, cen::protocol::g_hashSize);
    CHECK(cen::protocol::Generator::testHash(&test, text));

    const std::string textModify = "HA-2 (Secure Hash Algorithm 2) is a set of cryptographic hash functions designed by the United States National Security Agency (NSA) and first published in 2001.[3][4] "
                                   "They are built using the Merkle–Damgård construction, from a one-way compression function itself built using the Davies–Meyer structure from a specialized block cipher.";

    // Must return false
    CHECK(!cen::protocol::Generator::testHash(&test, textModify));
}

TEST_CASE("Protocol: Compression/Decompression")
{
    // Text taken from https://en.wikipedia.org/wiki/SHA-2
    const std::string text = "SHA-2 (Secure Hash Algorithm 2) is a set of cryptographic hash functions designed by the United States National Security Agency (NSA) and first published in 2001.[3][4] "
                             "They are built using the Merkle–Damgård construction, from a one-way compression function itself built using the Davies–Meyer structure from a specialized block cipher.";

    std::vector<uint8_t> compressed;

    cen::protocol::ProtocolHeader test {};

    test.size = text.size(); // Must be set for the decompression
    cen::protocol::Generator::compress(&test, text, compressed, 9);

    std::string decompressed = cen::protocol::Generator::decompress(&test, static_cast<const uint8_t *>(compressed.data()), compressed.size());

    CHECK(decompressed == text);
}

TEST_CASE("Protocol: Send/Receive NO Compression")
{

    CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_AcceptConnection pac;
    pac.name()           = "name";
    pac.uuid()           = "uuid";
    pac.implementation() = "impl";

    CENTAUR_PROTOCOL_NAMESPACE::Protocol send;
    CENTAUR_PROTOCOL_NAMESPACE::Generator::generate(&send, 10, &pac, false);

    cen::protocol::ProtocolHeader header;
    auto receive = CENTAUR_PROTOCOL_NAMESPACE::Generator::getData(&header, send.get(), send.getSize(), 100000);

    CHECK(receive == pac.json());
}

TEST_CASE("Protocol: Send/Receive With Compression")
{

    CENTAUR_PROTOCOL_NAMESPACE::message::Protocol_AcceptConnection pac;
    pac.name()           = "name";
    pac.uuid()           = "uuid";
    pac.implementation() = "impl";

    CENTAUR_PROTOCOL_NAMESPACE::Protocol send;
    CENTAUR_PROTOCOL_NAMESPACE::Generator::generate(&send, 10, &pac, true);

    cen::protocol::ProtocolHeader header {};
    auto receive = CENTAUR_PROTOCOL_NAMESPACE::Generator::getData(&header, send.get(), send.getSize(), 100000);

    CHECK(receive == pac.json());
}

TEST_CASE("Protocol: load private key")
{
    CENTAUR_PROTOCOL_NAMESPACE::Encryption ec;

    CHECK_NOTHROW(ec.loadPrivateKey("/Volumes/RicardoESSD/Projects/Centaur/local/Resources/Private/{85261bc6-8f92-57ca-802b-f08b819031db}.pem"));
}

TEST_CASE("Protocol: load public key")
{
    CENTAUR_PROTOCOL_NAMESPACE::Encryption ec;

    CHECK_NOTHROW(ec.loadPublicKey("/Volumes/RicardoESSD/Projects/Centaur/local/Plugin/Private/{85261bc6-8f92-57ca-802b-f08b819031db}.pem"));
}

TEST_CASE("Protocol: Private Encrypt/Public decrypt")
{
    CENTAUR_PROTOCOL_NAMESPACE::Encryption ec;

    CHECK_NOTHROW(ec.loadPrivateKey("/Volumes/RicardoESSD/Projects/Centaur/local/Resources/Private/{c9c99ff8-c074-5697-b5ad-80bb6fe7337a}.pem"));
    CHECK_NOTHROW(ec.loadPublicKey("/Volumes/RicardoESSD/Projects/Centaur/local/Plugin/Private/{c9c99ff8-c074-5697-b5ad-80bb6fe7337a}.pem"));

    std::string text = "text";

    std::string base64, decBase64;

    CHECK_NOTHROW((base64 = ec.encryptPrivate(text, CENTAUR_PROTOCOL_NAMESPACE::Encryption::BinaryBase::Base64)));
    CHECK_NOTHROW((decBase64 = ec.decryptPrivate(base64, CENTAUR_PROTOCOL_NAMESPACE::Encryption::BinaryBase::Base64)));

    CHECK(decBase64 != text);

    CHECK_NOTHROW((decBase64 = ec.decryptPublic(base64, CENTAUR_PROTOCOL_NAMESPACE::Encryption::BinaryBase::Base64)));

    CHECK(decBase64 == text);
}

TEST_CASE("Protocol: Public Encrypt/Private decrypt")
{
    CENTAUR_PROTOCOL_NAMESPACE::Encryption ec;

    CHECK_NOTHROW(ec.loadPrivateKey("/Volumes/RicardoESSD/Projects/Centaur/local/Resources/Private/{f77ecf55-8162-5570-a9dc-3a79c6757c72}.pem"));
    CHECK_NOTHROW(ec.loadPublicKey("/Volumes/RicardoESSD/Projects/Centaur/local/Plugin/Private/{f77ecf55-8162-5570-a9dc-3a79c6757c72}.pem"));

    std::string text = "DSDSDDS";

    std::string base64, base16, decBase64, decBase16;

    CHECK_NOTHROW((base64 = ec.encryptPublic(text, CENTAUR_PROTOCOL_NAMESPACE::Encryption::BinaryBase::Base64)));
    CHECK_NOTHROW((decBase64 = ec.decryptPublic(base64, CENTAUR_PROTOCOL_NAMESPACE::Encryption::BinaryBase::Base64)));

    CHECK(decBase64 != text);

    CHECK_NOTHROW((decBase64 = ec.decryptPrivate(base64, CENTAUR_PROTOCOL_NAMESPACE::Encryption::BinaryBase::Base64)));

    CHECK(decBase64 == text);
}
