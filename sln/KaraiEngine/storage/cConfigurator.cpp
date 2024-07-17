#include "cConfigurator.h"

#include "../global/global.h"

#include "../engine/cLoadManager.h"
extern cLoadManager* g_pLoadManager;

#include "../objects/camera/cCamera.h"
extern cCamera* g_pCamera;

#include "../objects/light/cLightManager.h"
extern cLightManager* g_pLightManager;

#include "../graphics/models/cModelManager.h"
extern cModelManager* g_pModelManager;

#include <iostream>

cConfigurator::cConfigurator()
{
	m_pConfig = new cConfigData();

	m_pConfig->xmlPath = "../../extern/assets/configs/xml";
	m_pConfig->jsonPath = "../../extern/assets/configs/json";

	m_pConfig->cameraFileName = "camera";
	m_pConfig->lightsFileName = "lights";
	m_pConfig->audiosFileName = "audios";
	m_pConfig->objectsFileName = "objects";
	m_pConfig->texturesFileName = "textures";
	m_pConfig->animationsFileName = "animations";

	m_pXmlRW = new cXmlReaderWriter(m_pConfig);

	m_pJsonRW = new cJsonReaderWriter(m_pConfig);
}

cConfigurator::~cConfigurator()
{
	delete m_pConfig;
	delete m_pXmlRW;
	delete m_pJsonRW; 
}

void cConfigurator::ReadConfigFromFile()
{
	m_pConfig->vLights.clear();
	m_pConfig->vObjects.clear();

#if XML_CONFIG
	m_pXmlRW->ReadConfiguration();
#endif

#if JSON_CONFIG
	m_pJsonRW->ReadConfiguration();
#endif

	this->m_CameraFromConfig();
	this->m_LightFromConfig();
	this->m_ObjectFromConfig();

	g_pConsole->cSuccess("Game configuration loaded");
}

void cConfigurator::WriteConfigToFile()
{
	this->m_LightToConfig();
	this->m_CameraToConfig();
	this->m_ObjectToConfig();

#if XML_CONFIG
	m_pXmlRW->WriteConfiguration(m_XmlPath + "camera.xml");
	m_pXmlRW->WriteConfiguration(m_XmlPath + "lights.xml");
	m_pXmlRW->WriteConfiguration(m_XmlPath + "models.xml");
#endif

#if JSON_CONFIG
	m_pJsonRW->WriteConfiguration();
#endif

	g_pConsole->cSuccess("Game configuration saved");
}

void cConfigurator::SetConfigPath(std::string configPath)
{
	this->m_JsonPath += "/json/";
	this->m_XmlPath += "/xml/";
}

void cConfigurator::m_CameraFromConfig() 
{
	g_pCamera->SetWindowSize(m_pConfig->camera.width, m_pConfig->camera.height);
	g_pCamera->SetCameraMod(m_pConfig->camera.mod);
	g_pCamera->SetPosition(m_pConfig->camera.position);
	g_pCamera->SetDirection(m_pConfig->camera.direction);
	g_pCamera->SetFOVMinMax(m_pConfig->camera.fovMinMax);
	g_pCamera->SetYawMinMax(m_pConfig->camera.yawMinMax);
	g_pCamera->SetPitchMinMax(m_pConfig->camera.pitchMinMax);
	g_pCamera->SetPitchYawSpeed(m_pConfig->camera.pitchYawSpeed);
	g_pCamera->SetDistMinMax(m_pConfig->camera.distMinMax);
	g_pCamera->SetMoveSpeed(m_pConfig->camera.moveSpeed);
	g_pCamera->SetSpeedScale(m_pConfig->camera.speedScale);
}
void cConfigurator::m_CameraToConfig()
{
	g_pCamera->GetWindowSize(m_pConfig->camera.width, m_pConfig->camera.height);
	m_pConfig->camera.mod = g_pCamera->GetCameraMod();
	m_pConfig->camera.position = g_pCamera->GetPosition();
	m_pConfig->camera.direction = g_pCamera->GetDirection();
	m_pConfig->camera.fovMinMax = g_pCamera->GetFOVMinMax();
	m_pConfig->camera.yawMinMax = g_pCamera->GetYawMinMax();
	m_pConfig->camera.pitchMinMax = g_pCamera->GetPitchMinMax();
	m_pConfig->camera.pitchYawSpeed = g_pCamera->GetPitchYawSpeed();
	m_pConfig->camera.distMinMax = g_pCamera->GetDistMinMax();
	m_pConfig->camera.moveSpeed = g_pCamera->GetMoveSpeed();
	m_pConfig->camera.speedScale = g_pCamera->GetSpeedScale();
}

