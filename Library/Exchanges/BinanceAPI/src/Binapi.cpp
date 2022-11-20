////////////////////////////////////////////////////////////////////////////////
// Created by Ricardo Romero on 21/2/2022 for BinanceAPI.
// Copyright (c) 2022. Ricardo Romero
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Binapi.hpp"
#include "APIException.hpp"
#include "schemas.hpp"
#include <array>
#include <fmt/format.h>
#include <iostream>
#include <random>

#if defined(__clang__) || defined(__GNUC__)
BINAPI_WARN_PUSH
// clang-format off
BINAPI_WARN_OFF(old-style-cast)
BINAPI_WARN_OFF(padded)
BINAPI_WARN_OFF(deprecated-volatile)
BINAPI_WARN_OFF(weak-vtables)
BINAPI_WARN_OFF(documentation-unknown-command)
BINAPI_WARN_OFF(reserved-id-macro)
BINAPI_WARN_OFF(documentation-deprecated-sync)
BINAPI_WARN_OFF(undefined-func-template)
BINAPI_WARN_OFF(nonportable-system-include-path)
BINAPI_WARN_OFF(language-extension-token)
// clang-format on
#endif /*defined(__clang__) || defined(__GNUC__)*/

#include <openssl/hmac.h>

#if defined(__clang__) || defined(__GNUC__)
BINAPI_WARN_POP
#endif /*defined(__clang__) || defined(__GNUC__)*/

namespace BINAPI_NAMESPACE::local
{
    auto BINAPI_HIDDEN loadJsonSchema(const char *schemaData) noexcept -> rapidjson::SchemaDocument
    {
        namespace json = rapidjson;
        json::Document schemaDocument;
        // Since its internal data it will be assumed. all data is correct. in case of data corruption the problem lies in the security of the system
        schemaDocument.Parse(schemaData);
        // Load the rapidjson
        json::SchemaDocument schemaConfig(schemaDocument);
        return json::SchemaDocument(schemaDocument);
    }

    auto BINAPI_HIDDEN loadSchema(const char *data) -> std::unique_ptr<rapidjson::SchemaDocument>
    {
        return std::make_unique<rapidjson::SchemaDocument>(loadJsonSchema(data));
    };

} // namespace BINAPI_NAMESPACE::local

namespace
{
    /// \brief Used in HMAC to fastly convert unsigned char to char*
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
    constexpr unsigned int hashSize = EVP_MAX_MD_SIZE;
} // namespace

BINAPI_NAMESPACE::BinanceAPI::BinanceAPI(std::string_view baseURL, std::string_view userAgent, BinanceKeys *keys, BinanceLimits *exl) noexcept :
    m_baseURL { std::move(baseURL) },
    m_userAgent { std::move(userAgent) },
    m_binanceKeys { keys },
    m_binanceLimits { exl }
{
}

BINAPI_NAMESPACE::BinanceAPI::~BinanceAPI() noexcept = default;

auto BINAPI_NAMESPACE::BinanceAPI::HMAC256(const std::string &key, const std::string &plainText, char *out) noexcept -> void
{
    unsigned int locHashSize = hashSize;
    unsigned char *str       = ::HMAC(EVP_sha256(),
              key.c_str(),
              static_cast<int>(key.size()),
              reinterpret_cast<const unsigned char *>(plainText.c_str()),
              plainText.size(),
              nullptr,
              &locHashSize);

    // Copy the string using some C-style code
    auto k = 0ul;
    for (auto i = 0ul; i < locHashSize; i++)
    {
        for (auto j = 0ul; j < 2; ++j, ++k)
            out[k] = g_toString[str[i]][j]; // fmt::format("{:02x}", str[i]);
    }
    out[k] = 0;
}

auto BINAPI_NAMESPACE::BinanceAPI::getKeys() const noexcept -> BinanceKeys *
{
    return m_binanceKeys;
}

auto BINAPI_NAMESPACE::BinanceAPI::getLimits() const noexcept -> BinanceLimits *
{
    return m_binanceLimits;
}

auto BINAPI_NAMESPACE::BinanceAPI::getLastCallTime() const noexcept -> double
{
    return m_lastCallSeconds;
}

auto BINAPI_NAMESPACE::BinanceAPI::getTime() noexcept -> uint64_t
{
    return static_cast<std::size_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
}

