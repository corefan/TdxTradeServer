#include "tts_setting.h"
#include <QSettings>
#include <QDir>
#include <QDebug>

#define TTS_SETTING_FILE_NAME QString::fromUtf8("TdxTradeServer.ini")
#define TTS_SETTING_DIR_NAME QString::fromUtf8("TdxTradeServer")

TTS_Setting::TTS_Setting(QObject *parent) : QObject(parent)
{

}


// 1. [current_path]/TdxTradeServer.ini
// 2. ~/TdxTradeServer/TdxTradeServer.ini
// 3. default config
QSettings* TTS_Setting::loadSettingsFile()
{
    QString ini_file = TTS_SETTING_FILE_NAME;
    QSettings* setting;
    if (QDir::current().exists(ini_file)) {
        setting = new QSettings(QDir::current().filePath(ini_file), QSettings::IniFormat);
        qInfo() << "Load setting file from " << setting->fileName();
        return setting;
    } else if (QDir::home().exists(TTS_SETTING_DIR_NAME)) {
        QDir dir = QDir::home();
        dir.cd(TTS_SETTING_DIR_NAME);
        if (dir.exists(ini_file)) {
            setting = new QSettings(dir.filePath(ini_file), QSettings::IniFormat);
            qInfo() << "Load setting file from " << setting->fileName();
            return setting;
        } else {
            qInfo() << "No setting file loaded, use default values";
            return NULL;
        }
    } else {
        qInfo() << "No setting file loaded, use default values";
        return NULL;
    }
}

TTS_SettingObject TTS_Setting::loadSettings()
{
    TTS_SettingObject so;

    QSettings* setting = TTS_Setting::loadSettingsFile();

    QDir currentDir = QDir::current();
    QString defaultPath = currentDir.cleanPath(DEFAULT_TRADE_DLL_NAME);
    // use default
    if (setting == NULL) {
        so.port = DEFAULT_PORT;
        so.trade_dll_path = defaultPath;
        so.bind = DEFAULT_BIND;
        so.ssl_enabled = false;
    } else {
        so.port = setting->value("port", DEFAULT_PORT).toInt();
        so.trade_dll_path = setting->value("trade_dll_path", defaultPath).toString();
        so.bind = setting->value("bind", QString(DEFAULT_BIND)).toString();
        so.ssl_enabled = setting->value("ssl_enabled").toBool();
        if (so.ssl_enabled) {
            so.ssl_certificate = setting->value("ssl_certificate", QString("")).toString();
            so.ssl_private_key = setting->value("ssl_private_key", QString("")).toString();
        }
        so.transport_enc_key = setting->value("transport_enc_key", QString("")).toString();
        so.transport_enc_iv = setting->value("transport_enc_iv", QString("")).toString();
    }

    qInfo() << "Using port : " << so.port;
    qInfo() << "Using dll file : " << so.trade_dll_path;
    qInfo() << "Bind Address is : " << so.bind;
    if (so.ssl_enabled) {
        qInfo() << "SSL connection is endabled";
        qInfo() << "SSL certificate : " << so.ssl_certificate;
        qInfo() << "SSL private key : " << so.ssl_private_key;
    }

    if (!so.transport_enc_key.isEmpty()) {
        // 检测有效性
        if (so.transport_enc_key.size() != 16) {
            qInfo() << "Error: transport_enc_key must be a string with size 16Byte (128bit)";
            exit(-1);
        }

        if (so.transport_enc_iv.size() != 16) {
            qInfo() << "Error: transport_enc_iv must be a string with size 16Bytes (128bit)";
            exit(-1);
        }

        so.transport_enc_enabled = true;
        qInfo() << "Transport Sign Key enabled! ";
    } else {
        so.transport_enc_enabled = false;
        qInfo() << "Transport Sign Key **NOT** enabled! ";
    }

    delete setting;

    // check if dll exists
    if (!QFileInfo(so.trade_dll_path).exists()) {
        qInfo() << "Dll file not exits, Please Check!!!!";
        exit(-1);
    }
    return so;
}
