#include <QCoreApplication>
#include <QSettings>
#include <QDebug>
#include <restbed>
#include "tts_setting.h"
#include "tts_tradeapi.h"
#include "tts_common.h"
#include "tts_server.h"
#include "tts_encrypt.h"

using namespace std;
using namespace restbed;

void test(TTS_SettingObject so);


void xMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QByteArray localMsg = msg.toLocal8Bit();
    QString now = QDateTime::currentDateTime().toString("MM/dd HH:mm:ss");
    fprintf(stderr, "%s TdxTradeServer> %s\n", now.toLocal8Bit().constData(), localMsg.constData());
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(xMessageHandler);


    cout << TTS_ASCII_WELCOME;
    // load settings
    TTS_SettingObject so = TTS_Setting::loadSettings();
    //test(so);
    //exit(-1);
    // TTS_TradeApi api(so.trade_dll_path);
    TTS_Server server(so);
    // start msg loop
    server.start();

    return EXIT_SUCCESS;
}


void test(TTS_SettingObject so) {
    string s = "x5gnMcO8%2FWV8CbsyKAWJJL3JYUMkK4jL7kod3a3UhyFRgHKIWHX6M%2Fdxl25GRam5fDKwis3dQVUuQ1cw3XDltBniXFBDTjzea0ibVAIQcxZQgDqKBK2J%2FHi3fi5tAnTQHPizhsuRY51fqEsAleDv3ZVVJFVaLNb0Wubg02h8RiXJnvPYcAO%2BBWUw4ZsLKi%2BaYRBsvw%2BUxyeAypPljKTASqNTvfh9vBTkTi4KiNrlEzkSPVfdvhFCJ3xJ3yOvrc58TXHVneJxN6iR%2FxlnBn5853k0h4CxdV7ru3VhsXaO7b%2B9aoCUddsidJH47fo%2F9%2BU1wtW7zKbaJjaBBuCwOlw%2BoGV9AAAAAAAAAAAAAAAAAADHRlOooBvMWqXmgHXwnBBN";
    TTS_Encrypt* e = new TTS_Encrypt(so);
    string decoded = restbed::Uri::decode(s);
    string un64 = e->fromBase64(s);
    string sx = e->decryptString(un64);

    cout << sx << endl;;
}
