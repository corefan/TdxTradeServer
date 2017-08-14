#ifndef TTS_SETTING_H
#define TTS_SETTING_H

#include <QObject>
#include <QSettings>

#define DEFAULT_TRADE_DLL_NAME  "trade.dll"
#define DEFAULT_PORT            19820

typedef struct _TTS_SettingObject {
    int32_t     port;
    QString     trade_dll_path;
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
