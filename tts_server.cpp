#include "tts_server.h"
#include "tts_tradeapi.h"
#include <QObject>
#include <memory>
#include <QDebug>
#include <json.hpp>

using namespace restbed;
using json = nlohmann::json;

TTS_Server::TTS_Server(TTS_SettingObject setting)
{
    _setting = setting;
    resource = make_shared< Resource >();
    statusResource = make_shared< Resource >();
    restbed_settings = make_shared< Settings >();
    ssl_settings = make_shared<SSLSettings>();
    encryter = make_shared<TTS_Encrypt>(setting);
}

void TTS_Server::start() {
    reqnum = 0;
    auto callback = bind(&TTS_Server::postMethodHandler, this, placeholders::_1);
    tradeApi = make_shared<TTS_TradeApi>(_setting.trade_dll_path);
    resource->set_path("/api");
    resource->set_method_handler("POST", callback);

    statusResource->set_path("/status");
    statusResource->set_method_handler("GET", [&](const shared_ptr< Session > session){
        const auto request = session->get_request();
        json j;
        j["success"] = true;
        j["reqnum"] = reqnum;
        session->close(OK, j.dump());
    });

    //restbed_settings->set_bind_address(_setting.bind.toStdString());
    restbed_settings->set_default_header("Connection", "close");

    if (_setting.ssl_enabled) {
       ssl_settings->set_http_disabled(true);
       ssl_settings->set_port(_setting.port);
       ssl_settings->set_certificate(Uri(_setting.ssl_certificate.toStdString()));
       ssl_settings->set_private_key(Uri(_setting.ssl_private_key.toStdString()));
       restbed_settings->set_ssl_settings(ssl_settings);
    } else {
        restbed_settings->set_port(_setting.port);
    }


    service.publish(resource);
    service.publish(statusResource);
    qInfo() << "Starting to listening.." ;
    service.start(restbed_settings);

}


void TTS_Server::stop() {
    qInfo() << "GoodByte!";
    service.stop();
}

/**
 * @brief TTS_Server::postMethodHandler
 *
 * 这里，请求体的结构应为
 * {
 *      "func": "Logon",
 *      "params": {
 *          ..
 *          ..
 *      }
 * }
 *
 * @param session
 */
void TTS_Server::performResponse(const shared_ptr< Session > session, string output)
{
    if (_setting.transport_enc_enabled) {
        string encoutput = encryter->encryptString(output);
        string encoutput64 = encryter->toBase64(encoutput);
        string urlencodedencoutput64 = restbed::Uri::encode(encoutput64);
        session->close(OK, urlencodedencoutput64);
    } else {
        session->close(OK, output);
    }
}

void TTS_Server::postMethodHandler(const shared_ptr< Session > session) {
    const auto request = session->get_request();
    int contentLength = request->get_header("Content-Length", 0);

    session->fetch(contentLength, [&] (const shared_ptr<Session> session, const Bytes& body) {
        string requestBody(body.begin(), body.end());
        reqnum++;

        if (_setting.transport_enc_enabled) {
            string requestBodyUnquote =  restbed::Uri::decode(requestBody);
            string requestBodyPlain = encryter->fromBase64(requestBodyUnquote);
            requestBody = encryter->decryptString(requestBodyPlain);
        }

        json requestJson = json::parse(requestBody);

        if (requestJson["func"].is_null()) {
            QString _err= "parameter func does not exists";
            string _noFunc = tradeApi->jsonError(_err).dump();
            performResponse(session, _noFunc);
            return;
        }

        string func = requestJson["func"].get<string>();
        qInfo("Receiving request func=%s", func.c_str());
        string responseBody;
        auto params = requestJson["params"];
        // 参数的解析，后续应该用Command等模式将实现放到具体的类中
        if (func == P_LOGON) {
            if (params["ip"].is_string() && params["port"].is_number() && params["version"].is_string()
                    && params["yyb_id"].is_number() && params["account_no"].is_string()
                    && params["trade_account"].is_string()
                    && params["jy_password"].is_string() && params["tx_password"].is_string()) {
                string ip = params["ip"].get<string>();
                int port = params["port"].get<int>();
                string version = params["version"].get<string>();
                int yybId = params["yyb_id"].get<int>();
                string accountNo = params["account_no"].get<string>();
                string tradeAccount = params["trade_account"].get<string>();
                string jyPassword = params["jy_password"].get<string>();
                string txPassword = params["tx_password"].get<string>();

                responseBody = tradeApi->logon(ip.c_str(), port, version.c_str(), yybId, accountNo.c_str(), tradeAccount.c_str(), jyPassword.c_str(), txPassword.c_str()).dump();
            } else {
                responseBody = tradeApi->jsonError("error params").dump();
            }

        } else if (func == P_LOGOFF) {
            if (params["client_id"].is_number()) {
                responseBody = tradeApi->logoff(params["client_id"].get<int>()).dump();
            } else {
                responseBody = tradeApi->jsonError("error params").dump();
            }
        } else if (func == P_QUERYDATA) {
            if (params["client_id"].is_number()
                    && params["category"].is_number()
                    ) {
                int clientId = params["client_id"].get<int>();
                int category = params["category"].get<int>();
                responseBody = tradeApi->queryData(clientId, category).dump();
            } else {
                responseBody = tradeApi->jsonError("error params").dump();
            }
        } else if (func == P_SENDORDER) {
            if (params["client_id"].is_number()
                    && params["category"].is_number()
                    && params["price_type"].is_number()
                    && params["gddm"].is_string()
                    && params["zqdm"].is_string()
                    && params["price"].is_number()
                    && params["quantity"].is_number()) {
                responseBody = tradeApi->sendOrder(
                            params["client_id"].get<int>(),
                            params["category"].get<int>(),
                        params["price_type"].get<int>(),
                        params["gddm"].get<string>().c_str(),
                        params["zqdm"].get<string>().c_str(),
                        params["price"].get<float>(),
                        params["quantity"].get<int>()
                            ).dump();
            } else {
                responseBody = tradeApi->jsonError("error params").dump();
            }
        } else if (func == P_CANCELORDER)
        {
            if (params["client_id"].is_number()
                    && params["exchange_id"].is_string()
                    && params["hth"].is_string()) {
                responseBody = tradeApi->cancelOrder(
                            params["client_id"].get<int>(),
                        params["exchange_id"].get<string>().c_str(),
                        params["hth"].get<string>().c_str()
                            ).dump();
            } else {
                responseBody = tradeApi->jsonError("error params").dump();
            }
        } else if (func == P_GETQUOTE) {
            if (params["client_id"].is_number()
                    && params["code"].is_string()){
                responseBody = tradeApi->getQuote(params["client_id"].get<int>(),
                        params["code"].get<string>().c_str()).dump();
            } else {
                responseBody = tradeApi->jsonError("error params").dump();
            }
        } else if (func == P_REPAY) {
            if (params["client_id"].is_number()
                    && params["amount"].is_string()) {
                responseBody = tradeApi->repay(params["client_id"].get<int>(), params["amount"].get<string>().c_str()).dump();
            } else {
                responseBody = tradeApi->jsonError("error params").dump();
            }
        } else if (func == "stop_server") {
            qInfo() << "Server Stop Command Called!";
            stop();
        } else if (func == "ping") {
            responseBody = "{\"success\":true, \"data\": \"pong\"}";
        } else {
            responseBody = "{\"success\":false, \"error\": \"unknown command\"}";
        }

        performResponse(session, responseBody);
    });
}
