#ifndef TTS_TRADEAPI_H
#define TTS_TRADEAPI_H
#include <QtCore>
#include <Windows.h>
#include "json.hpp"
#include "tts_common.h"

using json = nlohmann::json;


typedef void (__stdcall *LPFN_OPENTDX)();
typedef void (__stdcall *LPFN_CLOSETDX)();

//  int  Logon(char* IP, short Port, char* Version, short YybID, char* AccountNo,char* TradeAccount, char* JyPassword,   char* TxPassword, char* ErrInfo);//登录帐号
// void  Logoff(int ClientID);//注销
// void  QueryData(int ClientID, int Category, char* Result, char* ErrInfo);//查询各类交易数据
// void  SendOrder(int ClientID, int Category ,int PriceType,  char* Gddm,  char* Zqdm , float Price, int Quantity,  char* Result, char* ErrInfo);//下单
// void  CancelOrder(int ClientID, char* ExchangeID, char* hth, char* Result, char* ErrInfo);//撤单
// void  GetQuote(int ClientID, char* Zqdm, char* Result, char* ErrInfo);//获取五档报价
// void  Repay(int ClientID, char* Amount, char* Result, char* ErrInfo);//融资融券账户直接还款

typedef int (__stdcall *LPFN_LOGON)(const char* IP, const short Port, const char* Version, short YybID,  const char* AccountNo, const char* TradeAccount, const char* JyPassword, const char* TxPassword, char* ErrInfo);
typedef void(__stdcall *LPFN_LOGOFF)(int ClientID);
typedef void(__stdcall *LPFN_QUERYDATA)(int ClientID, int Category, char* result, char* errInfo);
typedef void(__stdcall *LPFN_SENDORDER)(int ClientID, int Category ,int PriceType,  const char* Gddm,  const char* Zqdm , float Price, int Quantity,  char* Result, char* ErrInfo);
typedef void(__stdcall *LPFN_CANCELORDER)(int ClientID, const char* ExchangeID, const char* hth, char* Result, char* ErrInfo);
typedef void(__stdcall *LPFN_GETQUOTE)(int ClientID, const char* Zqdm, char* Result, char* ErrInfo);
typedef void(__stdcall *LPFN_REPAY)(int ClientID, const char* Amount, char* Result, char* ErrInfo);

#define P_LOGON         "logon"
#define P_LOGOFF        "logoff"
#define P_QUERYDATA     "query_data"
#define P_SENDORDER     "send_order"
#define P_CANCELORDER   "cancel_order"
#define P_GETQUOTE      "get_quote"
#define P_REPAY         "repay"

class TTS_TradeApi
{

private:
    HINSTANCE hDLL;
    int32_t lastClientId;
    /// api far call
    LPFN_OPENTDX lpOpenTdx;
    LPFN_CLOSETDX lpCloseTdx;
    LPFN_LOGON lpLogon;
    LPFN_LOGOFF lpLogoff;
    LPFN_QUERYDATA lpQueryData;
    LPFN_SENDORDER lpSendOrder;
    LPFN_CANCELORDER lpCancelOrder;
    LPFN_GETQUOTE lpGetQuote;
    LPFN_REPAY lpRepay;

    QMutex apiCallMutex; // add lock to all network call

    // store error and result;
    char* errout;
    char* result;

    bool outputUtf8;

    /// end api far call

    void setupErrForJson(const char* errout, json& resultJSON);

protected:
    json convertTableToJSON(const char* result, const char* errout);

public:
    TTS_TradeApi(const QString& dllFilePath);
    ~TTS_TradeApi();

    void setOutputUtf8(bool utf8);

    json logon(const char* IP, const short Port,
              const char* Version, short YybID,
              const char* AccountNo, const char* TradeAccount,
              const char* JyPassword, const char* TxPassword);

    json logoff(int ClientID);
    json queryData(int ClientID, int Category);
    json sendOrder(int ClientID, int Category ,int PriceType, const char* Gddm, const char* Zqdm , float Price, int Quantity);
    json cancelOrder(int ClientID, const char* ExchangeID, const char* hth);
    json getQuote(int ClientID, const char* Zqdm);
    json repay(int ClientID, const char* Amount);
    json jsonError(QString str);
};

#endif // TTS_TRADEAPI_H
