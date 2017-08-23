#include "tts_tradeapi.h"
#include "Windows.h"
#include <QDebug>
#include "json.hpp"
#include "tts_common.h"
#include <QTextCodec>

using namespace std;
using json = nlohmann::json;


TTS_TradeApi::TTS_TradeApi(const QString& dllFilePath)
{
    outputUtf8 = true;
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
    lpSendOrder = (LPFN_SENDORDER) GetProcAddress(hDLL, "SendOrder");
    lpCancelOrder = (LPFN_CANCELORDER) GetProcAddress(hDLL, "CancelOrder");
    lpGetQuote = (LPFN_GETQUOTE) GetProcAddress(hDLL, "GetQuote");
    lpRepay = (LPFN_REPAY) GetProcAddress(hDLL, "Repay");
    // end load functioins

    // initialize tdx
    lpOpenTdx();

    errout = new char[1024];
    result = new char[1024 * 1024];
    // 这里强制设定本地编码为gbk
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB18030"));
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

void TTS_TradeApi::setOutputUtf8(bool utf8) {
    outputUtf8 = utf8;
}

/**
 * @brief TTS_TradeApi::logon 登陆到服务器
 * @param IP 服务器ip地址
 * @param Port 端口号
 * @param Version 客户端版本
 * @param YybID 营业部id
 * @param AccountNo 帐号
 * @param TradeAccount 交易帐号
 * @param JyPassword 交易密码
 * @param TxPassword 通讯密码
 * @return data -> { "client_id" : xxx }
 *
 */
json TTS_TradeApi::logon(const char* IP, const short Port,
                        const char* Version, short YybID,
                        const char* AccountNo, const char* TradeAccount,
                        const char* JyPassword, const char* TxPassword) {
    json j;
    int ret = lpLogon(IP, Port, Version, YybID, AccountNo, TradeAccount, JyPassword, TxPassword, errout);
    if (ret == -1) {
        j[TTS_SUCCESS] = false;
        setupErrForJson(errout, j);
        return j;
    } else {
        j[TTS_SUCCESS] = true;
        j[TTS_DATA]["client_id"] = ret;
    }
    return j;
}

/**
 * @brief TTS_TradeApi::logoff 登出
 * @param ClientID cilent_id
 * @return success => true/false
 */
json TTS_TradeApi::logoff(int ClientID) {
    lpLogoff(ClientID);
    json j;
    j[TTS_SUCCESS] = true;
    return j;
}

/**
 * @brief TTS_TradeApi::queryData 查询信息
 * @param ClientID client_id
 * @param Category 信息类别
 * @return [{}, {}, {} ]
 */
json TTS_TradeApi::queryData(int ClientID, int Category) {
    lpQueryData(ClientID, Category, result, errout);
    return convertTableToJSON(result, errout);
}


json TTS_TradeApi::sendOrder(int ClientID, int Category ,int PriceType, const char* Gddm,  const char* Zqdm , float Price, int Quantity) {
    lpSendOrder(ClientID, Category, PriceType, Gddm, Zqdm, Price, Quantity, result, errout);
    return convertTableToJSON(result, errout);
}

json TTS_TradeApi::cancelOrder(int ClientID, const char *ExchangeID, const char *hth) {
    lpCancelOrder(ClientID, ExchangeID, hth, result, errout);
    return convertTableToJSON(result, errout);
}

json TTS_TradeApi::getQuote(int ClientID, const char *Zqdm) {
    lpGetQuote(ClientID, Zqdm, result, errout);
    return convertTableToJSON(result, errout);
}

json TTS_TradeApi::repay(int ClientID, const char *Amount) {
    lpRepay(ClientID, Amount, result, errout);
    return convertTableToJSON(result, errout);
}


void TTS_TradeApi::setupErrForJson(const char* errout, json& resultJSON)
{
    if (outputUtf8) {
        QString qErrout = QString::fromLocal8Bit(errout);
        string utf8Errout = qErrout.toUtf8().constData();
        resultJSON[TTS_ERROR] = utf8Errout;
    } else {
        resultJSON[TTS_ERROR] = errout;
    }
}


/**
 * @brief TTS_TradeApi::convertTableToJSON 将\n分割行\t分割字符的类似 csv格式的信息转换为json格式
 * @param result
 * @return  json结构的 [{line1}, {line2} ... ] 信息
 */

json TTS_TradeApi::convertTableToJSON(const char *result, const char* errout) {

    json resultJSON;
    if (result[0] == 0) {
        resultJSON[TTS_SUCCESS] = false;
        setupErrForJson(errout, resultJSON);
        return resultJSON;
    }

    json j;
    j = json::array();
    QString strResult = QString::fromLocal8Bit(result);
    // qInfo() << strResult;
    QStringList sl = strResult.split("\n");
    if (sl.length() > 1) {
        QString head = sl[0];
        QStringList hlist = head.split("\t");

        int line = 0;
        for (QString row : sl) {
            line++;
            if (line==1) continue;

            QStringList rowlist = row.split("\t");

            json oneRecord = json({});
            for(int i = 0; i < hlist.length(); i++) {
                string key, value;
                if (outputUtf8) {
                    key = hlist[i].toUtf8();
                    value = rowlist[i].toUtf8();
                } else {
                    key = hlist[i].toLocal8Bit();
                    value = rowlist[i].toLocal8Bit();
                }

                oneRecord[key]= value;
            }
            j.push_back(oneRecord);
        }
    }
    resultJSON[TTS_SUCCESS] = true;
    resultJSON[TTS_DATA] = j;
    return resultJSON;
}


json TTS_TradeApi::jsonError(QString str) {
    string value;
    if (outputUtf8) {
        value = str.toUtf8();
    } else {
        value = str.toLocal8Bit();
    }
    json j;
    j[TTS_SUCCESS] = false;
    j[TTS_ERROR] = value;
    return j;
}


