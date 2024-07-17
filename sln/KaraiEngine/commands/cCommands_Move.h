#pragma once

#include "iCommands.h"

#include "../objects/object_3D/cObject3D.h"

class cCommands_Move : public iCommands
{
public:
	cCommands_Move();
	~cCommands_Move();

	virtual bool Stop();

	virtual bool Update(float deltaTime);

	virtual std::string GetCommandName();

	virtual void SetCommandName(std::string commandName);

	void Initialize(std::string objectName, glm::vec3 endXYZ,
					float timeToMove = 0.0f, float speed = 0.0f, bool easeIn = false, bool easeOut = false);

	void Initialize(std::string objectName, std::string subjectName,
					float timeToMove = 0.0f, float speed = 0.0f, bool easeIn = false, bool easeOut = false);

private:

	void PrepareUpdate();

	void EaseInOut(float deltaTime);

	float GetAcceleration(float distance, float time = 0.0f);

	std::string m_commandName;

	glm::vec3 m_startXYZ;
	glm::vec3 m_endXYZ;
	glm::vec3 m_velocity;
	glm::vec3 m_direction;

	float m_speed;
	float m_maxSpeed;
	float m_distance;
	float m_timeToMove;
	float m_elapsedTime;
	float m_acceleration;

	cObject3D* m_pObject;
	cObject3D* m_pSubject;

	bool m_easeIn;
	bool m_easeOut;
	bool m_prepared;
};