#include "cJsonReaderWriter.h"

#include "../system/window/cConsoleLogs.h"
extern cConsoleLogs* g_pConsole;

#include <iostream>

cJsonReaderWriter::cJsonReaderWriter(cConfigData* pConfiguration)
{
	this->pConfig = pConfiguration;
}

cJsonReaderWriter::~cJsonReaderWriter() {}

bool cJsonReaderWriter::ReadConfiguration()
{
	m_ReadCameraConfig();

	m_ReadLightConfig();

	m_ReadObjectConfig();

	return true;
}

bool cJsonReaderWriter::WriteConfiguration()
{
	//// Save data to file
	//FILE* jsonFile = 0;
	//fopen_s(&jsonFile, configIn.preferenceFile.c_str(), "rb");

	//char readBuffer[65536];
	//FileReadStream is(jsonFile, readBuffer, sizeof(readBuffer));
	//Document doc;
	//doc.ParseStream(is);
	//fclose(jsonFile);

	//if (!doc.IsObject())
	//{
	//	return false;
	//}

	//if (!SetValue(doc["PrefLanguage"], prefsIn.prefLanguage))
	//{
	//	return false;
	//}

	//if (!SetValue(doc["PrefName"], prefsIn.prefName))
	//{
	//	return false;
	//}

	//if (!SetValue(doc["PrefAge"], prefsIn.prefAge))
	//{
	//	return false;
	//}

	//if (!SetValue(doc["PrefAnimal"], prefsIn.prefAnimal))
	//{
	//	return false;
	//}

	//fopen_s(&jsonFile, configIn.preferenceFile.c_str(), "wb");
	//char writeBuffer[65536];
	//FileWriteStream os(jsonFile, writeBuffer, sizeof(writeBuffer));
	//PrettyWriter<FileWriteStream>writer(os);

	//doc.Accept(writer);

	//fclose(jsonFile);

	return true;
}

bool cJsonReaderWriter::m_ReadCameraConfig()
{
	FILE* jsonFile = 0;
	std::string fullFileName = pConfig->jsonPath + "/" + pConfig->cameraFileName + ".json";
	fopen_s(&jsonFile, fullFileName.c_str(), "rb");
	if (jsonFile == nullptr)
	{
		g_pConsole->cError("Config reading error. File " + fullFileName + " not found.");
		return false;
	}

	Document doc;
	char readBuffer[65536];
	FileReadStream fileStream(jsonFile, readBuffer, sizeof(readBuffer));
	doc.ParseStream(fileStream);
	if (!doc.IsObject())
	{
		g_pConsole->cError("Config reading error. File " + fullFileName + " is not correct.");
		return false;
	}

	m_GetValue(doc, "Width", pConfig->camera.width);
	m_GetValue(doc, "Height", pConfig->camera.height);
	m_GetValue(doc, "CameraMod", pConfig->camera.mod);
	m_GetValue(doc, "Position", pConfig->camera.position);
	m_GetValue(doc, "Direction", pConfig->camera.direction);
	m_GetValue(doc, "FOVMinMax", pConfig->camera.fovMinMax);
	m_GetValue(doc, "PitchYawSpeed", pConfig->camera.pitchYawSpeed);
	m_GetValue(doc, "PitchMinMax", pConfig->camera.pitchMinMax);
	m_GetValue(doc, "YawMinMax", pConfig->camera.yawMinMax);
	m_GetValue(doc, "DistanceMinMax", pConfig->camera.distMinMax);
	m_GetValue(doc, "MoveSpeed", pConfig->camera.moveSpeed);
	m_GetValue(doc, "SpeedScale", pConfig->camera.speedScale);

	fclose(jsonFile);

	g_pConsole->cSuccess("The camera configuration was successfully read from an external file");
	return true;
}

