#pragma once
#include <vector>
#include <string>

class TGMTutil
{
public:
	//TGMTutil();
	//~TGMTutil();

	static std::string IntToString(int number);
	static std::wstring String2WString(const std::string s);
	static std::string WString2String(const std::wstring s);
	
	static std::string FormatString(const char* fmt, ...);
	static std::string ReplaceString(std::string &inputStr, char oldchar, char newchar);
	static std::string RemoveString(std::string inputStr, char chrWantRemove);
	static std::string RemoveSpecialCharacter(std::string inputStr);

#ifndef ANDROID
	static std::string GetParameter(int argc, char* argv[], char* key, char* defaultValue = "");
	static bool CheckParameterExist(int argc, char* argv[], char* key);
	static bool CheckParameterAloneExist(int argc, char* argv[], char* key);
#endif

	
	static std::vector<std::string> SplitString(std::string str, char separator);
	static std::string JoinVectorString(std::vector<std::string> strings, char* separator = ";");

	static int GetRandomInt(int min = 0, int max = 255);
#ifdef UNICODE
	static std::wstring ToLower(std::wstring str);
#else
	static std::string ToLower(std::string str);
#endif
};

