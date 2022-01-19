/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 06/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

// plid 1.0

// Plugin id generator
#define DONT_INCLUDE_QT
#include "Centaur.hpp"
#include "CentaurPlugin.hpp"
#define PLID_MAJOR_VERSION 1
#define PLID_MINOR_VERSION 0
#define PLID_PATCH         0

#ifdef __clang__
// clang-format off
CENTAUR_WARN_PUSH
CENTAUR_WARN_OFF(padded)
CENTAUR_WARN_OFF(undefined-func-template)
CENTAUR_WARN_OFF(signed-enum-bitfield)
CENTAUR_WARN_OFF(documentation-unknown-command)
CENTAUR_WARN_OFF(global-constructors)
CENTAUR_WARN_OFF(missing-noreturn)
CENTAUR_WARN_OFF(shadow-field-in-constructor)
CENTAUR_WARN_OFF(reserved-id-macro)
// clang-format on
#endif /*__clang__*/

#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/format.h>

#ifdef __clang__
CENTAUR_WARN_POP
#endif /*__clang__*/

#include <cxxopts.hpp>
#include <filesystem>
#include <fstream>
#include <random>
#include <string>

static int generatePublisherId(rapidjson::Document &jsonDoc, const std::string &publisher);
static int generatePluginId(rapidjson::Document &jsonDoc, const std::string &publisher, const int64_t &times, const bool &local);
static int findPublisher(rapidjson::Document &jsonDoc, const std::string &publisher);

int main(int argc, char *argv[])
{
    cxxopts::Options opts("plid", "Centaur Plugin Id generator");
    // clang-format off
    opts.add_options()
        ("h,help", "Display command line help")
        ("u,publisher", "Generate publisher id --publisher=developer name", cxxopts::value<std::string>())
        ("b,public", "Make the publisher id public --public=developer name", cxxopts::value<std::string>())
        ("d,dev", "Generate a Plugin Id with the Publisher specified by the command line option --dev=developer id", cxxopts::value<std::string>())
        ("t,times", "Generate 'times' id's", cxxopts::value<int64_t>())
        ("l,local", "Generate a plugin Id with the centaur group id. Beware that publish any id with this requires a special permission")
        ("p,publish", "Publish plugin Id", cxxopts::value<std::string>())
        ("f,find", "Find publisher", cxxopts::value<std::string>())
        ("m,merge", "Merge public PLID version with the local one. This is automatically called from Centaur when a PublisherId is not found")
        ("v,version", "plid version");
    // TODO: publish, merge and publisher
    // clang-format on

    if (argc == 1)
    {
        fmt::print("{}\n", opts.help());
        exit(EXIT_FAILURE);
    }

    try
    {
        // Open the file
        // Path where the JSON file should be
        static char file[] = { "Plugin/local/plid.json" };
        auto plidFileSize  = std::filesystem::file_size({ file });

        FILE *jsonFile { nullptr };
#if defined(WIN32) || defined(WIN64)
        // Windows specific
        auto err = fopen_s(&jsonFile, file, "rb");
        if (err)
            throw fmt::system_error(errno, "cannot open file '{}'", file);
#else
        jsonFile = fopen(file, "r");
        if (jsonFile == nullptr)
            throw fmt::system_error(errno, "cannot open file '{}'", file);
#endif

        char *plidBuffer = new char[plidFileSize + 1];
        rapidjson::FileReadStream frs(jsonFile, plidBuffer, plidFileSize);
        rapidjson::Document jsonDoc;
        if (jsonDoc.ParseStream(frs).HasParseError())
        {
            delete[] plidBuffer;
            fmt::print(fmt::fg(fmt::color::red), "error: ");
            fmt::print("json: {} at {}\n", std::string(rapidjson::GetParseError_En(jsonDoc.GetParseError())), jsonDoc.GetErrorOffset());
            exit(EXIT_FAILURE);
        }

        delete[] plidBuffer;
        fclose(jsonFile);

        auto cl          = opts.parse(argc, argv);
        auto oneInstance = [](const std::string &opt, auto times) {
            if (times > 1)
                throw std::runtime_error(fmt::format("only one instance of '{}' is allowed", opt));
        };

        if (auto publisher = cl.count("version"); publisher)
        {
            fmt::print("plid Version: {}.{}.{}\nCentaur Version: {}\nPlugin System Version: {}\n",
                PLID_MAJOR_VERSION, PLID_MINOR_VERSION, PLID_PATCH,
                cent::CentaurVersionString,
                cent::plugin::CentaurPluginVersionString);
            return EXIT_SUCCESS;
        }

        if (auto publisher = cl.count("publisher"); publisher)
        {
            oneInstance("publisher", publisher);
            return generatePublisherId(jsonDoc, cl["publisher"].as<std::string>());
        }

        int64_t genTimes = 1;
        if (auto times = cl.count("times"); times)
        {
            oneInstance("times", times);
            genTimes = cl["times"].as<int64_t>();
        }

        if (auto dev = cl.count("dev"); dev)
        {
            oneInstance("dev", dev);
            return generatePluginId(jsonDoc, cl["dev"].as<std::string>(), genTimes, false);
        }

        if (auto local = cl.count("local"); local)
        {
            oneInstance("local", local);
            return generatePluginId(jsonDoc, "", genTimes, true);
        }

        if (auto find = cl.count("find"); find)
        {
            oneInstance("find", find);
            return findPublisher(jsonDoc, cl["find"].as<std::string>());
        }

    } catch (const std::exception &ex)
    {
        fmt::print(fmt::fg(fmt::color::red), "error: ");
        fmt::print("{}\n", ex.what());
        exit(EXIT_FAILURE);
    }

    return 0;
}

