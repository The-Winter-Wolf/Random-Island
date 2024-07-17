#pragma once

#include "iCommands.h"

#include <vector>

enum eGroupType
{
	SERIAL,
	PARALLEL,
	INFINIT
};

class cCommands_Group
{
public:
	cCommands_Group(eGroupType groupType);
	~cCommands_Group();

	void StopGroup();

	bool Update(float deltaTime);

	std::string GetGroupName();

	eGroupType GetGroupType();

	void SetGroupName(std::string groupName);

	void AddCommandToGroup(std::string commandName);

	void DeleteCommandFromGroup(std::string commandName);

private:

	unsigned int m_parallelIndex;
	unsigned int m_serialIndex;

	std::string groupName;

	eGroupType groupType;

	std::vector< iCommands* > m_vecCommands;
	std::vector< iCommands*>::iterator itCommands;
};
