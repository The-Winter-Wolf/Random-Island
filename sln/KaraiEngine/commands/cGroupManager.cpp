#include "cGroupManager.h"

#include "cCommandManager.h"

#include <iostream>

extern cCommandManager* g_pCommandManager;

cGroupManager::cGroupManager()
{
	this->bIsTriggered = false;
	this->itGroups = m_mpGroups.begin();
	this->itRunGroups = m_vpRunGroups.begin();
}

cGroupManager::~cGroupManager()
{
	this->DeleteAllGroups();
}

void cGroupManager::Update(float deltaTime)
{
	for (unsigned int index = 0; index != m_vpRunGroups.size(); index++)
	{
		if (m_vpRunGroups[index]->Update(deltaTime))
		{
			std::cout << "Group [" << m_vpRunGroups[index]->GetGroupName() << "] ended." << std::endl;
			itRunGroups = m_vpRunGroups.begin() + index;
			m_vpRunGroups.erase(itRunGroups);
			index--;
		}
	}
	for (iCommands* pTrigger : m_vpTriggers)
	{
		if (pTrigger->Update(deltaTime))
		{

			itTriggeredCmd = m_mpTriggeredCmd.find(pTrigger->GetCommandName());

			if (itTriggeredCmd == m_mpTriggeredCmd.end())
			{
				return;
			}
			else
			{
				if (!bIsTriggered)
				{
					this->RunGroup(itTriggeredCmd->second);
					bIsTriggered = true;
					return;
				}

			}
		}
	}
}

cCommands_Group* cGroupManager::GetGroup(std::string groupName)
{
	itGroups = m_mpGroups.find(groupName);

	if (itGroups == m_mpGroups.end())
	{
		std::cout << "Get group failed. Group [" << groupName << "] not found." << std::endl;
		return nullptr;
	}
	if (itGroups != m_mpGroups.end())
	{
		return itGroups->second;
	}
}

void cGroupManager::CreateSerialGroup(std::string groupName)
{
	cCommands_Group* pSerialGroup = new cCommands_Group(SERIAL);
	pSerialGroup->SetGroupName(groupName);
	m_mpGroups[groupName] = pSerialGroup;
	std::cout << "Serial group [" << groupName << "] created successfully." << std::endl;
	return;
}

void cGroupManager::CreateParallelGroup(std::string groupName)
{
	cCommands_Group* pParallelGroup = new cCommands_Group(PARALLEL);
	pParallelGroup->SetGroupName(groupName);
	m_mpGroups[groupName] = pParallelGroup;
	std::cout << "Parallel group [" << groupName << "] created successfully." << std::endl;
	return;
}

void cGroupManager::CreateInfinityGroup(std::string groupName)
{
	cCommands_Group* pInfinityGroup = new cCommands_Group(INFINIT);
	pInfinityGroup->SetGroupName(groupName);
	m_mpGroups[groupName] = pInfinityGroup;
	std::cout << "Infinity group [" << groupName << "] created successfully." << std::endl;
	return;
}

void cGroupManager::RunGroup(std::string groupName)
{
	cCommands_Group* group = this->GetGroup(groupName);

	this->m_vpRunGroups.push_back(group);

	std::cout << "Group [" << groupName << "] started." << std::endl;

	return;
}

void cGroupManager::PauseGroup(std::string groupName)
{
	for (unsigned int index = 0; index != m_vpRunGroups.size(); index++)
	{
		itRunGroups = m_vpRunGroups.begin() + index;

		if ((*itRunGroups)->GetGroupName() == groupName)
		{
			std::cout << "Group [" << groupName << "] paused." << std::endl;
			m_vpRunGroups.erase(itRunGroups);
			index--;
			return;
		}
		if (itRunGroups == m_vpRunGroups.end())
		{
			return;
		}
	}
}

void cGroupManager::StopGroup(std::string groupName)
{
	bIsTriggered = false;

	for (unsigned int index = 0; index != m_vpRunGroups.size(); index++)
	{
		itRunGroups = m_vpRunGroups.begin() + index;

		if ((*itRunGroups)->GetGroupName() == groupName)
		{
			(*itRunGroups)->StopGroup();
			std::cout << "Group [" << groupName << "] stopped." << std::endl;
			m_vpRunGroups.erase(itRunGroups);
			index--;
			return;
		}
		if (itRunGroups == m_vpRunGroups.end())
		{
			return;
		}
	}
}

void cGroupManager::DeleteGroup(std::string groupName)
{
	this->StopGroup(groupName);

	itGroups = m_mpGroups.find(groupName);

	if (itGroups == m_mpGroups.end())
	{
		std::cout << "Delete failed. Group [" << groupName << "] not found." << std::endl;
		return;
	}
	if (itGroups != m_mpGroups.end())
	{
		itGroups->second->~cCommands_Group();
		m_mpGroups.erase(itGroups);

		std::cout << "Group [" << groupName << "] deleted." << std::endl;
		return;
	}
}

void cGroupManager::RunAllGroups()
{
	m_vpRunGroups.clear();

	for (itGroups = m_mpGroups.begin(); itGroups != m_mpGroups.end(); itGroups++)
	{
		this->RunGroup(itGroups->first);
	}

	std::cout << "All groups have been run." << std::endl;
	return;
}

void cGroupManager::PauseAllGroups()
{
	m_vpRunGroups.clear();

	std::cout << "All groups have been paused." << std::endl;
	return;
}

void cGroupManager::StopAllGroups()
{
	for (itGroups = m_mpGroups.begin(); itGroups != m_mpGroups.end(); itGroups++)
	{
		this->StopGroup(itGroups->first);
	}

	std::cout << "All groups have been stopped." << std::endl;
	return;
}

void cGroupManager::DeleteAllGroups()
{
	m_vpRunGroups.clear();

	for (itGroups = m_mpGroups.begin(); itGroups != m_mpGroups.end(); itGroups++)
	{
		if (itGroups == m_mpGroups.end())
		{
			itGroups->second->~cCommands_Group();
			m_mpGroups.erase(itGroups);
			std::cout << "All groups have been deleted." << std::endl;
			return;
		}
		itGroups->second->~cCommands_Group();
		m_mpGroups.erase(itGroups);
	}

	std::cout << "All groups have been deleted." << std::endl;
	return;
}

void cGroupManager::AddGroupTrigger(std::string triggerName)
{
	std::string triggerGroup;

	iCommands* pTriggerCmd = g_pCommandManager->GetGroupTrigger(triggerName, triggerGroup);

	itGroups = m_mpGroups.find(triggerGroup);

	if (itGroups == m_mpGroups.end())
	{
		return;
	}
	else
	{
		m_mpTriggeredCmd[triggerName] = triggerGroup;
		m_vpTriggers.push_back(pTriggerCmd);

		return;
	}
}

void cGroupManager::AddCommandToGroup(std::string groupName, std::string commandName)
{
	cCommands_Group* group = this->GetGroup(groupName);
	group->AddCommandToGroup(commandName);

	std::cout << "Command [" << commandName << "] added to [" << groupName << "]." << std::endl;
	return;
}

void cGroupManager::DeleteCommandFromGroup(std::string groupName, std::string commandName)
{
	cCommands_Group* group = this->GetGroup(groupName);
	group->DeleteCommandFromGroup(commandName);

	std::cout << "Command [" << commandName << "] deleted from [" << groupName << "]." << std::endl;
	return;
}

