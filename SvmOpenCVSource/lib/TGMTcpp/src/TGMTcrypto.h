/*
    using openssl to encrypt/descript
*/

#pragma once
#include <string>
#include <vector>

class TGMTcrypto
{
	static std::string EncodeBase64(std::string value);
	static std::string DecodeBase64(std::string encoded_string);
public:
    //TGMTcrypto();
    //~TGMTcrypto();

    static std::string EncryptAES(std::string value, std::string password);
    static std::string DecryptAES(std::string value, std::string password);
};

