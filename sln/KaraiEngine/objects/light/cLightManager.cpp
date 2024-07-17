#include "cLightManager.h"

#include "../../global/global.h"
#include <glm/gtc/type_ptr.hpp>

#include "../../graphics/shaders/cShaderManager.h"
extern cShaderManager* g_pShaderManager;

const unsigned int cLightManager::NUMBER_OF_LIGHTS = 20;

cLightManager* cLightManager::m_pLightManager = NULL;

cLightManager* cLightManager::GetLightManager()
{
	if (cLightManager::m_pLightManager == NULL)
	{
		cLightManager::m_pLightManager = new cLightManager();
	}
	return cLightManager::m_pLightManager;
}

cLightManager::cLightManager() 
{
	this->m_vLights.resize(NUMBER_OF_LIGHTS);
}

cLightManager::~cLightManager(){}

cLight::cLight()
{
	this->position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	this->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);	// White light
	this->specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);	// White light

	// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	this->atten = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	// Spot, directional lights (Default is stright down)
	this->direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);	
	// x = lightType (0 - point, 1 - spot, 2 - directional)
	// y = inner angle, z = outer angle, w = TBD
	this->param1 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	// x = 0 for off, 1 for on
	this->param2 = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

	return;
}

int cLightManager::GetNumberOfLight()
{
	return this->NUMBER_OF_LIGHTS;
}

void cLightManager::Init()
{
	//this->m_HandLoadLights();

	this->m_InitLightNUB();

	//this->m_InitLightUniform();
}

void cLightManager::Update()
{
	this->m_UpdateLightNUB();

	//this->m_UpdateLightUniform();
}

cLight* cLightManager::GetLight(unsigned int indexOfLight)
{
	return &(this->m_vLights[indexOfLight]);
}

void cLightManager::TurnOnLight(unsigned int indexOfLight)
{
	this->m_vLights[indexOfLight].param2.x = 1.0f;
	return;
}

void cLightManager::TurnOffLight(unsigned int indexOfLight)
{
	this->m_vLights[indexOfLight].param2.x = 0.0f;
	return;
}

//////////////////////////////////////////////////////////////

void cLightManager::m_HandLoadLights()
{
	// Example
	//theLights[0].param2.x = 1.0f;		// Turn on
	//theLights[0].param1.x = 0.0f;		// Point light

	//theLights[0].position.x = 0.0f;
	//theLights[0].position.y = 25.0f;
	//theLights[0].position.z = 0.0f;

	//theLights[0].atten.x = 0.0f;        // Constant attenuation
	//theLights[0].atten.y = 0.01f;       // Linear attenuation
	//theLights[0].atten.z = 0.01f;       // Quadratic attenuation

	//theLights[0].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
	//theLights[0].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//theLights[0].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	//theLights[0].param1.y = 20.0f;		// Inner angle
	//theLights[0].param1.z = 60.0f;		// Outer angle

	//theLights[1].param2.x = 0.0f;		// Turn on
	//theLights[1].param1.x = 0.0f;		// Directional light
	//theLights[1].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
	//theLights[1].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//theLights[1].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

bool cLightManager::m_InitLightNUB()
{
	this->blockName = "LightDataNUB";

	unsigned int sizeOfLightBuffer = sizeof(cLight) * NUMBER_OF_LIGHTS;

	if (g_pShaderManager->InitUniformBlock(blockName, sizeOfLightBuffer))
	{
		return true;
	}

	return false;
}

bool cLightManager::m_InitLightUniform()
{
	// Auto init in ShaderManager
	return true;
}

void cLightManager::m_UpdateLightNUB()
{
	void* lightData = static_cast<void*>(&m_vLights[0]);

	g_pShaderManager->UpdateUniformBlock(blockName, lightData);

	return;
}

void cLightManager::m_UpdateLightUniform()
{
	for (unsigned int index = 0; index < cLightManager::NUMBER_OF_LIGHTS; index++)
	{
		std::string i = std::to_string(index);	
		g_pShaderManager->SetUniform(g_ShaderID, "theLights["+i+"].position",  m_vLights[index].position);
		g_pShaderManager->SetUniform(g_ShaderID, "theLights["+i+"].diffuse",	 m_vLights[index].diffuse);
		g_pShaderManager->SetUniform(g_ShaderID, "theLights["+i+"].specular",  m_vLights[index].specular);
		g_pShaderManager->SetUniform(g_ShaderID, "theLights["+i+"].atten",	 m_vLights[index].atten);
		g_pShaderManager->SetUniform(g_ShaderID, "theLights["+i+"].direction", m_vLights[index].direction);
		g_pShaderManager->SetUniform(g_ShaderID, "theLights["+i+"].param1",	 m_vLights[index].param1);
		g_pShaderManager->SetUniform(g_ShaderID, "theLights["+i+"].param2",	 m_vLights[index].param2);
	}

	return;
}