/// \brief Checks if the publisher already exists
static bool isPublisher(rapidjson::Document &jsonDoc, const std::string &publisher)
try
{
    auto iter = jsonDoc.FindMember(publisher.c_str());
    return !(iter == jsonDoc.MemberEnd());

} catch (const std::exception &ex)
{
    fmt::print("{}\n", ex.what());
    exit(EXIT_FAILURE);
}

static int generatePublisherId(rapidjson::Document &jsonDoc, const std::string &publisher)
{
    if (isPublisher(jsonDoc, publisher))
    {
        fmt::print("publisher name already exists");
        return EXIT_FAILURE;
    }

    cent::plugin::PluginUUID uuid {};

    auto genRandom = []() -> std::pair<unsigned int, unsigned int> {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<unsigned int> rndInt(0x10000, 0xFFFFFFFF);
        unsigned int pb0 = 0, pb1 = 0;
        // Generate Id's at the 1000 iteration
        for (int i = 0; i < 1000; ++i)
            pb0 = rndInt(gen);
        for (int i = 0; i < 1000; ++i)
            pb1 = rndInt(gen);
        return { pb0, pb1 };
    };

    bool idStringFound = true;

    while (idStringFound)
    {
        auto [pb0, pb1]  = genRandom();
        uuid.dev0.dev0_u = pb0;
        uuid.dev1.dev1_s = static_cast<unsigned short>(pb1);
        std::string id   = fmt::format("{:08x}-{:04x}", uuid.dev0.dev0_u, uuid.dev1.dev1_s);

        bool localFound  = false;
        for (auto &iter : jsonDoc.GetObject())
        {
            auto stringItem = iter.value.FindMember("string");
            if (stringItem == iter.value.MemberEnd())
            {
                fmt::print("Missing 'string' object in publisher: {}.\n Local plid file is ill-formed\n", iter.name.GetString());
                exit(EXIT_FAILURE);
            }

            std::string string = stringItem->value.GetString();
            if (string == id)
            {
                localFound = true;
                break;
            }
        }
        if (!localFound)
        {
            rapidjson::Value bytes;
            bytes.SetObject();

            bytes.AddMember("0", uuid.dev0.dev0_c[0], jsonDoc.GetAllocator());
            bytes.AddMember("1", uuid.dev0.dev0_c[1], jsonDoc.GetAllocator());
            bytes.AddMember("2", uuid.dev0.dev0_c[2], jsonDoc.GetAllocator());
            bytes.AddMember("3", uuid.dev0.dev0_c[3], jsonDoc.GetAllocator());
            bytes.AddMember("4", uuid.dev1.dev1_c[0], jsonDoc.GetAllocator());
            bytes.AddMember("5", uuid.dev1.dev1_c[1], jsonDoc.GetAllocator());

            rapidjson::Value val;
            val.SetObject();
            val.AddMember("string", rapidjson::Value { id.c_str(), static_cast<unsigned int>(id.size()) }, jsonDoc.GetAllocator());
            val.AddMember("bytes", bytes, jsonDoc.GetAllocator());

            jsonDoc.AddMember(rapidjson::Value { publisher.c_str(), static_cast<unsigned int>(publisher.size()) }, val, jsonDoc.GetAllocator());

            rapidjson::StringBuffer buffer;
            rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
            jsonDoc.Accept(writer);

            std::ofstream fplid;
            fplid.open("Plugin/local/plid.json", std::ios_base::trunc);
            if (!fplid.is_open())
            {
                fmt::print("could not save the plid file.\n");
                exit(EXIT_FAILURE);
            }

            fplid << buffer.GetString();

            fplid.close();

            return 0;
        }
    }
    return EXIT_FAILURE;
}

