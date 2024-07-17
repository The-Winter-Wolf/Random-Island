#pragma once

#include "iCommands.h"

#include "../objects/object_3D/cObject3D.h"

class cCommands_Follow : public iCommands
{
public:
	cCommands_Follow();
	~cCommands_Follow();

	virtual bool Stop();

	virtual bool Update(float deltaTime);

	virtual std::string GetCommandName();

	virtual void SetCommandName(std::string commandName);	

	void Initialize(std::string objectName, std::string subjectName, float distance = 0.0f,
					float speed = 0.0f, glm::vec3 offset = glm::vec3(0.0f), bool infinity = false);

private:

	void EaseInOut(float deltaTime);

	float GetAcceleration(float distance);

	std::string m_commandName;

	glm::vec3 m_offset;

	glm::vec3 m_startXYZ;
	glm::vec3 m_endXYZ;

	glm::vec3 m_velocity;
	glm::vec3 m_direction;

	glm::quat m_startOrient;
	glm::quat m_endOrient;

	float m_speed;
	float m_maxSpeed;

	float m_curdist;
	float m_distance;
	float m_followDist;

	float m_timeToMove;
	float m_elapsedTime;

	float m_acceleration;

	cObject3D* m_pObject;
	cObject3D* m_pSubject;

	bool m_easeIn;
	bool m_easeOut;
	bool m_prepared;
	bool m_infinity;
};

