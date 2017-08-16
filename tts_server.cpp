#include "tts_server.h"
#include "tts_tradeapi.h"
#include <QObject>
#include <memory>
#include <QDebug>

using namespace restbed;


TTS_Server::TTS_Server(TTS_SettingObject setting)
{
    _setting = setting;
    resource = make_shared< Resource >();
    restbed_settings = make_shared< Settings >();

}

void TTS_Server::start() {
    auto callback = bind(&TTS_Server::postMethodHandler, this);
    tradeApi = make_shared<TTS_TradeApi>(_setting.trade_dll_path);
    resource->set_path("/api");
    resource->set_method_handler("POST", callback);

    restbed_settings->set_port(_setting.port);
    restbed_settings->set_default_header("Connection", "close");

    service.publish(resource);
    qInfo() << "starting to listening.." ;
    service.start(restbed_settings);

}


void TTS_Server::stop() {
    service.stop();
}

void TTS_Server::postMethodHandler(const shared_ptr< Session > session) {
    const auto request = session->get_request();
    int contentLength = request->get_header("Content-Length", 0);

    session->fetch(contentLength, [] (const shared_ptr<Session> session, const Bytes& body) {

        //string requestBody(body.begin(), body.end());

        session->close(OK, body);
    });
}
