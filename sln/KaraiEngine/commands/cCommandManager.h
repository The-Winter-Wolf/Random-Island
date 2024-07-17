#pragma once

#include "iCommands.h"

#include <glm/glm.hpp>

#include <vector>
#include <map>

class cCommandManager
{
public:
	cCommandManager();
	~cCommandManager();

	void Update(float deltaTime);

	void RunAllCommands();

	void PauseAllCommands();

	void StopAllCommands();

	void DeleteAllCommands();

	void RunCommand(std::string commandName);

	void PauseCommand(std::string commandName);

	void StopCommand(std::string commandName);

	void DeleteCommand(std::string commandName);

	iCommands* GetCommand(std::string commandName);

	void AddCurveCommand(std::string commandName, std::string objectName, float time, float rotAngle, glm::vec3 point1,
						 glm::vec3 point2 = glm::vec3(0.0f), glm::vec3 point3 = glm::vec3(0.0f),
						 glm::vec3 point4 = glm::vec3(0.0f), glm::vec3 point5 = glm::vec3(0.0f));

	void AddFollowCommand(std::string commandName, std::string objectName, std::string subjectName,
						  float distance = 0.0f, float speed = 0.0f, glm::vec3 offset = glm::vec3(0.0f), bool infinity = false);

	void AddMoveCommand(std::string commandName, std::string objectName, glm::vec3 endXYZ,
						float timeToMove = 0.0f, float speed = 0.0f, bool easeIn = false, bool easeOut = false);

	void AddMoveCommand(std::string commandName, std::string objectName, std::string subjectName,
						float timeToMove = 0.0f, float speed = 0.0f, bool easeIn = false, bool easeOut = false);

	void AddOrientCommand(std::string commandName, std::string objectName, std::string subjectName,
						 float timeToOrient = 0.0f, bool infinity = false);

	void AddOrientCommand(std::string commandName, std::string objectName, glm::vec3 endXYZ,
						  float timeToOrient = 0.0f, bool infinity = false);

	void AddRotateCommand(std::string commandName, std::string objectName, glm::vec3 angleXYZ,
						  float timeToOrient = 0.0f, bool infinity = false);

	void AddTriggerCommand(std::string commandName, std::string triggeredCmd, std::string triggerObj, std::string triggeredSbj,
						   float distance = 0.0f, float delayTime = 0.0f);

	void AddTriggerCommand(std::string commandName, std::string triggeredCmd, glm::vec3 triggerXYZ, std::string triggeredSbj,
						   float distance = 0.0f, float delayTime = 0.0f);

	void SetObjectColor(std::string commandName, std::string objectName, bool isColor, glm::vec4 newColor);

	void SetVisibility(std::string commandName, std::string objectName, bool visibility);

	void GetObjectCopy(std::string commandName, std::string objectName, std::string newFriendlyName, glm::vec3 posXYZ);

	iCommands* GetGroupTrigger(std::string &triggerName, std::string &triggeredCmd);

private:

	std::map<std::string /*commandName*/, iCommands* /*command*/> m_mpCommands;
	std::map<std::string /*commandName*/, iCommands* /*command*/>::iterator itCommands;

	std::map<std::string /*triggerName*/, std::string /*triggeredCmd*/> m_mpTriggeredCmds;
	std::map<std::string /*triggerName*/, std::string /*triggeredCmd*/>::iterator itTriggerCmds;

	std::vector<iCommands*> m_vpRunCommands;
	std::vector<iCommands*>::iterator itRunCommands;

	bool bIsTriggered;
};

