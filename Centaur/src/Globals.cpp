/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 10/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#include "Globals.hpp"
#include "../ui/ui_CentaurApp.h"
#include "Logger.hpp"
#include <QDebug>
#include <QFile>
#include <QSqlError>
#include <QSqlRecord>
#include <QTextStream>
#include <concepts>

#include <cryptopp/aes.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/hex.h>
#include <cryptopp/modes.h>
#include <cryptopp/rijndael.h>

CENTAUR_NAMESPACE::Globals *CENTAUR_NAMESPACE::g_globals { nullptr };

BEGIN_CENTAUR_NAMESPACE

namespace
{
    auto isInitializationVector() -> bool
    {
        QSettings settings("CentaurProject", "Centaur");
        settings.beginGroup("__iv__");
        const auto present = settings.value("__present__").toBool();
        settings.endGroup();
        return present;
    }

    auto createInitializationVector(const QByteArray &data) -> void
    {
        QSettings settings("CentaurProject", "Centaur");
        settings.beginGroup("__iv__");
        settings.setValue("__present__", true);
        settings.setValue("__local__", data);
        settings.endGroup();
    }

    auto getInitializationVector() -> QByteArray
    {
        QSettings settings("CentaurProject", "Centaur");
        settings.beginGroup("__iv__");
        auto data = settings.value("__local__").toByteArray();
        settings.endGroup();
        return data;
    }
} // namespace

auto AESSym::createUniqueId(int rounds, std::size_t maxSize) -> std::string
{
#if defined(__clang__) || defined(__GNUC__)
    CENTAUR_WARN_PUSH()
    CENTAUR_WARN_OFF("-Wc++20-compat")
#endif
    static constinit std::array<char, 125> characters(
        { "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" });
#if defined(__clang__) || defined(__GNUC__)
    CENTAUR_WARN_POP()
#endif
    std::random_device rd;
    std::mt19937 gen(rd());

    for (int i = 0; i < rounds; ++i)
        std::shuffle(characters.begin(), characters.begin() + 124, gen);

    return { characters.data(), maxSize };
}

auto AESSym::encrypt(const QByteArray &data, const QByteArray &key) -> QString
{
    namespace crypto = CryptoPP;

    std::string encoderOutput;
    crypto::HexEncoder encoder(new crypto::StringSink(encoderOutput));

    crypto::SecByteBlock cKey(crypto::AES::DEFAULT_KEYLENGTH);
    if (key.size() > static_cast<qsizetype>(cKey.size()))
    {
        // Truncate
        cKey.Assign(reinterpret_cast<const crypto::SecByteBlock::value_type *>(key.data()), cKey.size());
    }
    else if (key.size() < static_cast<qsizetype>(cKey.size()))
    {
        // Repeat the key
        const auto repeatTimes = (static_cast<qsizetype>(cKey.size()) / key.size()) + 1;
        QByteArray newKeyData;
        for (std::remove_const<decltype(repeatTimes)>::type i = 0; i < repeatTimes; ++i)
            newKeyData.append(key.constData(), key.size());

        cKey.Assign(reinterpret_cast<const crypto::SecByteBlock::value_type *>(newKeyData.data()), cKey.size());
    }

    crypto::SecByteBlock iv(crypto::AES::BLOCKSIZE);
    QByteArray ivData;
    if (!isInitializationVector())
    {
        ivData = QByteArray::fromStdString(AESSym::createUniqueId(1000, crypto::AES::BLOCKSIZE));
        createInitializationVector(ivData);
    }
    else
        ivData = getInitializationVector();

    iv.Assign(reinterpret_cast<const crypto::SecByteBlock::value_type *>(ivData.data()), static_cast<unsigned long>(ivData.size()));

    std::string cipher;
    try
    {
        crypto::CBC_Mode<crypto::AES>::Encryption encryption;
        encryption.SetKeyWithIV(cKey, cKey.size(), iv);

        crypto::StringSource s(data.toStdString(), true,
            new crypto::StreamTransformationFilter(encryption,
                new crypto::StringSink(cipher)));
    } catch (const crypto::Exception &ex)
    {
        qDebug() << ex.what();
        return {};
    }

    encoder.Put(reinterpret_cast<const crypto::byte *>(&cipher[0]), cipher.size());
    encoder.MessageEnd();

    return QString::fromStdString(encoderOutput);
}

