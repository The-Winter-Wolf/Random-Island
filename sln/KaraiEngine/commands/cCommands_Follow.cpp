#include "cCommands_Follow.h"

#include "../utils/cQuaternionUtils.h"

extern cQuaternionUtils* g_pQuatUtils;
extern cObject3D* g_pFindMeshByFriendlyName(std::string friendlyNameToFind);

cCommands_Follow::cCommands_Follow()
{
	this->m_endXYZ = glm::vec3(0.0f);
	this->m_startXYZ = glm::vec3(0.0f);
	this->m_velocity = glm::vec3(0.0f);
	this->m_direction = glm::vec3(0.0f);

	this->m_endOrient = glm::quat();
	this->m_startOrient = glm::quat();
	this->m_offset = glm::vec3(0.0f);

	this->m_maxSpeed = 10.0f;
	this->m_timeToMove = 0.5f;
	this->m_followDist = 5.0f;

	this->m_speed = 0.0f;
	this->m_curdist = 0.0f;
	this->m_distance = 0.0f;

	this->m_elapsedTime = 0.0f;

	this->m_acceleration = 0.0f;

	this->m_easeIn = false;
	this->m_easeOut = false;

	this->m_infinity = true;
	this->m_prepared = false;

	this->m_pObject = nullptr;
	this->m_pSubject = nullptr;
}

cCommands_Follow::~cCommands_Follow() {}

bool cCommands_Follow::Stop()
{
	m_elapsedTime = 0.0f;
	m_prepared = false;
	return true;
}

bool cCommands_Follow::Update(float deltaTime)
{
	glm::vec3 currentStart = m_pObject->GetPosition();
	
	glm::vec3 currentEnd = m_pSubject->GetPosition();
	
	glm::quat currentOrient;

	glm::vec3 direction = glm::normalize(currentEnd - currentStart);

	m_curdist = glm::distance(currentStart, currentEnd + this->m_offset) - m_followDist;

	m_startOrient = m_pObject->GetQuatOrientation();
	m_endOrient = g_pQuatUtils->LookAt(direction);
	currentOrient = g_pQuatUtils->RotateTowards(m_startOrient, m_endOrient, 0.5f * deltaTime);	
	m_pObject->SetQuatOrientation(currentOrient);

	if (m_pObject->friendlyName == "Camera")
	{
		m_pObject->SetScale(currentEnd);
	}
	if (m_curdist < m_followDist)
	{
		return false;
	}
	if (!m_prepared)
	{
		m_startXYZ = currentStart;
		m_endXYZ = currentEnd + this->m_offset;

		m_direction = glm::normalize(m_endXYZ - m_startXYZ);

		m_distance = m_curdist;

		m_acceleration = GetAcceleration(m_distance);

		m_prepared = true;
	}
	if (m_endXYZ != currentEnd)
	{
		m_prepared = false;
	}

	this->EaseInOut(deltaTime);

	currentStart += m_velocity * (float)deltaTime;

	m_pObject->SetPosition(currentStart);

	m_elapsedTime += deltaTime;

	//std::cout << "time: " << m_elapsedTime << " // distance: " << m_distance
	//		  << " // speed: " << m_speed << " // accel: " << m_acceleration << std::endl;

	if (m_infinity)
	{
		m_elapsedTime = 0.0f;
	}
	if (m_elapsedTime >= m_timeToMove)
	{
		m_elapsedTime = 0.0f;
		m_prepared = false;
		return true;
	}

	return false;
}

std::string cCommands_Follow::GetCommandName()
{
	return this->m_commandName;
}

void cCommands_Follow::SetCommandName(std::string commandName)
{
	this->m_commandName = commandName;
}

void cCommands_Follow::EaseInOut(float deltaTime)
{
	if (m_curdist > (m_distance * 0.87))
	{	
		m_speed += m_acceleration * deltaTime;
	}
	if (m_curdist < (m_distance * 0.17))
	{
		m_speed -= m_acceleration * deltaTime;
	}

	if (m_speed > m_maxSpeed)
	{
		m_speed = m_maxSpeed;
	}
	if (m_speed < 0.0f)
	{
		m_speed = 0.0f;
	}
	m_velocity = m_direction * m_speed;
}

float cCommands_Follow::GetAcceleration(float distance)
{
	// EaseInOut
	float dist = distance;
	float accel = 0.0f;

	float tm = distance / m_maxSpeed;

	float dist15 = dist * 0.15f;
	float time15 = tm * 0.15f;

	accel = dist15 / (time15 * time15);

	return accel;
}

void cCommands_Follow::Initialize(std::string objectName, std::string subjectName,
								  float distance, float speed, glm::vec3 offset, bool infinity)
{
	m_pObject = g_pFindMeshByFriendlyName(objectName);
	m_pSubject = g_pFindMeshByFriendlyName(subjectName);

	this->m_velocity = glm::vec3(0.0f);

	this->m_elapsedTime = 0.0f;

	this->m_infinity = infinity;

	this->m_prepared = false;

	m_offset = offset;

	if (distance >= 0.0f)
	{
		m_followDist = distance;
	}
	if (speed > 0.0f)
	{
		m_maxSpeed = speed;
	}

	if (!m_easeIn || !m_easeOut)
	{
		m_speed = m_maxSpeed;
	}
}
