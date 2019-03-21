#pragma once
#include <string>


#define GetTGMTConfig TGMTConfig::GetInstance
#define USE_JSON 0

class TGMTConfig
{
	static TGMTConfig* instance;
#ifdef UNICODE
	std::wstring m_iniFile;

	std::wstring TGMTConfig::ReadSettingFromConfig(std::wstring iniFile, std::wstring section, std::wstring attribute);
#else
	std::string m_iniFile;

	std::string TGMTConfig::ReadSettingFromConfig(std::string iniFile, std::string section, std::string attribute);
#endif
public:
	static TGMTConfig* GetInstance()
	{
		if (!instance)
			instance = new TGMTConfig();
		return instance;
	}

	TGMTConfig();
	~TGMTConfig();

#ifdef UNICODE
	bool LoadSettingFromFile(std::wstring settingFile);
	
	std::wstring ReadValueString(std::wstring section, std::wstring key, std::wstring defaultValue = L"");
	int ReadValueInt(std::wstring section, std::wstring key, int defaultValue = 0);
	bool ReadValueBool(std::wstring section, std::wstring key, bool defaultValue = false);
	float ReadValueFloat(std::wstring section, std::wstring key, float defaultValue = 0.f);

	void WriteConfigString(std::wstring section, std::wstring key, std::wstring value);
	void WriteConfigInt(std::wstring section, std::wstring key, int value);
	void WriteConfigFloat(std::wstring section, std::wstring key, float value);
	void WriteConfigBool(std::wstring section, std::wstring key, bool value);	
#else
	bool LoadSettingFromFile(std::string settingFile);

	std::string ReadValueString(std::string section, std::string key, std::string defaultValue = "");
	int ReadValueInt(std::string section, std::string key, int defaultValue = 0);
	bool ReadValueBool(std::string section, std::string key, bool defaultValue = false);
	float ReadValueFloat(std::string section, std::string key, float defaultValue = 0.f);

	void WriteConfigString(std::string section, std::string key, std::string value);
	void WriteConfigInt(std::string section, std::string key, int value);
	void WriteConfigFloat(std::string section, std::string key, float value);
	void WriteConfigBool(std::string section, std::string key, bool value);
#endif
};

