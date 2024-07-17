#include "cCommands_Trigger.h"

extern cObject3D* g_pFindMeshByFriendlyName(std::string friendlyNameToFind);

cCommands_Trigger::cCommands_Trigger()
{
	this->m_delayTime = 0.0f;
	this->m_distance = 0.0f;
	this->m_time = 0.0f;

	this->m_bIsTriggered = false;

	this->m_objXYZ = glm::vec3(0.0f);
	this->m_sbjXYZ = glm::vec3(0.0f);
}

cCommands_Trigger::~cCommands_Trigger() {}

bool cCommands_Trigger::Stop()
{
	m_bIsTriggered = false;
	m_time = 0.0f;

	return true;
}

bool cCommands_Trigger::Update(float deltaTime)
{
	glm::vec3 startPos = glm::vec3(0.0f);

	if (m_pObject == NULL)
	{
		startPos = m_objXYZ;
	}
	else
	{
		startPos = m_pObject->GetPosition();
	}

	glm::vec3 endPos = m_pSubject->GetPosition();

	float dist = glm::distance(startPos, endPos);

	if (dist < m_distance)
	{
		m_bIsTriggered = true;
	}
 
	if (m_bIsTriggered)
	{
		m_time += deltaTime;
	}

	if (m_time > m_delayTime)
	{
		m_bIsTriggered = false;
		m_time = 0.0f;

		return true;
	}
 
    return false;
}

std::string cCommands_Trigger::GetCommandName()
{
    return this->m_commandName;
}

void cCommands_Trigger::SetCommandName(std::string commandName)
{
	this->m_commandName = commandName;
	return;
}

void cCommands_Trigger::Initialize(std::string triggerObj, std::string triggeredSbj, float distance, float delayTime)
{
	m_pObject = g_pFindMeshByFriendlyName(triggerObj);
	m_pSubject = g_pFindMeshByFriendlyName(triggeredSbj);

	this->m_objXYZ = m_pObject->GetPosition();
	this->m_sbjXYZ = m_pSubject->GetPosition();

	if (m_distance >= 0.0f)
	{
		this->m_distance = distance;
	}
	if (m_delayTime >= 0.0f)
	{
		this->m_delayTime = delayTime;
	}
}

void cCommands_Trigger::Initialize(glm::vec3 triggerXYZ, std::string triggeredSbj, float distance, float delayTime)
{
	m_pSubject = g_pFindMeshByFriendlyName(triggeredSbj);

	this->m_objXYZ = triggerXYZ;
	this->m_sbjXYZ = m_pSubject->GetPosition();

	if (m_distance >= 0.0f)
	{
		this->m_distance = distance;
	}
	if (m_delayTime >= 0.0f)
	{
		this->m_delayTime = delayTime;
	}
}

