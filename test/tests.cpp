
#define CATCH_CONFIG_ENABLE_BENCHMARKING
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