auto BINAPI_NAMESPACE::BinanceAPI::setRecvWindow(const uint32_t recvWindow) noexcept -> void
{
    m_recvWindow = recvWindow;
}

auto binapi::BinanceAPI::setRecvWindowDefault() noexcept -> void
{
    m_recvWindow = 5000;
}

auto BINAPI_NAMESPACE::BinanceAPI::getRecvWindow() const noexcept -> uint32_t
{
    return m_recvWindow;
}

auto BINAPI_NAMESPACE::BinanceAPI::getCandlesTimesAndLimits(BinanceTimeIntervals interval, uint64_t startTime, uint64_t endTime, uint64_t &totalExpected) noexcept -> std::vector<std::tuple<uint64_t, uint64_t, uint64_t>>
{
    std::vector<std::tuple<uint64_t, uint64_t, uint64_t>> data;

    // 'now' and 'then' must hold the time in milliseconds
    uint64_t now  = endTime,
             then = startTime;

    // 'now' and 'then' is in minutes so: 'now' and 'then' % MOD gives the
    // milliseconds that passed in a MOD milliseconds constrain in the time: example:
    // 12:34: 'now' and 'then' % 15000 will result in 4000.
    // We then subtract that difference in milliseconds
    // This way we don't have to worry from extraneous timestamps from the interval
    uint64_t mod = fromIntervalToMilliseconds(interval);

    // Make the necessary adjustments
    now  = now - (now % mod);
    then = then - (then % mod);

    // The interval is practically the mod
    const uint64_t &msInterval = mod;
    // Since all times must be already fixed to the nearest time, a mod operation is not needed

    // Compute the total number of items expected
    totalExpected = (now - then) / msInterval;

    // Max item count for the exchange is 1000 but what if we need 1030
    // We need to store those 30 elements as well
    const uint64_t lastItems       = totalExpected % 1000;
    const uint64_t itemsFittedIn1K = totalExpected / 1000ull;

    auto timeCounter = startTime;
    for (auto i = 0ull; i < itemsFittedIn1K; ++i)
    {
        data.emplace_back(timeCounter, timeCounter + (msInterval * 1001), 1000);

        timeCounter = timeCounter + (msInterval * 1000);
    }
    data.emplace_back(timeCounter, 0, lastItems);

    return data;
}

auto BINAPI_NAMESPACE::BinanceAPI::request(const local::BinanceAPIRequest &request, const std::vector<std::pair<std::string, std::string>> &params, bool secure, double *const timeLog) -> rapidjson::Document
{
    using namespace BINAPI_NAMESPACE::local;

    cpr::Url url { fmt::format("{}{}", m_baseURL, request.endPoint) };

    cpr::Parameters parameters;
    cpr::Session session;

    for (const auto &[key, value] : params)
        parameters.Add({ key, value });

    if (secure)
    {
        // Do proper changes to the data
        secureRequest(session, parameters, *session.GetCurlHolder(), request.preventSignatureWhenSigned);
    }

    session.SetUrl(url);
    session.SetParameters(parameters);
    session.SetUserAgent({ m_userAgent.data(), m_userAgent.size() });

    bool secureRequest;
    secureRequest = m_binanceLimits->secureCall(request.callWeight);
    if (request.isOrderCall && secureRequest)
        secureRequest = m_binanceLimits->secureOrderCall(request.callWeight);

    if (!secureRequest)
    {
        throw APIException(APIException::Type::limits, "limits reached");
    }

    cpr::Response apiCall;
    switch (request.http)
    {
        case HTTPRequest::GET:
            apiCall = session.Get();
            break;
        case HTTPRequest::DELETE:
            apiCall = session.Delete();
            break;
        case HTTPRequest::POST:
            apiCall = session.Post();
            break;
        case HTTPRequest::PUT:
            apiCall = session.Put();
            break;
    }

    // Log Time
    if (timeLog != nullptr)
        *timeLog = apiCall.elapsed;

    // CURL Checking
    if (apiCall.error.code != cpr::ErrorCode::OK)
    {
        // Error Code, URL, API Call, Error Message
        throw APIException(APIException::Type::request, static_cast<int>(apiCall.error.code), apiCall.url, apiCall.error.message);
    }

    // HTTP Error checking
    if (apiCall.status_code != 200)
    {
        if (apiCall.header["content-type"] == "application/json;charset=UTF-8" || apiCall.header["content-type"] == "application/json")
        {
            int code;
            std::string msg;
            handleHTTPErrorResponse(apiCall.text, code, msg);
            throw APIException(APIException::Type::http2, apiCall.status_code, code, apiCall.url, apiCall.reason, msg);
        }
        else
        {
            throw APIException(APIException::Type::http1, static_cast<int>(apiCall.status_code), apiCall.url, apiCall.reason);
        }
    }

    rapidjson::Document jsonDoc;
    jsonDoc.Parse(apiCall.text.c_str());
    if (jsonDoc.HasParseError())
    {
        throw APIException(APIException::Type::json, apiCall.url, fmt::format("{}. At Offset: {}", rapidjson::GetParseError_En(jsonDoc.GetParseError()), jsonDoc.GetErrorOffset()));
    }

    rapidjson::SchemaValidator errorValidator(*request.errorSchema);
    if (jsonDoc.Accept(errorValidator))
    {
        int code;
        std::string msg;
        if (handleAPIRequestError(request.isSAPI, jsonDoc, code, msg))
            throw APIException(APIException::Type::api, code, apiCall.url, msg);
    }

    // Validate against the returned endpoint data
    rapidjson::SchemaValidator schemaValidator(*request.docSchema);

    if (!jsonDoc.Accept(schemaValidator))
    {
        rapidjson::StringBuffer sbSchema, sbDocument;
        schemaValidator.GetInvalidSchemaPointer().StringifyUriFragment(sbSchema);
        schemaValidator.GetInvalidDocumentPointer().StringifyUriFragment(sbDocument);

        std::string errorKeyWord;

        if (schemaValidator.GetInvalidSchemaKeyword() != nullptr)
            errorKeyWord = schemaValidator.GetInvalidSchemaKeyword();

        std::string what = fmt::format("Invalid schema: {}\nInvalid keyword: {}\nInvalid document: {}",
            sbSchema.GetString(),
            errorKeyWord,
            sbDocument.GetString());

        sbSchema.Clear();
        sbDocument.Clear();

        throw APIException(APIException::Type::schema, apiCall.url, what);
    }

    return jsonDoc;
}