void cConfigurator::m_LightFromConfig()
{
	m_pConfig->vLights.resize(g_pLightManager->GetNumberOfLight());

	for (int i = 0; i != g_pLightManager->GetNumberOfLight(); i++)
	{
		cLight* curLight = g_pLightManager->GetLight(i);

		curLight->param1 = m_pConfig->vLights[i].prm1;
		curLight->param2 = m_pConfig->vLights[i].prm2;
		curLight->atten = m_pConfig->vLights[i].atten;
		curLight->diffuse = m_pConfig->vLights[i].diff;
		curLight->position = m_pConfig->vLights[i].pos;
		curLight->direction = m_pConfig->vLights[i].dir;
		curLight->specular = m_pConfig->vLights[i].spec;
	}
}
void cConfigurator::m_LightToConfig()
{
	m_pConfig->vLights.clear();
	for (int i = 0; i != g_pLightManager->GetNumberOfLight(); i++)
	{
		sLight light;
		cLight* curLight = g_pLightManager->GetLight(i);

		light.prm1 = curLight->param1;
		light.prm2 = curLight->param2;
		light.atten = curLight->atten;
		light.diff = curLight->diffuse;
		light.pos = curLight->position;
		light.dir = curLight->direction;
		light.spec = curLight->specular;

		m_pConfig->vLights.push_back(light);
	}
}

void cConfigurator::m_ObjectFromConfig()
{
	g_vpObjectToDraw.clear();

	for (int i = 0; i != m_pConfig->vObjects.size(); i++)
	{
		if (!g_pModelManager->CheckNameInMap(m_pConfig->vObjects[i].fileName))
		{	
			g_pModelManager->SetBasePath(m_pConfig->vObjects[i].filePath);
			g_pModelManager->LoadModelIntoVAO(m_pConfig->vObjects[i].fileName, g_ShaderID, 
										      m_pConfig->vObjects[i].flipUVs);
		}

		cObject3D* object = new cObject3D();
		object->BindModelToObject(m_pConfig->vObjects[i].fileName);
		object->friendlyName = m_pConfig->vObjects[i].friendlyName;
		object->SetPosition(m_pConfig->vObjects[i].position);
		object->SetOrientationFromEuler(glm::radians(m_pConfig->vObjects[i].rotation));
		object->SetScale(m_pConfig->vObjects[i].scale);
		object->SetOpacity(m_pConfig->vObjects[i].color.a);
		object->SetColor(m_pConfig->vObjects[i].color);
		object->bHasLight = m_pConfig->vObjects[i].hasLightColor.x;
		object->bHasColor = m_pConfig->vObjects[i].hasLightColor.y;
		object->bIsVisible = m_pConfig->vObjects[i].isVisible;
		object->bIsTwoSided = m_pConfig->vObjects[i].isSidedWireframe.x;
		object->bIsWireframe = m_pConfig->vObjects[i].isSidedWireframe.y;
		//object->SetHP(m_pConfig->vObjects[i].hpXpSpeed.x);
		//object->SetXP(m_pConfig->vObjects[i].hpXpSpeed.y);
		//object->SetSpeed(m_pConfig->vObjects[i].hpXpSpeed.z);

		g_vpObjectToDraw.push_back(object);
	}

	return;
}
void cConfigurator::m_ObjectToConfig()
{
	m_pConfig->vObjects.resize(g_vpObjectToDraw.size());
	for (int i = 0; i != g_vpObjectToDraw.size(); i++)
	{
		m_pConfig->vObjects[i].isVisible = g_vpObjectToDraw[i]->bIsVisible;
		m_pConfig->vObjects[i].fileName = g_vpObjectToDraw[i]->GetModel()->modelName;
		m_pConfig->vObjects[i].filePath = g_vpObjectToDraw[i]->GetModel()->modelPath;
		m_pConfig->vObjects[i].friendlyName = g_vpObjectToDraw[i]->friendlyName;
		m_pConfig->vObjects[i].hasLightColor.x = g_vpObjectToDraw[i]->bHasLight;
		m_pConfig->vObjects[i].hasLightColor.y = g_vpObjectToDraw[i]->bHasColor;
		m_pConfig->vObjects[i].isSidedWireframe.x = g_vpObjectToDraw[i]->bIsTwoSided;
		m_pConfig->vObjects[i].isSidedWireframe.y = g_vpObjectToDraw[i]->bIsWireframe;
		m_pConfig->vObjects[i].rotation = glm::degrees(g_vpObjectToDraw[i]->GetEulerOrientation());
		m_pConfig->vObjects[i].position = g_vpObjectToDraw[i]->GetPosition();
		m_pConfig->vObjects[i].scale = g_vpObjectToDraw[i]->GetScale();
		m_pConfig->vObjects[i].color = g_vpObjectToDraw[i]->GetColor();
		m_pConfig->vObjects[i].color.a = g_vpObjectToDraw[i]->GetOpacity();
		//m_pConfig->vObjects[i].hpXpSpeed.x = g_vpObjectToDraw[i]->GetHP();
		//m_pConfig->vObjects[i].hpXpSpeed.y = g_vpObjectToDraw[i]->GetXP();
		//m_pConfig->vObjects[i].hpXpSpeed.z = g_vpObjectToDraw[i]->GetSpeed();
	}
}


