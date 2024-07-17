#pragma once

#include "iCommands.h"

#include "../objects/object_3D/cObject3D.h"

class cCommands_Orient : public iCommands
{
public:
	cCommands_Orient();
	~cCommands_Orient();

	virtual bool Stop();

	virtual bool Update(float deltaTime);

	virtual std::string GetCommandName();

	virtual void SetCommandName(std::string commandName);

	void Initialize(std::string objectName, std::string subjectName, float timeToOrient = 0.0f, bool infinity = false);

	void Initialize(std::string objectName, glm::vec3 endXYZ, float timeToOrient = 0.0f, bool infinity = false);

private:

	void PrepareUpdate();

	std::string m_commandName;

	glm::vec3 m_startXYZ;
	glm::vec3 m_endXYZ;
	glm::vec3 m_velocity;

	glm::quat m_startOrient;
	glm::quat m_endOrient;
	glm::quat m_current;

	float m_angle;
	float m_distance;
	float m_timeToOrient;
	float m_elapsedTime;

	cObject3D* m_pObject;
	cObject3D* m_pSubject;

	bool m_prepared;
	bool m_infinity;
};