bool cJsonReaderWriter::m_ReadLightConfig()
{
	FILE* jsonFile = 0;
	std::string fullFileName = pConfig->jsonPath + "/" + pConfig->lightsFileName + ".json";
	fopen_s(&jsonFile, fullFileName.c_str(), "rb");
	if (jsonFile == nullptr)
	{
		g_pConsole->cError("Config reading error. File " + fullFileName + " not found.");
		return false;
	}

	Document doc;
	char readBuffer[65536];
	FileReadStream fileStream(jsonFile, readBuffer, sizeof(readBuffer));
	doc.ParseStream(fileStream);
	if (!doc.IsObject())
	{
		g_pConsole->cError("Config reading error. File " + fullFileName + " is not correct.");
		return false;
	}
	if (!doc.HasMember("Lights"))
	{
		g_pConsole->cError("Config reading error. File " + fullFileName + " is not correct.");
		return false;
	}

	Value& lights = doc["Lights"];
	if (!lights.IsArray())
	{
		g_pConsole->cError("Config reading error. File " + fullFileName + " is not correct.");
		return false;
	}
	if (lights.Size() == 0)
	{
		g_pConsole->cWarning("Config reading warning. File " + fullFileName + " has zero elements.");
		return false;
	}
	
	for (int index = 0; index != lights.Size(); index++)
	{
		m_GetValue(lights[index], "Name", pConfig->light.name);
		m_GetValue(lights[index], "State", pConfig->light.state);
		if (pConfig->light.state == "Off" || pConfig->light.state == "0")
		{
			pConfig->light.prm2.x = 0.0f;
		}
		if (pConfig->light.state == "On" || pConfig->light.state == "1")
		{
			pConfig->light.prm2.x = 1.0f;
		}
		m_GetValue(lights[index], "Type", pConfig->light.type);
		if (pConfig->light.type == "Point")
		{
			pConfig->light.prm1.x = 0.0f;
		}
		if (pConfig->light.type == "Spot")
		{
			pConfig->light.prm1.x = 1.0f;
		}
		if (pConfig->light.type == "Direct")
		{
			pConfig->light.prm1.x = 2.0f;
		}
		m_GetValue(lights[index], "InAngle", pConfig->light.prm1.y);
		m_GetValue(lights[index], "OutAngle", pConfig->light.prm1.z);
		m_GetValue(lights[index], "LinearAtten", pConfig->light.atten.y);
		m_GetValue(lights[index], "QuadAtten", pConfig->light.atten.z);
		m_GetValue(lights[index], "Diffuse", pConfig->light.diff);
		m_GetValue(lights[index], "Specular", pConfig->light.spec);
		m_GetValue(lights[index], "Position", pConfig->light.pos);
		m_GetValue(lights[index], "Direction", pConfig->light.dir);

		pConfig->vLights.push_back(pConfig->light);
	}

	fclose(jsonFile);

	g_pConsole->cSuccess("The lights configuration was successfully read from an external file");
	return true;
}

bool cJsonReaderWriter::m_ReadObjectConfig()
{
	FILE* jsonFile = 0;
	std::string fullFileName = pConfig->jsonPath + "/" + pConfig->objectsFileName + ".json";
	fopen_s(&jsonFile, fullFileName.c_str(), "rb");
	if (jsonFile == nullptr)
	{
		g_pConsole->cError("Config reading error. File " + fullFileName + " not found.");
		return false;
	}

	Document doc;
	char readBuffer[65536];
	FileReadStream fileStream(jsonFile, readBuffer, sizeof(readBuffer));
	doc.ParseStream(fileStream);
	if (!doc.IsObject())
	{
		g_pConsole->cError("Config reading error. File " + fullFileName + " is not correct.");
		return false;
	}
	if (!doc.HasMember("Objects"))
	{
		g_pConsole->cError("Config reading error. File " + fullFileName + " is not correct.");
		return false;
	}

	Value& objects = doc["Objects"];
	if (!objects.IsArray())
	{
		g_pConsole->cError("Config reading error. File " + fullFileName + " is not correct.");
		return false;
	}
	if (objects.Size() == 0)
	{
		g_pConsole->cWarning("Config reading warning. File " + fullFileName + " has zero elements.");
		return false;
	}

	for (int index = 0; index != objects.Size(); index++)
	{
		m_GetValue(objects[index], "IsVisible", pConfig->object.isVisible);
		m_GetValue(objects[index], "FriendlyName", pConfig->object.friendlyName);
		m_GetValue(objects[index], "FileName", pConfig->object.fileName);
		m_GetValue(objects[index], "FilePath", pConfig->object.filePath);
		m_GetValue(objects[index], "Position", pConfig->object.position);
		m_GetValue(objects[index], "Rotation", pConfig->object.rotation);
		m_GetValue(objects[index], "Scale", pConfig->object.scale);
		m_GetValue(objects[index], "Color", pConfig->object.color);
		m_GetValue(objects[index], "HpXpSpeed", pConfig->object.hpXpSpeed);
		m_GetValue(objects[index], "HasLight_HasColor", pConfig->object.hasLightColor);
		m_GetValue(objects[index], "IsTwoSided_IsWireframe", pConfig->object.isSidedWireframe);

		pConfig->vObjects.push_back(pConfig->object);
	}

	fclose(jsonFile);

	g_pConsole->cSuccess("The objects configuration was successfully read from an external file");
	return true;
}