static int findPublisher(rapidjson::Document &jsonDoc, const std::string &publisher)
{
    if (!isPublisher(jsonDoc, publisher))
    {
        fmt::print("publisher does not exists\n");
        return 0;
    }

    auto iter = jsonDoc[publisher.c_str()].FindMember("string");
    if (iter == jsonDoc[publisher.c_str()].MemberEnd())
    {
        fmt::print("plid file is ill formed\n");
        exit(EXIT_FAILURE);
    }

    fmt::print("Publisher id:\n{}\n", iter->value.GetString());
    return 0;
}

static int generatePluginId(rapidjson::Document &jsonDoc, const std::string &publisher, const int64_t &times, const bool &local)
{
    cent::plugin::PluginUUID uuid {};

    if (local && !publisher.empty())
    {
        fmt::print("internal error");
        exit(EXIT_FAILURE);
    }

    if (local)
    {
        uuid.dev0.dev0_u = cent::plugin::CentaurId0;
        uuid.dev1.dev1_s = cent::plugin::CentaurId1;
    }
    else
    {
        if (!isPublisher(jsonDoc, publisher))
        {
            fmt::print("publisher does not exists\n");
            return 0;
        }

        auto iter = jsonDoc[publisher.c_str()].FindMember("bytes");
        if (iter == jsonDoc[publisher.c_str()].MemberEnd())
        {
            fmt::print("plid file is ill formed - 'bytes' not found\n");
            exit(EXIT_FAILURE);
        }

        auto byte0Iter = jsonDoc[publisher.c_str()]["bytes"].FindMember("0");
        auto byte1Iter = jsonDoc[publisher.c_str()]["bytes"].FindMember("1");
        auto byte2Iter = jsonDoc[publisher.c_str()]["bytes"].FindMember("2");
        auto byte3Iter = jsonDoc[publisher.c_str()]["bytes"].FindMember("3");
        auto byte4Iter = jsonDoc[publisher.c_str()]["bytes"].FindMember("4");
        auto byte5Iter = jsonDoc[publisher.c_str()]["bytes"].FindMember("5");

        if (byte0Iter == jsonDoc[publisher.c_str()]["bytes"].MemberEnd())
        {
            fmt::print("plid file is ill formed - '0' not found\n");
            exit(EXIT_FAILURE);
        }

        if (byte1Iter == jsonDoc[publisher.c_str()]["bytes"].MemberEnd())
        {
            fmt::print("plid file is ill formed - '1' not found\n");
            exit(EXIT_FAILURE);
        }

        if (byte2Iter == jsonDoc[publisher.c_str()]["bytes"].MemberEnd())
        {
            fmt::print("plid file is ill formed - '2' not found\n");
            exit(EXIT_FAILURE);
        }

        if (byte3Iter == jsonDoc[publisher.c_str()]["bytes"].MemberEnd())
        {
            fmt::print("plid file is ill formed - '3' not found\n");
            exit(EXIT_FAILURE);
        }

        if (byte4Iter == jsonDoc[publisher.c_str()]["bytes"].MemberEnd())
        {
            fmt::print("plid file is ill formed - '4' not found\n");
            exit(EXIT_FAILURE);
        }

        if (byte5Iter == jsonDoc[publisher.c_str()]["bytes"].MemberEnd())
        {
            fmt::print("plid file is ill formed - '5' not found\n");
            exit(EXIT_FAILURE);
        }

        uuid.dev0.dev0_c[0] = static_cast<unsigned char>(jsonDoc[publisher.c_str()]["bytes"]["0"].GetUint());
        uuid.dev0.dev0_c[1] = static_cast<unsigned char>(jsonDoc[publisher.c_str()]["bytes"]["1"].GetUint());
        uuid.dev0.dev0_c[2] = static_cast<unsigned char>(jsonDoc[publisher.c_str()]["bytes"]["2"].GetUint());
        uuid.dev0.dev0_c[3] = static_cast<unsigned char>(jsonDoc[publisher.c_str()]["bytes"]["3"].GetUint());
        uuid.dev1.dev1_c[0] = static_cast<unsigned char>(jsonDoc[publisher.c_str()]["bytes"]["4"].GetUint());
        uuid.dev1.dev1_c[1] = static_cast<unsigned char>(jsonDoc[publisher.c_str()]["bytes"]["5"].GetUint());
    }

    fmt::print("PluginId will be generated for the next versions:\n");
    fmt::print("Centaur Version: {}\nPlugin System Version: {}\n\n",
        cent::CentaurVersionString,
        cent::plugin::CentaurPluginVersionString);

    auto genRandom = []() -> std::pair<unsigned int, unsigned int> {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<unsigned int> rndInt(0x10000, 0xFFFFFFFF);
        unsigned int pb0 = 0, pb1 = 0;
        // Generate Id's at the 1000 iteration
        for (int i = 0; i < 1000; ++i)
            pb0 = rndInt(gen);
        for (int i = 0; i < 1000; ++i)
            pb1 = rndInt(gen);
        return { pb0, pb1 };
    };

    for (int64_t t = 0; t < times; ++t)
    {
        auto [sp0, sp1]      = genRandom();
        auto [plg0, plg1]    = genRandom();

        uuid.sp0.sp0_s       = static_cast<unsigned short>(sp0);
        uuid.sp0.sp0_c[1]    = cent::CentaurVersionStringId;
        uuid.sp1.sp1_s       = static_cast<unsigned short>(sp1);
        uuid.sp1.sp1_c[0]    = cent::plugin::CentaurPluginVersionStringId;

        uuid.plg.plg0.plg0_s = static_cast<unsigned short>(plg0);
        uuid.plg.plg1.plg1_u = plg1;

        fmt::print("{:08x}-{:04x}-{:04x}-{:04x}-{:04x}{:08x}\n",
            uuid.dev0.dev0_u,
            uuid.dev1.dev1_s,
            uuid.sp0.sp0_s,
            uuid.sp1.sp1_s,
            uuid.plg.plg0.plg0_s,
            uuid.plg.plg1.plg1_u);
    }

    return 0;
}