auto AESSym::decrypt(const QString &text, const QByteArray &key) -> QString
{
    namespace crypto = CryptoPP;

    crypto::SecByteBlock cKey(crypto::AES::DEFAULT_KEYLENGTH);
    if (key.size() > static_cast<qsizetype>(cKey.size()))
    {
        // Truncate
        cKey.Assign(reinterpret_cast<const crypto::SecByteBlock::value_type *>(key.data()), cKey.size());
    }
    else if (key.size() < static_cast<qsizetype>(cKey.size()))
    {
        // Repeat the key
        const auto repeatTimes = (static_cast<qsizetype>(cKey.size()) / key.size()) + 1;
        QByteArray newKeyData;
        for (std::remove_const<decltype(repeatTimes)>::type i = 0; i < repeatTimes; ++i)
            newKeyData.append(key.constData(), key.size());

        cKey.Assign(reinterpret_cast<const crypto::SecByteBlock::value_type *>(newKeyData.data()), cKey.size());
        // essentially if size==16 and key is 12345: cKey will contain: 1234512345123451
    }

    crypto::SecByteBlock iv(crypto::AES::BLOCKSIZE);
    QByteArray ivData;
    if (!isInitializationVector())
    {
        ivData = QByteArray::fromStdString(AESSym::createUniqueId(1000, crypto::AES::BLOCKSIZE));
        createInitializationVector(ivData);
    }
    else
        ivData = getInitializationVector();

    iv.Assign(reinterpret_cast<const crypto::SecByteBlock::value_type *>(ivData.data()), static_cast<unsigned long>(ivData.size()));

    std::string decrypted;
    try
    {
        std::string decoded;

        crypto::HexDecoder decoder(new crypto::StringSink(decoded));
        decoder.Put(reinterpret_cast<const crypto::byte *>(text.toLocal8Bit().constData()), static_cast<size_t>(text.toLocal8Bit().size()));
        decoder.MessageEnd();

        crypto::CBC_Mode<crypto::AES>::Decryption decryption;
        decryption.SetKeyWithIV(cKey, cKey.size(), iv);

        crypto::StringSource s(decoded, true,
            new crypto::StreamTransformationFilter(decryption,
                new crypto::StringSink(decrypted)) // StreamTransformationFilter
        );                                         // StringSource
    } catch (const crypto::Exception &ex)
    {
        qDebug() << ex.what();
        return {};
    }

    return QString::fromStdString(decrypted);
}

END_CENTAUR_NAMESPACE

namespace
{
    // clang-format off
    #if defined(__clang__) || defined(__GNUC__)
    CENTAUR_WARN_PUSH()
    CENTAUR_WARN_OFF("-Wexit-time-destructors")
    CENTAUR_WARN_OFF("-Wglobal-constructors")
    #endif
    // clang-format on
    const QString emptyString {};
#if defined(__clang__) || defined(__GNUC__)
    CENTAUR_WARN_POP()
#endif

} // namespace

cen::Globals::SymbolsIcons::SymbolsIcons()
{
    // Limit the cache to 1MB
    QPixmapCache::setCacheLimit(1024);
}

bool cen::Globals::SymbolsIcons::find(int size, const QString &symbol, QPixmap *px, int format)
{
    const QString formatString = [&format]() -> QString {
        switch (format)
        {
            case 0: return "PNG";
            case 1: return "SVG";
            default: return "";
        }
    }();

    const QString key = QString("%1-%2-%3").arg(size).arg(symbol, formatString);

    if (QPixmapCache::find(key, px))
        return true;
    else
    {
        QSqlQuery select;
        select.prepare("SELECT file FROM information WHERE symbol = (SELECT id FROM symbols WHERE symbol=:sym) AND size = :size AND format= :format;");
        select.bindValue(":sym", symbol);
        select.bindValue(":size", size);
        select.bindValue(":format", format);

        if (!select.exec())
        {
            logError("app", QString(QCoreApplication::tr("Failed to acquire the image. %1").arg(select.lastError().text())));
            return false;
        }
        else
        {
            while (select.next()) // We only expect 1
            {
                const QSqlRecord currentRecord = select.record();
                QString interPath;
                if (format == 1)
                    interPath = "svg";
                else
                    interPath = QString("%1").arg(size);

                auto file = QString("%1/Local/crypto/%2/%3").arg(g_globals->paths.resPath, interPath, currentRecord.value(0).toString());

                px->load(file);
                return QPixmapCache::insert(key, *px);
            }
            return false;
        }
    }
}
