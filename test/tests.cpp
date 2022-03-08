
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>
#include <chrono>
#include <protocol.hpp>
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

TEST_CASE("Communication JSON Protocols")
{
    using namespace CENTAUR_NAMESPACE;
    protocol::Protocol_AcceptConnection pac;

    pac.uuid()           = "{afc31b50-481a-0dce-40b5-4bd59f1118ad}";
    pac.name()           = "Connection test";
    pac.implementation() = "exchange";

    CHECK(pac.json() == R"({"data":{"uuid":"{afc31b50-481a-0dce-40b5-4bd59f1118ad}","name":"Connection test","implementation":"exchange"}})");
}

TEST_CASE("Protocol Generation private key")
{
    CENTAUR_PROTOCOL_NAMESPACE::Encryption ec;

    ec.generatePrivateKey("/Volumes/RicardoESSD/Projects/Centaur/local/Keys/privateKey.key");
}
