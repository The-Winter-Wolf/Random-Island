#include "cCommands_Curve.h"

#include "../utils/cQuaternionUtils.h"

extern cQuaternionUtils* g_pQuatUtils;
extern cObject3D* g_pFindMeshByFriendlyName(std::string friendlyNameToFind);

cCommands_Curve::cCommands_Curve()
{
	this->m_angle = 0.5f;
	this->m_time = 0.0f;
	this->m_currentTime = 0.0f;
	this->m_pObject = nullptr;
	this->m_startXYZ = glm::vec3(0.0f);
	this->m_prepared = false;
}

cCommands_Curve::~cCommands_Curve() {}

bool cCommands_Curve::Stop()
{
	m_currentTime = 0;
	return true;
}

std::string cCommands_Curve::GetCommandName()
{
	return this->m_commandName;
}

void cCommands_Curve::SetCommandName(std::string commandName)
{
	this->m_commandName = commandName;
}

void cCommands_Curve::Initialize(std::string objectName, float time, float angle,
	 glm::vec3 point1, glm::vec3 point2, glm::vec3 point3, glm::vec3 point4, glm::vec3 point5)
{
	this->m_pObject = g_pFindMeshByFriendlyName(objectName);
	this->m_startXYZ = m_pObject->GetPosition();

	if (time > 0.0f)
	{
		this->m_time = time;
	}
	if (angle >= 0.0f)
	{
		this->m_angle = angle;
	}

	m_point01 = point1;
	m_point02 = point2;
	m_point03 = point3;
	m_point04 = point4;
	m_point05 = point5;
}

bool cCommands_Curve::Update(float deltaTime)
{
	if (m_time <= 0.0f || m_currentTime > 1.0f)
	{
		points.clear();
		m_currentTime = 0;
		m_prepared = false;
		return true;
	}

	if (!m_prepared)
	{
		this->PrepareUpdate();
	}

	glm::vec3 startXYZ = m_pObject->GetPosition();
	glm::quat startOrient = m_pObject->GetQuatOrientation();

	m_currentTime += deltaTime * 1.0f/m_time;

	m_currentXYZ = this->BesierCurve();

	m_pObject->SetPosition(m_currentXYZ);

	glm::vec3 dir = glm::normalize(m_currentXYZ - startXYZ);

	glm::quat endOrient = g_pQuatUtils->LookAt(dir);

	endOrient = g_pQuatUtils->RotateTowards(startOrient, endOrient, m_angle * deltaTime);

	m_pObject->SetQuatOrientation(endOrient);

	return false;
}

void cCommands_Curve::PrepareUpdate()
{
	points.clear();

	m_startXYZ = m_pObject->GetPosition();

	this->points.push_back(m_startXYZ);
	if (m_point01 != glm::vec3(0.0f))
		this->points.push_back(m_point01);
	if (m_point02 != glm::vec3(0.0f))
		this->points.push_back(m_point02);
	if (m_point03 != glm::vec3(0.0f))
		this->points.push_back(m_point03);
	if (m_point04 != glm::vec3(0.0f))
		this->points.push_back(m_point04);
	if (m_point05 != glm::vec3(0.0f))
		this->points.push_back(m_point05);

	m_prepared = true;
}

glm::vec3 cCommands_Curve::BesierCurve()
{
	int size = static_cast<int>(points.size());
	float time = 1.0f - m_currentTime;
	std::vector<glm::vec3> temp;
	for (int i = 0; i < size-1; i++)
	{
		glm::vec3 vector = time * points[i] + m_currentTime * points[i+1];
		temp.push_back(vector);
		bool check = false;
	}
	while (size > 1)
	{
		size--;
		for (int k = 0; k < size-1; k++)
		{
			temp[k] = time * temp[k] + m_currentTime * temp[k+1];
		}
	}
	return temp[0];
}
