#pragma once

#include "iCommands.h"

#include "../objects/object_3D/cObject3D.h"

class cCommands_Rotate : public iCommands
{
public:
	cCommands_Rotate();
	~cCommands_Rotate();

	virtual bool Stop();

	virtual bool Update(float deltaTime);

	virtual std::string GetCommandName();

	void SetCommandName(std::string commandName);

	void Initialize(std::string objectName, glm::vec3 angleXYZ, float timeToOrient = 0.0f, bool infinity = false);

private:

	void PrepareUpdate();

	std::string m_commandName;

	glm::vec3 m_startXYZ;
	glm::vec3 m_endXYZ;
	glm::vec3 m_velocity;

	glm::quat m_startOrient;
	glm::quat m_endOrient;
	glm::quat m_current;

	float m_timeToOrient;
	float m_elapsedTime;

	cObject3D* m_pObject;

	bool m_prepared;
	bool m_infinity;
};


