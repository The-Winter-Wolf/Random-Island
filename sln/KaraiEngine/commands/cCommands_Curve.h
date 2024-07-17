#pragma once

#include "iCommands.h"

#include "../objects/object_3D/cObject3D.h"

#include <vector>

class cCommands_Curve : public iCommands
{
public:
	cCommands_Curve();
	~cCommands_Curve();

	virtual bool Stop();

	virtual bool Update(float deltaTime);

	virtual std::string GetCommandName();
	
	virtual void SetCommandName(std::string commandName);

	void Initialize(std::string objectName, float time, float angle, glm::vec3 point1, 
					glm::vec3 point2 = glm::vec3(0.0f), glm::vec3 point3 = glm::vec3(0.0f),
					glm::vec3 point4 = glm::vec3(0.0f), glm::vec3 point5 = glm::vec3(0.0f));

private:

	void PrepareUpdate();

	glm::vec3 BesierCurve();

	std::string m_commandName;

	std::vector<glm::vec3> points;

	float m_angle;
	float m_time;
	float m_currentTime;

	bool m_prepared;

	glm::vec3 m_currentXYZ;
	glm::vec3 m_startXYZ;
	glm::vec3 m_point01;
	glm::vec3 m_point02;
	glm::vec3 m_point03;
	glm::vec3 m_point04;
	glm::vec3 m_point05;

	cObject3D* m_pObject;
};