auto BINAPI_NAMESPACE::BinanceAPI::secureRequest(cpr::Session &session, cpr::Parameters &parameters, const cpr::CurlHolder &cprCurlHolder, bool preventSignature) noexcept -> void
{
    cpr::Header header = cpr::Header {
        {"X-MBX-APIKEY", m_binanceKeys->apiKey}
    };
    session.SetHeader(header);

    const auto parametersContent = parameters.GetContent(cprCurlHolder);

    // For USER_STREAM data.  This does not content any parameters
    if (!parametersContent.empty() && !preventSignature)
    {
        char signature[hashSize + 1] {};
        HMAC256(m_binanceKeys->secretKey, parametersContent, signature);
        parameters.Add({ "signature", signature });
    }
}

auto BINAPI_NAMESPACE::BinanceAPI::handleAPIRequestError(bool data, const rapidjson::Value &value, int &code, std::string &message) -> bool
{
    if (data) /* Means a Binance WAPI call*/
    {
        // Error
        auto member = value.FindMember("success");
        auto msg    = value.FindMember("msg");
        if ((member != value.MemberEnd() && !value["success"].GetBool()) || (value.MemberCount() == 1 && msg != value.MemberEnd()))
        {
            code    = 0; // We don't have a code for
            message = value["msg"].GetString();
            return true;
        }
    }
    else
    {
        code    = value["code"].GetInt();
        message = value["msg"].GetString();
        return true;
    }
    return false;
}

auto BINAPI_NAMESPACE::BinanceAPI::handleHTTPErrorResponse(const std::string &jsonObject, int &code, std::string &message) -> void
{
    // This actually return some JSON Data
    rapidjson::Document jsonDoc;
    jsonDoc.Parse(jsonObject.c_str());
    code    = jsonDoc["code"].GetInt();
    message = jsonDoc["msg"].GetString();
}

auto BINAPI_NAMESPACE::BinanceAPI::apiRequest(const local::BinanceAPIRequest &requests, bool secure) -> rapidjson::Document
{
    return request(requests, {}, secure, &m_lastCallSeconds);
}

auto BINAPI_NAMESPACE::BinanceAPI::apiRequest(const local::BinanceAPIRequest &requests, const std::vector<std::pair<std::string, std::string>> &parameters, bool secure) -> rapidjson::Document
{
    return request(requests, parameters, secure, &m_lastCallSeconds);
}
