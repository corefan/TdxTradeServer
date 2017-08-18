#ifndef TTS_ENCRYPT_H
#define TTS_ENCRYPT_H
#include "tts_setting.h"
#include "aes.h"

using namespace std;

/**
 * @brief The TTS_Encrypt class 使用AES_128_CBC进行加密 zeropadding...
 *
 */
class TTS_Encrypt
{
private:
    unsigned char key[16];
    unsigned char iv[16];
    AESModeOfOperation moo;

public:
    TTS_Encrypt(TTS_SettingObject so);
    ~TTS_Encrypt();

    // 进行加密
    string encryptString(const string plaintext);
    // 进行解密
    string decryptString(const string enctext);

    string toBase64(const string input);
    string fromBase64(const string input);
};

#endif // TTS_ENCRYPT_H
