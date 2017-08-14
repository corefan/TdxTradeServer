#include "tts_tradeapi.h"
#include "Windows.h"
#include <QDebug>
#include "json.hpp"
#include "tts_common.h"

using namespace std;
using json = nlohmann::json;


TTS_TradeApi::TTS_TradeApi(const QString& dllFilePath)
{
    string strDllFilePath = dllFilePath.toStdString();
    hDLL = LoadLibraryA(strDllFilePath.c_str());

    if (0 == hDLL) {
        qInfo() << "Load dll failed";
        exit(-1);
    }

    // load functions
    lpOpenTdx = (LPFN_OPENTDX) GetProcAddress(hDLL, "OpenTdx");
    lpCloseTdx = (LPFN_CLOSETDX) GetProcAddress(hDLL, "CloseTdx");
    lpLogon = (LPFN_LOGON) GetProcAddress(hDLL, "Logon");
    lpLogoff = (LPFN_LOGOFF) GetProcAddress(hDLL, "Logoff");
    lpQueryData = (LPFN_QUERYDATA) GetProcAddress(hDLL, "QueryData");
    // end load functioins

    // initialize tdx
    lpOpenTdx();

    errout = new char[1024];
    result = new char[1024 * 1024];
}

TTS_TradeApi::~TTS_TradeApi()
{
    // close tdx
    lpCloseTdx();
    if (hDLL) {
        FreeLibrary(hDLL);
    }
    delete[] errout;
    delete[] result;
}

json TTS_TradeApi::logon(const char* IP, const short Port,
                        const char* Version, short YybID,
                        const char* AccountNo, const char* TradeAccount,
                        const char* JyPassword, const char* TxPassword) {
    json j;
    int ret = lpLogon(IP, Port, Version, YybID, AccountNo, TradeAccount, JyPassword, TxPassword, errout);
    if (ret == -1) {
        j[TTS_SUCCESS] = false;
        j[TTS_ERROR] = errout;
        return j;
    } else {
        j[TTS_SUCCESS] = true;
        j[TTS_DATA]["client_id"] = ret;
    }
    return j;
}

json TTS_TradeApi::logoff(int ClientID) {
    lpLogoff(ClientID);
    json j;
    j[TTS_SUCCESS] = true;
    return j;
}

json TTS_TradeApi::queryData(int ClientID, int Category) {
    lpQueryData(ClientID, Category, result, errout);

    json j;
    if (result[0] == 0) {
        j[TTS_SUCCESS] = false;
        j[TTS_ERROR] = errout;
        return j;
    }

    QString strResult = result;
    qInfo() << strResult;
    QStringList sl = strResult.split("\n");

    j[TTS_SUCCESS] = true;
    j[TTS_DATA] = json::array();

    if (sl.length() > 1) {
        QString head = sl[0];
        QStringList hlist = head.split("\t");

        int line = 0;
        for (QString row : sl) {
            line++;
            if (line==1) continue;

            QStringList rowlist = row.split("\t");

            json oneRecord = json({});
            for(int i; i < hlist.length(); i++) {
                oneRecord[hlist[i].toStdString()]= rowlist[i].toStdString();
            }
            j[TTS_DATA].push_back(oneRecord);
        }
    }

    return j;
}
