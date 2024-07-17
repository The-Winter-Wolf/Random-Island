#pragma once
#include "cCommands_Group.h"

#include <string>
#include <map>

class cGroupManager
{
public:
	cGroupManager();
	~cGroupManager();

	void Update(float deltaTime);

	void CreateSerialGroup(std::string groupName);

	void CreateParallelGroup(std::string groupName);

	void CreateInfinityGroup(std::string groupName);

	cCommands_Group* GetGroup(std::string groupName);

	void RunGroup(std::string groupName);

	void PauseGroup(std::string groupName);

	void StopGroup(std::string groupName);

	void DeleteGroup(std::string groupName);

	void RunAllGroups();

	void PauseAllGroups();

	void StopAllGroups();

	void DeleteAllGroups();

	void AddGroupTrigger(std::string triggerName);

	void AddCommandToGroup(std::string groupName, std::string commandName);

	void DeleteCommandFromGroup(std::string groupName, std::string commandName);

private:

	bool bIsTriggered;

	std::map< std::string, cCommands_Group* > m_mpGroups;
	std::map< std::string, cCommands_Group* >::iterator itGroups;

	std::map< std::string /*triggerName*/, std::string /*triggerCmd*/ > m_mpTriggeredCmd;
	std::map< std::string, std::string >::iterator itTriggeredCmd;

	std::vector< iCommands* > m_vpTriggers;
	std::vector< iCommands* >::iterator itTriggers;

	std::vector< cCommands_Group* > m_vpRunGroups;
	std::vector< cCommands_Group* >::iterator itRunGroups;
};

