#include "cCommands_Rotate.h"

extern cObject3D* g_pFindMeshByFriendlyName(std::string friendlyNameToFind);

cCommands_Rotate::cCommands_Rotate()
{
	this->m_endXYZ = glm::vec3(0.0f);
	this->m_startXYZ = glm::vec3(0.0f);
	this->m_velocity = glm::vec3(0.0f);

	this->m_elapsedTime = 0.0f;
	this->m_timeToOrient = 0.5f;

	this->m_prepared = false;
	this->m_infinity = false;

	this->m_pObject = nullptr;
}

cCommands_Rotate::~cCommands_Rotate() {}

bool cCommands_Rotate::Stop()
{
	m_elapsedTime = 0.0f;
	m_prepared = false;
	return false;
}

void cCommands_Rotate::PrepareUpdate()
{
	m_velocity = m_endXYZ / m_timeToOrient;

	m_current = m_startOrient;

	m_prepared = true;
}

bool cCommands_Rotate::Update(float deltaTime)
{
	if (m_infinity)
	{
		m_elapsedTime = 0.0f;
	}

	if (m_elapsedTime >= m_timeToOrient)
	{
		m_elapsedTime = 0;
		m_prepared = false;
		return true;
	}

	if (!m_prepared)
	{
		this->PrepareUpdate();
	}

	m_elapsedTime += deltaTime;

	m_current = m_pObject->GetQuatOrientation();

	glm::quat change = glm::quat(m_velocity * (float)deltaTime);

	m_current *= change;

	if (std::abs(m_current.x) > std::abs(m_endOrient.x * 40) && 
		std::abs(m_current.y) > std::abs(m_endOrient.y * 40) && 
		std::abs(m_current.z) > std::abs(m_endOrient.z * 40))
	{
		return false;
	}

	m_pObject->SetQuatOrientation(m_current);

	if (m_pObject->friendlyName == "Camera")
	{
		glm::vec3 forward = m_current * glm::vec3(0.0f, 0.0f, -1.0f);
		m_pObject->SetScale(forward);
	}

	return false;
}

std::string cCommands_Rotate::GetCommandName()
{
	return this->m_commandName;
}

void cCommands_Rotate::SetCommandName(std::string commandName)
{
	this->m_commandName = commandName;
}

void cCommands_Rotate::Initialize(std::string objectName, glm::vec3 angleXYZ, float timeToOrient, bool infinity)
{
	m_pObject = g_pFindMeshByFriendlyName(objectName);

	m_startOrient = m_pObject->GetQuatOrientation();

	m_endXYZ = glm::radians(angleXYZ);

	m_endOrient = glm::quat(m_endXYZ);

	if (timeToOrient > 0.0f)
	{
		m_timeToOrient = timeToOrient;
	}

	m_infinity = infinity;
}