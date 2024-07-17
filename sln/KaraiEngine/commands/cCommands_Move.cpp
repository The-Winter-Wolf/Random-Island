#include "cCommands_Move.h"

#include "../utils/cQuaternionUtils.h"

extern cQuaternionUtils* g_pQuatUtils;
extern cObject3D* g_pFindMeshByFriendlyName(std::string friendlyNameToFind);

cCommands_Move::cCommands_Move()
{	
	this->m_endXYZ = glm::vec3(0.0f);
	this->m_startXYZ = glm::vec3(0.0f);
	this->m_velocity = glm::vec3(0.0f);
	this->m_direction = glm::vec3(0.0f);

	this->m_speed = 0.0f;
	this->m_maxSpeed = 10.0f;

	this->m_distance = 0.0f;

	this->m_timeToMove = 0.0f;
	this->m_elapsedTime = 0.0f;

	this->m_acceleration = 0.0f;

	this->m_easeIn = false;
	this->m_easeOut = false;
	this->m_prepared = false;

	this->m_pObject = nullptr;
	this->m_pSubject = nullptr;
}

cCommands_Move::~cCommands_Move() {}

bool cCommands_Move::Stop()
{
	m_speed = 0.0f;
	m_elapsedTime = 0.0f;
	m_prepared = false;
	return false;
}

void cCommands_Move::PrepareUpdate()
{
	if (m_pObject != nullptr)
	{
		m_startXYZ = m_pObject->GetPosition();
	}
	if (m_pSubject != nullptr)
	{
		m_endXYZ = m_pSubject->GetPosition();
	}

	m_distance = glm::distance(m_endXYZ, m_startXYZ);

	if (m_distance < 0.1f)
	{
		return;
	}

	m_direction = glm::normalize(m_endXYZ - m_startXYZ);

	glm::quat quat = g_pQuatUtils->LookAt(m_direction);

	m_pObject->SetQuatOrientation(quat);

	m_distance = glm::distance(m_endXYZ, m_startXYZ);

	if (m_easeIn || m_easeOut)
	{
		m_acceleration = GetAcceleration(m_distance, m_timeToMove);
		return;
	}
	else if (m_timeToMove != 0)
	{
		m_speed = m_distance / m_timeToMove;
		return;
	}
	else
	{
		m_speed = m_maxSpeed;
		m_timeToMove = m_distance / m_speed;
		return;
	}

	return;
}

bool cCommands_Move::Update(float deltaTime)
{
	if (!m_prepared)
	{
		this->PrepareUpdate();
		m_prepared = true;
	}

	glm::vec3 currentXYZ = m_pObject->GetPosition();
	float distance = glm::distance(currentXYZ, m_endXYZ);

	if (m_easeIn || m_easeOut)
	{
		this->EaseInOut(deltaTime);
	}
	else
	{
		m_velocity = m_direction * m_speed;
	}

	currentXYZ += m_velocity * (float)deltaTime;

	m_pObject->SetPosition(currentXYZ);

	m_elapsedTime += deltaTime;

	//std::cout << "time: " << m_elapsedTime << " // distance: " << distance
	//		  << " // speed: " << m_speed << " // accel: " << m_acceleration << std::endl;

	if (m_elapsedTime >= m_timeToMove)
	{
		m_speed = 0.0f;
		m_elapsedTime = 0.0f;
		m_prepared = false;
		return true;
	}

	return false;
}

void cCommands_Move::EaseInOut(float deltaTime)
{
	if (m_easeIn && m_easeOut)
	{
		if (m_elapsedTime < (m_timeToMove * 0.17))
		{
			m_speed += m_acceleration * deltaTime;
		}
		if (m_elapsedTime > (m_timeToMove * 0.88))
		{
			m_speed -= m_acceleration * deltaTime;
		}
	}
	if (!m_easeIn && m_easeOut)
	{
		if (m_elapsedTime <= (m_timeToMove * 0.85))
		{
			m_speed = m_distance / m_timeToMove;
		}
		if (m_elapsedTime > (m_timeToMove * 0.9))
		{
			m_speed -= m_acceleration * deltaTime / 3.0f;
		}
	}
	if (m_easeIn && !m_easeOut)
	{
		if (m_elapsedTime < (m_timeToMove * 0.17))
		{
			m_speed += m_acceleration * deltaTime;
		}
	}

	m_velocity = m_direction * m_speed;
}

float cCommands_Move::GetAcceleration(float distance, float time)
{
	// EaseInOut
	float dist = distance;
	float tm = time;
	float accel = 0.0f;

	if (tm == 0.0f)
	{
		tm = distance / m_maxSpeed;
	}
	m_timeToMove = tm;

	float dist15 = dist * 0.15f;
	float time15 = tm * 0.15f;

	accel = dist15 / (time15 * time15);

	return accel;
}


std::string cCommands_Move::GetCommandName()
{
	return this->m_commandName;
}

void cCommands_Move::SetCommandName(std::string commandName)
{
	this->m_commandName = commandName;
}


void cCommands_Move::Initialize(std::string objectName, glm::vec3 endXYZ,
								float timeToMove, float maxSpeed, bool easeIn, bool easeOut)
{
	m_pObject = g_pFindMeshByFriendlyName(objectName);

	m_startXYZ = m_pObject->GetPosition();
	m_endXYZ = endXYZ;

	if (timeToMove >= 0.0f)
	{
		m_timeToMove = timeToMove;
	}

	if (maxSpeed > 0.0f)
	{
		m_maxSpeed = maxSpeed;
	}

	m_easeIn = easeIn;
	m_easeOut = easeOut;
}

void cCommands_Move::Initialize(std::string objectName, std::string subjectName,
								float timeToMove, float maxSpeed, bool easeIn, bool easeOut)
{
	m_pObject = g_pFindMeshByFriendlyName(objectName);
	m_pSubject = g_pFindMeshByFriendlyName(subjectName);

	this->m_startXYZ = m_pObject->GetPosition();
	this->m_endXYZ = m_pSubject->GetPosition();
	this->m_velocity = glm::vec3(0.0f);
	this->m_direction = glm::vec3(0.0f);

	this->m_speed = 0.0f;
	this->m_distance = 0.0f;
	this->m_acceleration = 0.0f;

	this->m_elapsedTime = 0.0f;
	this->m_prepared = false;

	if (timeToMove >= 0.0f)
	{
		m_timeToMove = timeToMove;
	}

	if (maxSpeed > 0.0f)
	{
		m_maxSpeed = maxSpeed;
	}

	m_easeIn = easeIn;
	m_easeOut = easeOut;
}