bool cJsonReaderWriter::m_ReadTextureConfig()
{
	return true;
}

bool cJsonReaderWriter::m_ReadAnimationConfig()
{
	return true;
}

bool cJsonReaderWriter::m_ReadAudioConfig()
{
	return true;
}


bool cJsonReaderWriter::m_GetValue(Value& valueIn, int index, int& valueOut)
{
	if (valueIn.Size() < index)
	{
		return false;
	}

	Value& tempValue = valueIn[index];
	if (tempValue.IsInt())
	{
		valueOut = tempValue.GetInt();
		return true;
	}
	if (tempValue.IsFloat())
	{
		valueOut = static_cast<int>(tempValue.GetFloat());
		return true;
	}
	if (tempValue.IsDouble())
	{
		valueOut = static_cast<int>(tempValue.GetDouble());
		return true;
	}

	return false;
}
												   
bool cJsonReaderWriter::m_GetValue(Value& valueIn, int index, bool& valueOut)
{
	if (valueIn.Size() < index)
	{
		return false;
	}

	Value& tempValue = valueIn[index];
	if (tempValue.IsBool())
	{
		valueOut = tempValue.GetBool();
		return true;
	}

	int tempInt = -1;
	if (tempValue.IsInt())
	{
		tempInt = tempValue.GetInt();
	}
	if (tempValue.IsFloat())
	{
		tempInt = static_cast<int>(tempValue.GetFloat());
	}
	if (tempValue.IsDouble())
	{
		tempInt = static_cast<int>(tempValue.GetDouble());
	}

	if (tempInt == 0)
	{
		valueOut = false;
		return true;
	}
	if (tempInt == 1)
	{
		valueOut = true;
		return true;
	}

	return false;
}
												
bool cJsonReaderWriter::m_GetValue(Value& valueIn, int index, float& valueOut)
{
	if (valueIn.Size() < index)
	{
		return false;
	}

	Value& tempValue = valueIn[index];
	if (tempValue.IsFloat())
	{
		valueOut = tempValue.GetFloat();
		return true;
	}
	if (tempValue.IsInt())
	{
		valueOut = static_cast<float>(tempValue.GetInt());
		return true;
	}
	if (tempValue.IsDouble())
	{
		valueOut = static_cast<float>(tempValue.GetDouble());
		return true;
	}

	return false;
}
												
bool cJsonReaderWriter::m_GetValue(Value& valueIn, int index, double& valueOut)
{
	if (valueIn.Size() < index)
	{
		return false;
	}

	Value& tempValue = valueIn[index];
	if (tempValue.IsDouble())
	{
		valueOut = tempValue.GetDouble();
		return true;
	}
	if (tempValue.IsFloat())
	{
		valueOut = static_cast<double>(tempValue.GetFloat());
		return true;
	}
	if (tempValue.IsInt())
	{
		valueOut = static_cast<double>(tempValue.GetInt());
		return true;
	}

	return false;
}

