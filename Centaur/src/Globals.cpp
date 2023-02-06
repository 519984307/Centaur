/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 10/12/21.
// Copyright (c) 2021 Ricardo Romero.  All rights reserved.
//

#include "Globals.hpp"
#include "../ui/ui_CentaurApp.h"
#include "Logger.hpp"
#include <QDebug>
#include <QDir>
#include <QErrorMessage>
#include <QFile>
#include <QFileInfo>
#include <QSqlError>
#include <QSqlRecord>
#include <QTextStream>

#include <concepts>

#include <cryptopp/aes.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/hex.h>
#include <cryptopp/modes.h>
#include <cryptopp/rijndael.h>

#include <zip.h>

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

QPixmap cen::findAssetImage(int size, const QString &asset, CENTAUR_INTERFACE_NAMESPACE::AssetImageSource source, QWidget *caller)
{
    QPixmap px;
    const auto [sourceFile, cachePath] = [&]() -> QPair<QString, QString> {
        switch (source)
        {
            case CENTAUR_INTERFACE_NAMESPACE::AssetImageSource::Stock:
                return { g_globals->paths.resPath + "/Images/stk_img.zip",
                    g_globals->paths.resPath + "/Images/Cache/Stock" };
            case CENTAUR_INTERFACE_NAMESPACE::AssetImageSource::Forex:
                return { g_globals->paths.resPath + "/Images/frx_img.zip",
                    g_globals->paths.resPath + "/Images/Cache/Forex" };
            case CENTAUR_INTERFACE_NAMESPACE::AssetImageSource::Crypto:
                return { g_globals->paths.resPath + "/Images/crp_img.zip",
                    g_globals->paths.resPath + "/Images/Cache/Crypto" };
        }
    }();

    auto createDirectory = [](const QString &directory) {
        QDir cacheDirectory(directory);
        if (!cacheDirectory.exists())
        {
            if (!cacheDirectory.mkpath(directory))
            {
                logError("FindImage", QCoreApplication::translate("trace", "Image cache directory %1 not created").arg(directory));
            }
            else
                logInfo("FindImage", QCoreApplication::translate("trace", "Image cache %1 directory created").arg(directory));
        }
    };

#if defined(__clang__) || defined(__GNUC__)
    CENTAUR_WARN_PUSH()
    CENTAUR_WARN_OFF("-Wexit-time-destructors")
#endif
    static const std::array<QString, 4>
        localImageCacheDirectories {
            g_globals->paths.resPath + "/Images/Cache/Stock",
            g_globals->paths.resPath + "/Images/Cache/Forex",
            g_globals->paths.resPath + "/Images/Cache/Crypto"
        };
#if defined(__clang__) || defined(__GNUC__)
    CENTAUR_WARN_POP()
#endif
    for (const auto &licd : localImageCacheDirectories)
        createDirectory(licd);

    // Find in the cache path
    const std::array<QString, 4>
        cacheFiles {
            QString("%1/%2.svg").arg(cachePath, asset),
            QString("%1/%2_%3.png").arg(cachePath, asset).arg(size),
            QString("%1/%2_%3.tiff").arg(cachePath, asset).arg(size),
            QString("%1/%2_%3.gif").arg(cachePath, asset).arg(size)
        };

    for (const auto &inCache : cacheFiles)
    {
        if (QFileInfo::exists(inCache))
        {
            logTrace("FindImage", QCoreApplication::translate("trace", "%1 find in file system cache").arg(asset));

            // According to the Qt documentation QPixmap::load uses QPixmapCache internally
            if (!px.load(inCache))
            {
                logTrace("FindImage", QCoreApplication::translate("error", "Failed to load %1 file from file system cache").arg(inCache));
                if (QFile::remove(inCache))
                {
                    logWarn("FindImage", QCoreApplication::translate("warning", "%1 removed from file system cache because of data corruption").arg(inCache));
                }
                else
                {
                    logTrace("FindImage", QCoreApplication::translate("error", "Attempt to to remove %1 file from file system cache  because of data corruption failed").arg(inCache));
                }
            }
            else
            {
                logTrace("FindImage", QCoreApplication::translate("trace", "File %1 loaded into the pixmap").arg(asset));
                if (inCache.endsWith("svg", Qt::CaseInsensitive))
                    px.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                return px;
            }
        }
    }

    int zip_error    = ZIP_ER_OK;
    auto zip_archive = zip_open(sourceFile.toLocal8Bit().constData(), ZIP_RDONLY, &zip_error);

    if (zip_error != ZIP_ER_OK || zip_archive == nullptr)
    {
        logError("FindImage", QCoreApplication::translate("error", "File %1 could not be found").arg(sourceFile));

        QErrorMessage errMessage(caller);
        errMessage.showMessage(
            QErrorMessage::tr("An internal file could not be located.\n%1\nReinstalling the application could fix the problem").arg(sourceFile),
            sourceFile);

        return {};
    }

    const std::array<QString, 4> zipFiles {
        QString("%2.svg").arg(asset.toLower()),
        QString("%2_%3.png").arg(asset.toLower()).arg(size),
        QString("%2_%3.tiff").arg(asset.toLower()).arg(size),
        QString("%2_%3.gif").arg(asset.toLower()).arg(size)
    };

#if defined(__clang__) || defined(__GNUC__)
    CENTAUR_WARN_PUSH()
    CENTAUR_WARN_OFF("-Wnullable-to-nonnull-conversion")
#endif
    QString actualFileName;
    zip_int64_t fileIndex = -1;
    for (const auto &compressedFile : zipFiles)
    {
        actualFileName = compressedFile;
        fileIndex      = zip_name_locate(zip_archive, compressedFile.toLocal8Bit().constData(), ZIP_FL_NOCASE | ZIP_FL_ENC_UTF_8);
        if (fileIndex != -1)
            break;
    }

    zip_stat_t st;
    if (fileIndex == -1)
    {
        logWarn("FindImage", QCoreApplication::translate("error", "Image for asset %1 could not be found").arg(asset));
        zip_close(zip_archive);
        return {};
    }

    auto stats = zip_stat_index(zip_archive, static_cast<zip_uint64_t>(fileIndex), ZIP_STAT_SIZE, &st);

    auto fileData = zip_fopen_index(zip_archive, static_cast<zip_uint64_t>(fileIndex), 0);

    if (stats != -1)
    {
        if (st.size > 0)
        {
            QScopedArrayPointer<char> _file_bytes(new char[st.size + 1ull]);
            auto bytes_read = zip_fread(fileData, _file_bytes.get(), st.size);

            if (bytes_read > 0)
            {
                // In order to use QPixmapCache, we must save the file and then attempt to load it with QPixmap::load
                const QString filePath = QString("%1/%2").arg(cachePath, actualFileName);
                QFile _cache_qfile(filePath);
                if (_cache_qfile.open(QIODeviceBase::WriteOnly))
                {
                    auto bytesWritten = _cache_qfile.write(_file_bytes.get(), static_cast<qint64>(st.size));
                    _cache_qfile.close();

                    if (bytesWritten > 0)
                    {
                        auto b = px.load(filePath);

                        if (b)
                            px.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

                        zip_close(zip_archive);
                        return px;
                    }
                    else
                        logError("FindImage", QCoreApplication::translate("error", "Failed to write the file data of asset the %1").arg(asset));
                }
                else
                    logError("FindImage", QCoreApplication::translate("error", "File for asset %1 could not be created/open").arg(asset));
            }
            else
            {
                logError("FindImage", QCoreApplication::translate("error", "Image data for asset %1 was not read").arg(asset));
            }
        }
        else
            logError("FindImage", QCoreApplication::translate("error", "Image data size for asset %1 is not valid").arg(asset));
    }
    else
    {
        logError("FindImage", QCoreApplication::translate("error", "Image data stats for asset %1 are not valid").arg(asset));
    }

    zip_close(zip_archive);
#if defined(__clang__) || defined(__GNUC__)
    CENTAUR_WARN_POP()
#endif
    return {};
}
