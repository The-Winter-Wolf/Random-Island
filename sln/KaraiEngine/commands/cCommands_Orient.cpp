#include "cCommands_Orient.h"

#include "../utils/cQuaternionUtils.h"

extern cQuaternionUtils* g_pQuatUtils;
extern cObject3D* g_pFindMeshByFriendlyName(std::string friendlyNameToFind);

cCommands_Orient::cCommands_Orient()
{
	this->m_endXYZ = glm::vec3(0.0f);
	this->m_startXYZ = glm::vec3(0.0f);
	this->m_velocity = glm::vec3(0.0f);

	this->m_distance = 0.0f;
	this->m_angle = 100.0f;
	this->m_elapsedTime = 0.0f;
	this->m_timeToOrient = 0.0f;

	this->m_prepared = false;
	this->m_infinity = false;

	this->m_pObject = nullptr;
	this->m_pSubject = nullptr;
}

cCommands_Orient::~cCommands_Orient() {}

bool cCommands_Orient::Stop()
{
	m_elapsedTime = 0.0f;
	m_prepared = false;
	return false;
}

void cCommands_Orient::PrepareUpdate()
{
	if (m_pObject != nullptr)
	{
		m_startXYZ = m_pObject->GetPosition();
	}
	if (m_pSubject != nullptr)
	{
		m_endXYZ = m_pSubject->GetPosition();
	}

	m_startOrient = m_pObject->GetQuatOrientation();

	glm::vec3 dir = glm::normalize(m_endXYZ - m_startXYZ);

	m_endOrient = g_pQuatUtils->LookAt(dir);

	if (m_timeToOrient <= 0.0f)
	{
		m_timeToOrient = 0.5f;
		return;
	}
	else
	{
		float cos = dot(m_startOrient, m_endOrient);
		m_distance = acos(cos);

		m_angle = m_distance / m_timeToOrient;

		return;
	}
}

bool cCommands_Orient::Update(float deltaTime)
{
	if (!m_prepared)
	{
		this->PrepareUpdate();
		m_prepared = true;
	}

	m_startXYZ = m_pObject->GetPosition();

	if (m_pSubject != nullptr)
	{
		m_endXYZ = m_pSubject->GetPosition();
	}

	glm::vec3 dir = glm::normalize(m_endXYZ - m_startXYZ);

	m_startOrient = m_pObject->GetQuatOrientation();
	m_endOrient = g_pQuatUtils->LookAt(dir);

	m_current = g_pQuatUtils->RotateTowards(m_startOrient, m_endOrient, m_angle * deltaTime / 1.6f);

	m_pObject->SetQuatOrientation(m_current);

	if (m_pObject->friendlyName == "Camera")
	{
		m_pObject->SetScale(m_endXYZ);
	}

	m_elapsedTime += deltaTime;

	if (m_infinity)
	{
		m_elapsedTime = 0.0f;
	}

	if (m_elapsedTime >= m_timeToOrient)
	{
		m_elapsedTime = 0.0f;
		m_prepared = false;
		return true;
	}

	return false;
}

std::string cCommands_Orient::GetCommandName()
{
	return this->m_commandName;
}

void cCommands_Orient::SetCommandName(std::string commandName)
{
	this->m_commandName = commandName;
}

void cCommands_Orient::Initialize(std::string objectName, glm::vec3 endXYZ, float timeToOrient, bool infinity)
{
	m_pObject = g_pFindMeshByFriendlyName(objectName);
	m_endXYZ = endXYZ;

	m_infinity = infinity;

	if (timeToOrient >= 0.0f)
	{
		m_timeToOrient = timeToOrient;
	}
}

void cCommands_Orient::Initialize(std::string objectName, std::string subjectName, float timeToOrient, bool infinity)
{
	m_pObject = g_pFindMeshByFriendlyName(objectName);
	m_pSubject = g_pFindMeshByFriendlyName(subjectName);

	m_infinity = infinity;

	if (timeToOrient >= 0.0f)
	{
		m_timeToOrient = timeToOrient;
	}
}