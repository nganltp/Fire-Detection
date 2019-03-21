#include "TGMTConfig.h"
#include "TGMTdebugger.h"
#include <fstream>
#include <sstream>
#include <string>
#include "TGMTfile.h"
#include "TGMTutil.h"

TGMTConfig* TGMTConfig::instance = nullptr;

TGMTConfig::TGMTConfig()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////

TGMTConfig::~TGMTConfig()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef UNICODE

bool TGMTConfig::LoadSettingFromFile(std::wstring settingFile)
{
	if(TGMTfile::FileExist(settingFile))
		m_iniFile = settingFile;
	else if(TGMTfile::FileExist(TGMTfile::GetCurrentDir() + L"\\" + settingFile))
		m_iniFile = TGMTfile::GetCurrentDir() + L"\\" + settingFile;
	else
		ASSERT(false, "Missing config file: %s ", settingFile.c_str());

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

std::wstring TGMTConfig::ReadSettingFromConfig(std::wstring iniFile, std::wstring section, std::wstring attribute)
{
	std::wstring result = L"";
	wchar_t retStr[1024] = { 0 };
	DWORD sizeRead = GetPrivateProfileString(section.c_str(), attribute.c_str(), NULL,
		retStr, _countof(retStr), iniFile.c_str());
	if (sizeRead > 0)
		result = retStr;
	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

std::wstring TGMTConfig::ReadValueString(std::wstring section, std::wstring key, std::wstring defaultValue)
{
	std::wstring temp = ReadSettingFromConfig(m_iniFile, section, key);
	if (temp != L"")
		return temp;
	else
		return defaultValue;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

int TGMTConfig::ReadValueInt(std::wstring section, std::wstring key, int defaultValue)
{
	std::wstring temp = ReadSettingFromConfig(m_iniFile, section, key);
	if (temp != L"")
		return atoi(TGMTutil::WString2String(temp).c_str());
	else
		return defaultValue;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

bool TGMTConfig::ReadValueBool(std::wstring section, std::wstring key, bool defaultValue)
{
	std::wstring temp = ReadSettingFromConfig(m_iniFile, section, key);
	temp = TGMTutil::ToLower(temp);
	if (temp == L"true")
		return true;
	else if (temp == L"false")
		return false;
	else
		return defaultValue;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

float TGMTConfig::ReadValueFloat(std::wstring section, std::wstring key, float defaultValue)
{
	std::wstring temp = ReadSettingFromConfig(m_iniFile, section, key);
	if (temp != L"")	
		return atof(TGMTutil::WString2String(temp).c_str());
	else
		return defaultValue;	
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void TGMTConfig::WriteConfigString(std::wstring section, std::wstring key, std::wstring value)
{
	WritePrivateProfileString(section.c_str(), key.c_str(), value.c_str(), m_iniFile.c_str());	
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void TGMTConfig::WriteConfigInt(std::wstring section, std::wstring key, int value)
{
	std::wstringstream ss;
	ss << value;
	WritePrivateProfileString(section.c_str(), key.c_str(), ss.str().c_str(), m_iniFile.c_str());
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void TGMTConfig::WriteConfigFloat(std::wstring section, std::wstring key, float value)
{
	std::wstringstream ss;
	ss << value;
	WritePrivateProfileString(section.c_str(), key.c_str(), ss.str().c_str(), m_iniFile.c_str());
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void TGMTConfig::WriteConfigBool(std::wstring section, std::wstring key, bool value)
{
	WritePrivateProfileString(section.c_str(), key.c_str(), value ? L"true" : L"false", m_iniFile.c_str());
}

//////////////////////////////////////////////////////////////////////////////////////////////////

#else

bool TGMTConfig::LoadSettingFromFile(std::string settingFile)
{
	ASSERT(TGMTfile::FileExist(settingFile), "Missing config file: %s ", settingFile.c_str());

	m_iniFile = TGMTfile::GetCurrentDir() + "\\" + settingFile;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

std::string TGMTConfig::ReadSettingFromConfig(std::string iniFile, std::string section, std::string attribute)
{
	char retStr[1024] = { 0 };
	std::string toRet = "";
	DWORD sizeRead = GetPrivateProfileString(section.c_str(), attribute.c_str(), NULL,
		retStr, _countof(retStr), iniFile.c_str());
	if (sizeRead > 0)
		toRet = retStr;

	return toRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

std::string TGMTConfig::ReadValueString(std::string section, std::string key, std::string defaultValue)
{
	std::string temp = ReadSettingFromConfig(m_iniFile, section, key);
	if (temp != "")
		return temp;
	else
		return defaultValue;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

int TGMTConfig::ReadValueInt(std::string section, std::string key, int defaultValue)
{
	std::string temp = ReadSettingFromConfig(m_iniFile, section, key);
	if (temp != "")
		return atoi(temp.c_str());
	else
		return defaultValue;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

bool TGMTConfig::ReadValueBool(std::string section, std::string key, bool defaultValue)
{
	std::string temp = ReadSettingFromConfig(m_iniFile, section, key);
	if (temp == "true")
		return true;
	else if (temp == "false")
		return false;
	else
		return defaultValue;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

float TGMTConfig::ReadValueFloat(std::string section, std::string key, float defaultValue)
{
	std::string temp = ReadSettingFromConfig(m_iniFile, section, key);
	if (temp != "")
		return atof(temp.c_str());
	else
		return defaultValue;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void TGMTConfig::WriteConfigString(std::string section, std::string key, std::string value)
{
	WritePrivateProfileString(section.c_str(), key.c_str(), value.c_str(), m_iniFile.c_str());
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void TGMTConfig::WriteConfigInt(std::string section, std::string key, int value)
{
	std::stringstream ss;
	ss << value;
	WritePrivateProfileString(section.c_str(), key.c_str(), ss.str().c_str(), m_iniFile.c_str());
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void TGMTConfig::WriteConfigFloat(std::string section, std::string key, float value)
{
	std::stringstream ss;
	ss << value;
	WritePrivateProfileString(section.c_str(), key.c_str(), ss.str().c_str(), m_iniFile.c_str());
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void TGMTConfig::WriteConfigBool(std::string section, std::string key, bool value)
{
	WritePrivateProfileString(section.c_str(), key.c_str(), value ? "true" : "false", m_iniFile.c_str());
}

#endif