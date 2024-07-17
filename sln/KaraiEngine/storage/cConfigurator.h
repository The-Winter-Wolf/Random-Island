#pragma once

#include "cConfigData.h"
#include "cXmlReaderWriter.h"
#include "cJsonReaderWriter.h"

class cConfigurator
{
public:
	cConfigurator();
	~cConfigurator();

	void ReadConfigFromFile();

	void WriteConfigToFile();

	void SetConfigPath(std::string configPath);

private:
	cConfigData* m_pConfig;
	cXmlReaderWriter* m_pXmlRW;
	cJsonReaderWriter* m_pJsonRW;

	std::string m_JsonPath;
	std::string m_XmlPath;

	void m_CameraFromConfig();
	void m_CameraToConfig();

	void m_LightFromConfig();
	void m_LightToConfig();	

	void m_ObjectFromConfig();
	void m_ObjectToConfig();
};