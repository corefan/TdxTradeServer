#include "tts_encrypt.h"
#include "aes.h"
#include <QDebug>
#include <iostream>

using namespace std;

TTS_Encrypt::TTS_Encrypt(TTS_SettingObject so)
{
    // init key and iv
    memcpy(key, so.transport_enc_key.toLocal8Bit().constData(), 16);
    memcpy(iv, so.transport_enc_iv.toLocal8Bit().constData(), 16);
    moo.set_key(key);
    moo.set_mode(MODE_CBC);
    moo.set_iv(iv);
}

TTS_Encrypt::~TTS_Encrypt()
{

}

string TTS_Encrypt::encryptString(const string plaintext)
{

    int plaintextSize = plaintext.size();
    int encryptedDataSize = plaintextSize + 32; // alloc more space

    string paddedtext;

    int needToPadding = 16 - plaintextSize % 16;

    int paddedSize = plaintextSize + needToPadding;

    // 添加zeropadding
    if (needToPadding != 0) {
        string padding(needToPadding, '\x00');
        paddedtext = plaintext + padding;
    } else {
        paddedtext = plaintext;
    }

    unsigned char* encryptedData = new unsigned char[encryptedDataSize];
    int len = moo.Encrypt((unsigned char*) paddedtext.data(), paddedSize, encryptedData);
    string estr;
    estr.assign((char*)encryptedData, len);
    delete[] encryptedData;
    return estr;
}

string TTS_Encrypt::decryptString(const string enctext)
{
    int encsize = enctext.size();
    unsigned char* decdata = new unsigned char[encsize];
    int len = moo.Decrypt( (unsigned char*) enctext.data(), encsize, decdata );
    string dstr;
    dstr.assign((char*)decdata, len);
    delete[] decdata;
    return dstr;
}

string TTS_Encrypt::toBase64(const string input)
{
    QByteArray qb = QByteArray::fromStdString(input);
    QByteArray qb64 = qb.toBase64();
    string out = qb64.data();
    return out;
}

string TTS_Encrypt::fromBase64(const string input)
{
    QByteArray qb64 = QByteArray::fromStdString(input);
    QByteArray qb = qb64.fromBase64(qb64);
    string out = qb.data();
    return out;
}

