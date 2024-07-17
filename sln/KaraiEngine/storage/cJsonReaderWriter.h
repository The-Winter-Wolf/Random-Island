#pragma once
#include "cConfigData.h"

#include <rapidjson/filewritestream.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

using namespace rapidjson;

class cJsonReaderWriter
{
public:
	cJsonReaderWriter(cConfigData* pConfiguration);
	~cJsonReaderWriter();

	bool ReadConfiguration();

	bool WriteConfiguration();

private:
	cConfigData* pConfig;

	bool m_ReadAudioConfig();
	bool m_ReadLightConfig();
	bool m_ReadCameraConfig();
	bool m_ReadObjectConfig();
	bool m_ReadTextureConfig();
	bool m_ReadAnimationConfig();

	bool m_GetValue(Value& valueIn, int index, int& valueOut);
	bool m_GetValue(Value& valueIn, int index, bool& valueOut);
	bool m_GetValue(Value& valueIn, int index, float& valueOut);
	bool m_GetValue(Value& valueIn, int index, double& valueOut);
	bool m_GetValue(Value& valueIn, int index, std::string& valueOut);

	bool m_GetValue(Value& valueIn, const char* paramName, int& valueOut);
	bool m_GetValue(Value& valueIn, const char* paramName, bool& valueOut);
	bool m_GetValue(Value& valueIn, const char* paramName, float& valueOut);
	bool m_GetValue(Value& valueIn, const char* paramName, double& valueOut);
	bool m_GetValue(Value& valueIn, const char* paramName, glm::vec2& valueOut);
	bool m_GetValue(Value& valueIn, const char* paramName, glm::vec3& valueOut);
	bool m_GetValue(Value& valueIn, const char* paramName, glm::vec4& valueOut);
	bool m_GetValue(Value& valueIn, const char* paramName, std::string& valueOut);

	bool m_SetValue(Value& valueIn, const char* paramName, const std::string& stringIn);
	bool m_SetValue(Value& valueIn, const char* paramName, const int& numberIn);
};