bool cJsonReaderWriter::m_GetValue(Value& valueIn, int index, std::string& valueOut)
{
	if (valueIn.Size() < index)
	{
		return false;
	}

	Value& tempValue = valueIn[index];
	if (tempValue.IsString())
	{
		valueOut = tempValue.GetString();
		return true;
	}
	if (tempValue.IsInt())
	{
		valueOut = std::to_string(tempValue.GetInt());
		return true;
	}
	if (tempValue.IsFloat())
	{
		valueOut = std::to_string(tempValue.GetFloat());
		return true;
	}
	if (tempValue.IsDouble())
	{
		valueOut = std::to_string(tempValue.GetDouble());
		return true;
	}
	if (tempValue.IsBool())
	{
		valueOut = std::to_string(tempValue.GetBool());
		return true;
	}

	return false;
}


bool cJsonReaderWriter::m_GetValue(Value& valueIn, const char* paramName, int& valueOut)
{
	if (!valueIn.HasMember(paramName))
	{
		std::string tempString = paramName;
		g_pConsole->cWarning("Config reading warning. Key [" + tempString + "] missing. The default setting will be set.");
		return false;
	}

	Value& tempValue = valueIn[paramName];
	if (tempValue.IsInt())
	{
		valueOut = tempValue.GetInt();
		return true;
	}
	if (tempValue.IsFloat())
	{
		valueOut = static_cast<int>(tempValue.GetFloat());
		return true;
	}
	if (tempValue.IsDouble())
	{
		valueOut = static_cast<int>(tempValue.GetDouble());
		return true;
	}

	std::string tempString = paramName;
	g_pConsole->cWarning("Config reading warning. Key [" + tempString + "] does not have the required value. " + 
						"The default setting will be set.");
	return false;
}

bool cJsonReaderWriter::m_GetValue(Value& valueIn, const char* paramName, bool& valueOut)
{
	if (!valueIn.HasMember(paramName))
	{
		std::string tempString = paramName;
		g_pConsole->cWarning("Config reading warning. Key [" + tempString + "] missing. The default setting will be set.");
		return false;
	}

	Value& tempValue = valueIn[paramName];
	if (tempValue.IsBool())
	{
		valueOut = tempValue.GetBool();
		return true;
	}

	int tempInt = -1;
	if (tempValue.IsInt())
	{
		tempInt = tempValue.GetInt();
	}
	if (tempValue.IsFloat())
	{
		tempInt = static_cast<int>(tempValue.GetFloat());
	}
	if (tempValue.IsDouble())
	{
		tempInt = static_cast<int>(tempValue.GetDouble());
	}

	if (tempInt == 0)
	{
		valueOut = false;
		return true;
	}
	if (tempInt == 1)
	{
		valueOut = true;
		return true;
	}

	std::string tempString = paramName;
	g_pConsole->cWarning("Config reading warning. Key [" + tempString + "] does not have the required value. " + 
						"The default setting will be set.");
	return false;
}	

bool cJsonReaderWriter::m_GetValue(Value& valueIn, const char* paramName, float& valueOut)
{
	if (!valueIn.HasMember(paramName))
	{
		std::string tempString = paramName;
		g_pConsole->cWarning("Config reading warning. Key [" + tempString + "] missing. The default setting will be set.");
		return false;
	}

	Value& tempValue = valueIn[paramName];
	if (tempValue.IsFloat())
	{
		valueOut = tempValue.GetFloat();
		return true;
	}
	if (tempValue.IsInt())
	{
		valueOut = static_cast<float>(tempValue.GetInt());
		return true;
	}
	if (tempValue.IsDouble())
	{
		valueOut = static_cast<float>(tempValue.GetDouble());
		return true;
	}

	std::string tempString = paramName;
	g_pConsole->cWarning("Config reading warning. Key [" + tempString + "] does not have the required value. " + 
						"The default setting will be set.");
	return false;
}	

