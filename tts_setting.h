#ifndef TTS_SETTING_H
#define TTS_SETTING_H

#include <QObject>
#include <QSettings>

#define DEFAULT_TRADE_DLL_NAME  "trade.dll"
#define DEFAULT_PORT            19820
#define DEFAULT_BIND            "127.0.0.1"

typedef struct _TTS_SettingObject {
    int32_t     port;               // 监听端口
    QString     trade_dll_path;     // dll路径
    QString     bind;               // 邦定ip
    bool        ssl_enabled;        // 是否启用ssl
    QString     ssl_private_key;    // 私钥
    QString     ssl_certificate;    // CA 证书
    QString     transport_enc_key; // 对传输进行签名的密钥
    QString     transport_enc_iv;  //
    bool        transport_enc_enabled; // 是否开启了签名加密
}  TTS_SettingObject;

class TTS_Setting : public QObject
{
    Q_OBJECT
public:
    explicit TTS_Setting(QObject *parent = nullptr);


public:
    static QSettings* loadSettingsFile();
    static TTS_SettingObject loadSettings();

signals:

public slots:
};

#endif // TTS_SETTING_H
