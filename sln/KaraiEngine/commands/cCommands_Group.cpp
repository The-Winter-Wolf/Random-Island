#include "cCommands_Group.h"

#include "cCommandManager.h"

extern cCommandManager* g_pCommandManager;

cCommands_Group::cCommands_Group(eGroupType groupType)
{
	this->groupType = groupType;
	this->m_serialIndex = 0;
	this->m_parallelIndex = 0;
}

cCommands_Group::~cCommands_Group()
{
	m_vecCommands.clear();
}

void cCommands_Group::StopGroup()
{
	for (itCommands = m_vecCommands.begin(); itCommands != m_vecCommands.end(); itCommands++)
	{
		(*itCommands)->Stop();
	}
}

bool cCommands_Group::Update(float deltaTime)
{
	if (groupType == SERIAL)
	{
		if (m_vecCommands[m_serialIndex]->Update(deltaTime))
		{
			m_serialIndex++;
		}
		if (m_serialIndex == m_vecCommands.size())
		{
			m_serialIndex = 0;
			return true;
		}

		return false;
	}
	if (groupType == PARALLEL)
	{
		for (unsigned int index = 0; index != m_vecCommands.size(); index++)
		{
			if (m_vecCommands[index]->Update(deltaTime))
			{
				m_parallelIndex++;
			}
			if (m_parallelIndex == m_vecCommands.size())
			{
				m_parallelIndex = 0;
				return true;
			}
		}

		return false;
	}
	if (groupType == INFINIT)
	{
		for (unsigned int index = 0; index != m_vecCommands.size(); index++)
		{
			m_vecCommands[index]->Update(deltaTime);
		}

		return true;
	}

	return false;
}

std::string cCommands_Group::GetGroupName()
{
	return this->groupName;
}

eGroupType cCommands_Group::GetGroupType()
{
	return this->groupType;
}

void cCommands_Group::SetGroupName(std::string groupName)
{
	this->groupName = groupName;
}

void cCommands_Group::AddCommandToGroup(std::string commandName)
{
	iCommands* command = g_pCommandManager->GetCommand(commandName);
	m_vecCommands.push_back(command);
}

void cCommands_Group::DeleteCommandFromGroup(std::string commandName)
{
	for (itCommands = m_vecCommands.begin(); itCommands != m_vecCommands.end(); itCommands++)
	{
		if ((*itCommands)->GetCommandName() == commandName)
		{
			m_vecCommands.erase(itCommands);
			return;
		}
		if (itCommands == m_vecCommands.end())
		{
			return;
		}
	}
}