bool cJsonReaderWriter::m_GetValue(Value& valueIn, const char* paramName, double& valueOut)
{
	if (!valueIn.HasMember(paramName))
	{
		std::string tempString = paramName;
		g_pConsole->cWarning("Config reading warning. Key [" + tempString + "] missing. The default setting will be set.");
		return false;
	}

	Value& tempValue = valueIn[paramName];
	if (tempValue.IsDouble())
	{
		valueOut = tempValue.GetDouble();
		return true;
	}
	if (tempValue.IsFloat())
	{
		valueOut = static_cast<double>(tempValue.GetFloat());
		return true;
	}
	if (tempValue.IsInt())
	{
		valueOut = static_cast<double>(tempValue.GetInt());
		return true;
	}

	std::string tempString = paramName;
	g_pConsole->cWarning("Config reading warning. Key [" + tempString + "] does not have the required value. " + 
						"The default setting will be set.");
	return false;
}	

bool cJsonReaderWriter::m_GetValue(Value& valueIn, const char* paramName, glm::vec2& valueOut)
{
	if (!valueIn.HasMember(paramName))
	{
		std::string tempString = paramName;
		g_pConsole->cWarning("Config reading warning. Key [" + tempString + "] missing. The default setting will be set.");
		return false;
	}

	Value& tempValue = valueIn[paramName];
	if (tempValue.IsArray() && (tempValue.Size() == 2))
	{
		m_GetValue(tempValue, 0, valueOut.x);
		m_GetValue(tempValue, 1, valueOut.y);
		return true;
	}
	if (tempValue.IsObject())
	{
		if (!m_GetValue(tempValue, "x", valueOut.x))
		{
			if (!m_GetValue(tempValue, "r", valueOut.r))
			{
				m_GetValue(tempValue, "s", valueOut.s);
			}
		}
		if (!m_GetValue(tempValue, "y", valueOut.y))
		{
			if (!m_GetValue(tempValue, "g", valueOut.g))
			{
				m_GetValue(tempValue, "t", valueOut.t);
			}
		}
		return true;
	}

	std::string tempString = paramName;
	g_pConsole->cWarning("Config reading warning. Key [" + tempString + "] does not have the required value. " + 
						"The default setting will be set.");
	return false;
}

bool cJsonReaderWriter::m_GetValue(Value& valueIn, const char* paramName, glm::vec3& valueOut)
{
	if (!valueIn.HasMember(paramName))
	{
		std::string tempString = paramName;
		g_pConsole->cWarning("Config reading warning. Key [" + tempString + "] missing. The default setting will be set.");
		return false;
	}

	Value& tempValue = valueIn[paramName];
	if (tempValue.IsArray() && (tempValue.Size() >= 3))
	{
		m_GetValue(tempValue, 0, valueOut.x);
		m_GetValue(tempValue, 1, valueOut.y);
		m_GetValue(tempValue, 2, valueOut.z);
		return true;
	}
	if (tempValue.IsObject())
	{
		if (!m_GetValue(tempValue, "x", valueOut.x))
		{
			if (!m_GetValue(tempValue, "r", valueOut.r))
			{
				m_GetValue(tempValue, "s", valueOut.s);
			}
		}
		if (!m_GetValue(tempValue, "y", valueOut.y))
		{
			if (!m_GetValue(tempValue, "g", valueOut.g))
			{
				m_GetValue(tempValue, "t", valueOut.t);
			}
		}
		if (!m_GetValue(tempValue, "z", valueOut.z))
		{
			if (!m_GetValue(tempValue, "b", valueOut.b))
			{
				m_GetValue(tempValue, "p", valueOut.p);
			}
		}
		return true;
	}

	std::string tempString = paramName;
	g_pConsole->cWarning("Config reading warning. Key [" + tempString + "] does not have the required value. " +
						"The default setting will be set.");
	return false;
}

