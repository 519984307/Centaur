/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 06/02/23.
// Copyright (c) 2023 Ricardo Romero.  All rights reserved.
//

#include <CLI/CLI.hpp>
#include <filesystem>
#include <fmt/color.h>
#include <fmt/core.h>
#include <map>
#include <openssl/sha.h>
#include <string>
#include <zip.h>

constexpr static std::string_view json_template = R"({{
  "version": "0.1.0",
  "identification": {{
    "name": "{}",
    "version": "{}",
    "uuid": "{}",
    "manufacturer": "{}",
    "checksum": "{}",
    "protected": {},
    "ui-version": {{
      "min-uuid": "{}"
    }}
  }}
}})";

constexpr static std::string_view cui_version = "a15c48b4-460b-4a79-a0a8-8ece90603f85";

static std::string checksum(const std::string &file, zip_t *z_archive) noexcept;

int main(int argc, char *argv[])
{
    CLI::App app("Centaur Plugin Packer (plpack");

    bool pl_protected = false;
    std::string pl_name, pl_version, pl_uuid, pl_man, pl_min, pl_lib, pl_out_path;

    std::map<std::string, std::string_view> ui_versions {
        {"0.1.0", cui_version}
    };

    app.add_option("-n,--name", pl_name,
           "Name of the plugin")
        ->required();
    app.add_option("-v,--version", pl_version,
           "Version of the plugin")
        ->required();
    app.add_option("-u,--uuid", pl_uuid,
           "Plugin identification string")
        ->required();
    app.add_option("-m,--man", pl_man,
           "Plugin Developer")
        ->required();
    app.add_option("-i,--min", pl_min,
           "Minimum UI version, if not specified, plpack will use the must recent version internally")
        ->transform(CLI::CheckedTransformer(ui_versions, CLI::ignore_case));
    app.add_option("-l,--lib", pl_lib,
           "The path where the actual plugin file is located. Beware that plpack will not check if it's a valid plugin")
        ->required()
        ->check(CLI::ExistingFile);
    app.add_flag("-p", pl_protected,
        "Indicate that the plugin might store sensitive data that shall be encrypted");

    app.add_option("-o,--out", pl_out_path,
           "Output path for the resulting file. The name will be the {#1}-{#2}.cpk. Where #1 is the name with spaces replaced by '-' and #2 are the first 8 characteres of the uuid")
        ->required()
        ->check(CLI::ExistingPath);

    CLI11_PARSE(app, argc, argv)

    std::string fileName = pl_name;
    auto pos             = fileName.find(' ');

    while (pos != std::string::npos)
    {
        fileName.replace(pos, 1, "-");
        pos = fileName.find(' ', pos);
    }

    if (pl_uuid.size() < 8 || pl_uuid[0] == '{')
    {
        fmt::print("{}: uuid seems to have the wrong format",
            fmt::format(fmt::fg(fmt::color::red), "error"));
        return EXIT_FAILURE;
    }

    fileName += "-" + pl_uuid.substr(0, 8);

    int zip_error    = ZIP_ER_OK;
    auto zip_archive = zip_open(fmt::format("{}/{}.cpk", pl_out_path, fileName).c_str(), ZIP_CREATE | ZIP_TRUNCATE, &zip_error);

    if (zip_error != ZIP_ER_OK)
    {
        fmt::print("{} ({}): can not create the output file",
            fmt::format(fmt::fg(fmt::color::red), "error"),
            zip_error);
        return EXIT_FAILURE;
    }

    fmt::print("Calculating checksum...\n");
    auto sha224 = checksum(pl_lib, zip_archive);

    if (sha224.empty())
        return EXIT_FAILURE;

    auto json = fmt::format(json_template,
        pl_name,
        pl_version,
        pl_uuid,
        pl_man,
        sha224,
        pl_protected,
        pl_min);

    fmt::print("{}\n", json);

    auto json_source = zip_source_buffer(zip_archive, json.c_str(), json.size(), 0);

    if (json_source == nullptr)
    {
        fmt::print("{}: JSON-metadata file was not added",
            fmt::format(fmt::fg(fmt::color::red), "error"));

        zip_close(zip_archive);
        return EXIT_FAILURE;
    }

    auto json_index = zip_file_add(zip_archive, "plugin.json", json_source, ZIP_FL_ENC_UTF_8);

    if (json_index == -1)
    {
        fmt::print("{}: JSON-metadata file was not indexed",
            fmt::format(fmt::fg(fmt::color::red), "error"));

        zip_close(zip_archive);
        return EXIT_FAILURE;
    }
    zip_set_file_compression(zip_archive, static_cast<zip_uint64_t>(json_index), ZIP_CM_DEFLATE, 7);

    fmt::print("Creating package...\n");
    auto z_close = zip_close(zip_archive);
    if (z_close == -1)
    {
        fmt::print("{}: Package coult not be created",
            fmt::format(fmt::fg(fmt::color::red), "error"));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

std::string checksum(const std::string &file, zip_t *z_archive) noexcept
{
    // clang-format off
    static constexpr std::array<std::string_view, 256> toString = {
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
    constexpr unsigned int hashSize = SHA224_DIGEST_LENGTH;

    auto size = std::filesystem::file_size(file);

    std::ifstream stream(file, std::ios::binary);
    if (stream.is_open())
    {
        auto file_data = static_cast<unsigned char *>(malloc(size + 1));
        stream.read(reinterpret_cast<char *>(file_data), static_cast<std::streamsize>(size));
        stream.close();

        auto sha224 = SHA224(file_data, size, nullptr);

        if (sha224 == nullptr)
        {
            fmt::print("{}: could not calculate SHA224",
                fmt::format(fmt::fg(fmt::color::red), "error"));
            free(file_data);
            return {};
        }

        std::string sh224_string;
        sh224_string.reserve(SHA224_DIGEST_LENGTH);

        for (auto i = 0u; i < hashSize; ++i)
        {
            sh224_string += toString[sha224[i]];
        }

        fmt::print("SHA224: {}\n",
            fmt::format(fmt::fg(fmt::color::dodger_blue), "{}", sh224_string));

        auto lib_source = zip_source_buffer(z_archive, file_data, size, 0);
        if (lib_source == nullptr)
        {
            fmt::print("{}: plugin file was not added",
                fmt::format(fmt::fg(fmt::color::red), "error"));

            free(file_data);
            zip_close(z_archive);
            return {};
        }

        auto lib_index = zip_file_add(z_archive, std::filesystem::path(file).filename().c_str(), lib_source, ZIP_FL_ENC_UTF_8);

        if (lib_index == -1)
        {
            fmt::print("{}: plugin file was not indexed",
                fmt::format(fmt::fg(fmt::color::red), "error"));

            free(file_data);
            zip_close(z_archive);
            return {};
        }
        zip_set_file_compression(z_archive, static_cast<zip_uint64_t>(lib_index), ZIP_CM_DEFLATE, 7);

        free(file_data);

        return sh224_string;
    }
    else
        fmt::print("{}: could not open the file",
            fmt::format(fmt::fg(fmt::color::red), "error"));

    return {};
}
