#pragma once

#include "iCommands.h"

#include "../objects/object_3D/cObject3D.h"

class cCommands_Trigger : public iCommands
{
public:
	cCommands_Trigger();
	~cCommands_Trigger();

	virtual bool Stop();

	virtual bool Update(float deltaTime);

	virtual std::string GetCommandName();

	virtual void SetCommandName(std::string commandName);

	void Initialize(std::string triggerObj, std::string triggeredSbj, float distance = 0.0f, float delayTime = 0.0f);

	void Initialize(glm::vec3 triggerXYZ, std::string triggeredSbj, float distance = 0.0f, float delayTime = 0.0f);

private:

	std::string m_commandName;
	std::string m_triggeredCmd;

	float m_delayTime;
	float m_distance;
	float m_time;

	bool m_bIsTriggered;
	
	glm::vec3 m_objXYZ;
	glm::vec3 m_sbjXYZ;

	cObject3D* m_pObject;
	cObject3D* m_pSubject;
};