bool cJsonReaderWriter::m_GetValue(Value& valueIn, const char* paramName, glm::vec4& valueOut)
{
	if (!valueIn.HasMember(paramName))
	{
		std::string tempString = paramName;
		g_pConsole->cWarning("Config reading warning. Key [" + tempString + "] missing. The default setting will be set.");
		return false;
	}

	Value& tempValue = valueIn[paramName];
	if (tempValue.IsArray() && (tempValue.Size() >= 3))
	{
		m_GetValue(tempValue, 0, valueOut.x);
		m_GetValue(tempValue, 1, valueOut.y);
		m_GetValue(tempValue, 2, valueOut.z);
		m_GetValue(tempValue, 3, valueOut.w);
		return true;
	}
	if (tempValue.IsObject())
	{
		if (!m_GetValue(tempValue, "x", valueOut.x))
		{
			if (!m_GetValue(tempValue, "r", valueOut.r))
			{
				m_GetValue(tempValue, "s", valueOut.s);
			}
		}
		if (!m_GetValue(tempValue, "y", valueOut.y))
		{
			if (!m_GetValue(tempValue, "g", valueOut.g))
			{
				m_GetValue(tempValue, "t", valueOut.t);
			}
		}
		if (!m_GetValue(tempValue, "z", valueOut.z))
		{
			if (!m_GetValue(tempValue, "b", valueOut.b))
			{
				m_GetValue(tempValue, "p", valueOut.p);
			}
		}
		if (!m_GetValue(tempValue, "w", valueOut.w))
		{
			if (!m_GetValue(tempValue, "a", valueOut.a))
			{
				m_GetValue(tempValue, "q", valueOut.q);
			}
		}
		return true;
	}

	std::string tempString = paramName;
	g_pConsole->cWarning("Config reading warning. Key [" + tempString + "] does not have the required value. " +
						"The default setting will be set.");
	return false;
}

bool cJsonReaderWriter::m_GetValue(Value& valueIn, const char* paramName, std::string& valueOut)
{
	if (!valueIn.HasMember(paramName))
	{
		std::string tempString = paramName;
		g_pConsole->cWarning("Config reading warning. Key [" + tempString + "] missing. The default setting will be set.");
		return false;
	}

	Value& tempValue = valueIn[paramName];
	if (tempValue.IsString())
	{
		valueOut = tempValue.GetString();
		return true;
	}
	if (tempValue.IsInt())
	{
		valueOut = std::to_string(tempValue.GetInt());
		return true;
	}
	if (tempValue.IsFloat())
	{
		valueOut = std::to_string(tempValue.GetFloat());
		return true;
	}
	if (tempValue.IsDouble())
	{
		valueOut = std::to_string(tempValue.GetDouble());
		return true;
	}
	if (tempValue.IsBool())
	{
		valueOut = std::to_string(tempValue.GetBool());
		return true;
	}

	std::string tempString = paramName;
	g_pConsole->cWarning("Config reading warning. Key [" + tempString + "] does not have the required value. " +
						"The default setting will be set.");
	return false;
}


bool cJsonReaderWriter::m_SetValue(Value& valueIn, const char* paramName, const std::string& stringIn)
{
	if (!valueIn.HasMember(paramName))
	{
		return false;
	}

	Value& tempValue = valueIn[paramName];
	if (!tempValue.IsString())
	{
		return false;
	}
	const char* cstr = stringIn.c_str();
	valueIn.SetString(StringRef(cstr));
	return true;
}

bool cJsonReaderWriter::m_SetValue(Value& valueIn, const char* paramName, const int& numberIn)
{
	if (!valueIn.IsInt())
	{
		return false;
	}
	valueIn.SetInt(numberIn);
	return true;
}

