#ifndef TTS_SERVER_H
#define TTS_SERVER_H
#include "tts_setting.h"
#include "tts_tradeapi.h"
#include "tts_encrypt.h"
#include <restbed>
#include <memory>

using namespace restbed;
using namespace std;

class TTS_Server
{

private:
    TTS_SettingObject _setting;
    Service service;
    shared_ptr<Resource> resource;
    shared_ptr<Resource> statusResource;
    shared_ptr<Settings> restbed_settings;
    shared_ptr<SSLSettings> ssl_settings;
    shared_ptr<TTS_TradeApi> tradeApi;
    shared_ptr<TTS_Encrypt> encryter;



    unsigned long reqnum;
    void performResponse(const shared_ptr< Session > session, string _noFunc);
    
public:
    TTS_Server(TTS_SettingObject setting);

    void start();
    void stop();

    void postMethodHandler(const shared_ptr< Session > session);
};

#endif // TTS_SERVER_H