//bool cJsonReaderWriter::ParseConfigDef(std::string& configJSONfile, sConfigDef& valueOut)
//{
//	// Load Config File
//	FILE* jsonFile = 0;
//	fopen_s(&jsonFile, configJSONfile.c_str(), "rb");
//
//	Document doc;
//	char readBuffer[65536];
//	FileReadStream is(jsonFile, readBuffer, sizeof(readBuffer));
//	doc.ParseStream(is);
//
//	if (!doc.IsObject())
//	{
//		return false;
//	}
//
//	// Get Preference File
//	if (!GetValue(doc["PreferenceFile"], valueOut.preferenceFile))
//	{
//		return false;
//	}
//
//	if (!GetValue(doc["LanguageFiles"], valueOut.languageFiles))
//	{
//		return false;
//	}
//
//	fclose(jsonFile);
//
//	return true;
//}
//
//bool cJsonReaderWriter::ParsePreferencesDef(std::string& userPreJSONfile, sPreferencesDef& valueOut)
//{
//	// Load UserPrefs File
//	FILE* jsonFile = 0;
//	fopen_s(&jsonFile, userPreJSONfile.c_str(), "rb");
//
//	Document doc;
//	char readBuffer[65536];
//	FileReadStream is(jsonFile, readBuffer, sizeof(readBuffer));
//	doc.ParseStream(is);
//
//	if (!doc.IsObject())
//	{
//		return false;
//	}
//
//	// Get Preference File
//
//	if (!GetValue(doc["DefaultLang"], valueOut.defaultLanguage))
//	{
//		return false;
//	}
//
//	if (!GetValue(doc["DefaultName"], valueOut.defaultName))
//	{
//		return false;
//	}
//
//	if (!GetValue(doc["DefaultAge"], valueOut.defaultAge))
//	{
//		return false;
//	}
//
//	if (!GetValue(doc["DefaultAnimal"], valueOut.defaultAnimal))
//	{
//		return false;
//	}
//
//	if (!GetValue(doc["PrefLanguage"], valueOut.prefLanguage))
//	{
//		return false;
//	}
//	if (valueOut.prefLanguage == "")
//	{
//		valueOut.prefLanguage = valueOut.defaultLanguage;
//	}
//
//	if (!GetValue(doc["PrefName"], valueOut.prefName))
//	{
//		return false;
//	}
//	if (valueOut.prefName == "")
//	{
//		valueOut.prefName = valueOut.defaultName;
//	}
//
//	if (!GetValue(doc["PrefAge"], valueOut.prefAge))
//	{
//		return false;
//	}
//	if (std::to_string(valueOut.prefAge) == "")
//	{
//		valueOut.prefAge = valueOut.defaultAge;
//	}
//
//	if (!GetValue(doc["PrefAnimal"], valueOut.prefAnimal))
//	{
//		return false;
//	}
//	if (valueOut.prefAnimal == "")
//	{
//		valueOut.prefAnimal = valueOut.defaultAnimal;
//	}
//
//	if (!GetValue(doc["MinAge"], valueOut.minAge))
//	{
//		return false;
//	}
//
//	if (!GetValue(doc["MaxAge"], valueOut.maxAge))
//	{
//		return false;
//	}
//
//	if (!GetValue(doc["MinName"], valueOut.minName))
//	{
//		return false;
//	}
//
//	if (!GetValue(doc["MaxName"], valueOut.maxName))
//	{
//		return false;
//	}
//
//	fclose(jsonFile);
//
//	return true;
//}
//
//bool cJsonReaderWriter::ParseLanguageDef(std::string& languageJSONfile, sLanguageDef& valueOut)
//{
//	//// Load Language File
//	//FILE* jsonFile = 0;
//	//fopen_s(&jsonFile, languageJSONfile.c_str(), "rb");
//
//	//Document doc;
//	//char readBuffer[65536];
//	//FileReadStream is(jsonFile, readBuffer, sizeof(readBuffer));
//	//doc.ParseStream(is);
//
//	//if (!doc.IsObject())
//	//{
//	//	return false;
//	//}
//
//	//if (!GetValue(doc["Language"], valueOut.languageName))
//	//{
//	//	return false;
//	//}
//	//;
//	//for (Value::ConstMemberIterator iter = doc.MemberBegin(); iter != doc.MemberEnd(); ++iter)
//	//{
//	//	std::string name = iter->name.GetString();
//	//	std::string value = iter->value.GetString();
//	//	valueOut.keyValues[name] = value;
//	//	bool isTemp = true;
//	//}
//
//	//fclose(jsonFile);
//
//	//return true;
//}