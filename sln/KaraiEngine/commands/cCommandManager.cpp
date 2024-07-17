#include "cCommandManager.h"

#include "cCommands_Curve.h"
#include "cCommands_Follow.h"
#include "cCommands_Move.h"
#include "cCommands_Orient.h"
#include "cCommands_Rotate.h"
#include "cCommands_Trigger.h"
#include "cCommands_Special.h"

#include <iostream>

cCommandManager::cCommandManager()
{
    this->bIsTriggered = false;
    this->itCommands = m_mpCommands.begin();
    this->itRunCommands = m_vpRunCommands.begin();
}

cCommandManager::~cCommandManager() 
{
    this->DeleteAllCommands();
}

void cCommandManager::Update(float deltaTime)
{
    for (unsigned int index = 0; index != m_vpRunCommands.size(); index++)
    {
        if (m_vpRunCommands[index]->Update(deltaTime))
        {
            for (itTriggerCmds = m_mpTriggeredCmds.begin(); itTriggerCmds != m_mpTriggeredCmds.end(); itTriggerCmds++)
            {
                if (m_vpRunCommands[index]->GetCommandName() == itTriggerCmds->first)
                {
                    if (!bIsTriggered)
                    {
                       this->RunCommand(itTriggerCmds->second);
                       bIsTriggered = true;

                       continue;
                    }

                }
            }

            std::cout << "Command [" << m_vpRunCommands[index]->GetCommandName() << "] ended." << std::endl;
            itRunCommands = m_vpRunCommands.begin() + index;
            m_vpRunCommands.erase(itRunCommands);
            index--;
        }
    }
}

void cCommandManager::RunCommand(std::string commandName)
{
    iCommands* command = this->GetCommand(commandName);

    this->m_vpRunCommands.push_back(command);

    std::cout << "Command [" << commandName << "] started." << std::endl;

    return;
}

void cCommandManager::PauseCommand(std::string commandName)
{
    for (unsigned int index = 0; index != m_vpRunCommands.size(); index++)
    {
        itRunCommands = m_vpRunCommands.begin() + index;

        if ((*itRunCommands)->GetCommandName() == commandName)
        {
            std::cout << "Command [" << commandName << "] paused." << std::endl;
            m_vpRunCommands.erase(itRunCommands);
            index--;
            return;
        }
        if (itRunCommands == m_vpRunCommands.end())
        {
            return;
        }
    }
}

void cCommandManager::StopCommand(std::string commandName)
{
    for (unsigned int index = 0; index != m_vpRunCommands.size(); index++)
    {
        itRunCommands = m_vpRunCommands.begin() + index;

        if ((*itRunCommands)->GetCommandName() == commandName)
        {
            (*itRunCommands)->Stop();
            std::cout << "Command [" << commandName << "] stopped." << std::endl;
            m_vpRunCommands.erase(itRunCommands);
            index--;
            return;
        }
        if (itRunCommands == m_vpRunCommands.end())
        {
            return;
        }
    }
}

void cCommandManager::DeleteCommand(std::string commandName)
{
    this->StopCommand(commandName);

    itCommands = m_mpCommands.find(commandName);

    if (itCommands == m_mpCommands.end())
    {
        std::cout << "Delete failed. Command [" << commandName << "] not found." << std::endl;
        return;
    }
    if (itCommands != m_mpCommands.end())
    {
        itCommands->second->~iCommands();
        m_mpCommands.erase(itCommands);

        std::cout << "Command [" << commandName << "] deleted." << std::endl;
        return;
    }        
}

iCommands* cCommandManager::GetCommand(std::string commandName)
{
    itCommands = m_mpCommands.find(commandName);

    if (itCommands == m_mpCommands.end())
    {
        std::cout << "Get command failed. Command [" << commandName << "] not found." << std::endl;
        return NULL;
    }
    else
    {
        return itCommands->second;
    }
}

void cCommandManager::RunAllCommands()
{
    m_vpRunCommands.clear();

    for (itCommands = m_mpCommands.begin(); itCommands != m_mpCommands.end(); itCommands++)
    {
        this->RunCommand(itCommands->first);
    }

    std::cout << "All commands have been run." << std::endl;
    return;
}

void cCommandManager::PauseAllCommands()
{
    m_vpRunCommands.clear();

    std::cout << "All commands have been paused." << std::endl;
    return;
}

void cCommandManager::StopAllCommands()
{
    for (itCommands = m_mpCommands.begin(); itCommands != m_mpCommands.end(); itCommands++)
    {
        this->StopCommand(itCommands->first);
    }

    std::cout << "All commands have been stopped." << std::endl;
    return;
}

void cCommandManager::DeleteAllCommands()
{
    m_vpRunCommands.clear();

    for (itCommands = m_mpCommands.begin(); itCommands != m_mpCommands.end(); itCommands++)
    {
        if (itCommands == m_mpCommands.end())
        {
            itCommands->second->~iCommands();
            m_mpCommands.erase(itCommands);
            std::cout << "All commands have been deleted." << std::endl;
            return;
        }
        itCommands->second->~iCommands();
        m_mpCommands.erase(itCommands);
    }

    std::cout << "All commands have been deleted." << std::endl;
    return;
}


void cCommandManager::AddCurveCommand(std::string commandName, std::string objectName, float time, float rotAngle, glm::vec3 point1,
                                      glm::vec3 point2, glm::vec3 point3, glm::vec3 point4, glm::vec3 point5)
{
    cCommands_Curve* pCurveCommand = new cCommands_Curve();
    pCurveCommand->SetCommandName(commandName);
    pCurveCommand->Initialize(objectName, time, rotAngle, point1, point2, point3, point4, point5);

    this->m_mpCommands[commandName] = pCurveCommand;
    std::cout << "Command [" << commandName << "] prepared successfully." << std::endl;
    return;
}

void cCommandManager::AddFollowCommand(std::string commandName, std::string objectName, std::string subjectName,
                                       float distance, float speed, glm::vec3 offset, bool infinity)
{
    cCommands_Follow* pFollowCommand = new cCommands_Follow();
    pFollowCommand->SetCommandName(commandName);
    pFollowCommand->Initialize(objectName, subjectName, distance, speed, offset, infinity);

    this->m_mpCommands[commandName] = pFollowCommand;
    std::cout << "Command [" << commandName << "] prepared successfully." << std::endl;
    return;
}

void cCommandManager::AddMoveCommand(std::string commandName, std::string objectName, glm::vec3 endXYZ,
                                     float timeToMove, float speed, bool easeIn, bool easeOut)
{
    cCommands_Move* pMoveCommand = new cCommands_Move();
    pMoveCommand->SetCommandName(commandName);
    pMoveCommand->Initialize(objectName, endXYZ, timeToMove, speed, easeIn, easeOut);

    this->m_mpCommands[commandName] = pMoveCommand;
    std::cout << "Command [" << commandName << "] prepared successfully." << std::endl;
    return;
}

void cCommandManager::AddMoveCommand(std::string commandName, std::string objectName, std::string subjectName,
                                     float timeToMove, float speed, bool easeIn, bool easeOut)
{
    cCommands_Move* pMoveCommand = new cCommands_Move();
    pMoveCommand->SetCommandName(commandName);
    pMoveCommand->Initialize(objectName, subjectName, timeToMove, speed, easeIn, easeOut);

    this->m_mpCommands[commandName] = pMoveCommand;
    std::cout << "Command [" << commandName << "] prepared successfully." << std::endl;
    return;
}

void cCommandManager::AddOrientCommand(std::string commandName, std::string objectName, std::string subjectName,
                                       float timeToOrient, bool infinity)
{
    cCommands_Orient* pOrientCommand = new cCommands_Orient();
    pOrientCommand->SetCommandName(commandName);
    pOrientCommand->Initialize(objectName, subjectName, timeToOrient, infinity);

    this->m_mpCommands[commandName] = pOrientCommand;
    std::cout << "Command [" << commandName << "] prepared successfully." << std::endl;
    return;
}

void cCommandManager::AddOrientCommand(std::string commandName, std::string objectName, glm::vec3 endXYZ,
                                       float timeToOrient, bool infinity)
{
    cCommands_Orient* pOrientCommand = new cCommands_Orient();
    pOrientCommand->SetCommandName(commandName);
    pOrientCommand->Initialize(objectName, endXYZ, timeToOrient, infinity);

    this->m_mpCommands[commandName] = pOrientCommand;
    std::cout << "Command [" << commandName << "] prepared successfully." << std::endl;
    return;
}

void cCommandManager::AddRotateCommand(std::string commandName, std::string objectName, glm::vec3 angleXYZ,
                                       float timeToOrient, bool infinity)
{
    cCommands_Rotate* pRotateCommand = new cCommands_Rotate();
    pRotateCommand->SetCommandName(commandName);
    pRotateCommand->Initialize(objectName, angleXYZ, timeToOrient, infinity);

    this->m_mpCommands[commandName] = pRotateCommand;
    std::cout << "Command [" << commandName << "] prepared successfully." << std::endl;
    return;
}

void cCommandManager::AddTriggerCommand(std::string commandName, std::string triggeredCmd, std::string triggerObj,
                                        std::string triggeredSbj, float distance, float delayTime)
{

    this->m_mpTriggeredCmds[commandName] = triggeredCmd;

    cCommands_Trigger* pTriggerCommand = new cCommands_Trigger();

    pTriggerCommand->SetCommandName(commandName);
    pTriggerCommand->Initialize(triggerObj, triggeredSbj, distance, delayTime);

    this->m_mpCommands[commandName] = pTriggerCommand;

    std::cout << "Command [" << commandName << "] prepared successfully." << std::endl;
    return;
}

void cCommandManager::AddTriggerCommand(std::string commandName, std::string triggeredCmd, glm::vec3 triggerXYZ,
                                        std::string triggeredSbj, float distance, float delayTime)
{
    this->m_mpTriggeredCmds[commandName] = triggeredCmd;

    cCommands_Trigger* pTriggerCommand = new cCommands_Trigger();

    pTriggerCommand->SetCommandName(commandName);
    pTriggerCommand->Initialize(triggerXYZ, triggeredSbj, distance, delayTime);

    this->m_mpCommands[commandName] = pTriggerCommand;

    std::cout << "Command [" << commandName << "] prepared successfully." << std::endl;
    return;
}

void cCommandManager::SetObjectColor(std::string commandName, std::string objectName, bool isColor, glm::vec4 newColor)
{
    cCommands_Special* pColorCmd = new cCommands_Special();
    pColorCmd->SetCommandName(commandName);
    pColorCmd->Initialize(objectName, isColor, newColor);

    this->m_mpCommands[commandName] = pColorCmd;
    std::cout << "Command [" << commandName << "] prepared successfully." << std::endl;
    return;
}

void cCommandManager::SetVisibility(std::string commandName, std::string objectName, bool visibility)
{
    cCommands_Special* pVisibleCmd = new cCommands_Special();
    pVisibleCmd->SetCommandName(commandName);
    pVisibleCmd->Initialize(objectName, visibility);

    this->m_mpCommands[commandName] = pVisibleCmd;
    std::cout << "Command [" << commandName << "] prepared successfully." << std::endl;
    return;
}

void cCommandManager::GetObjectCopy(std::string commandName, std::string objectName, std::string newFriendlyName, glm::vec3 posXYZ)
{
    cCommands_Special* pCopyCmd = new cCommands_Special();
    pCopyCmd->SetCommandName(commandName);
    pCopyCmd->Initialize(objectName, newFriendlyName, posXYZ);

    this->m_mpCommands[commandName] = pCopyCmd;
    std::cout << "Command [" << commandName << "] prepared successfully." << std::endl;
    return;
}

iCommands* cCommandManager::GetGroupTrigger(std::string& triggerName, std::string& triggeredCmd)
{
    itTriggerCmds = m_mpTriggeredCmds.find(triggerName);

    if (itTriggerCmds == m_mpTriggeredCmds.end())
    {
        return NULL;
    }
    else
    {
        triggeredCmd = itTriggerCmds->second;
        iCommands* cmd = this->GetCommand(triggerName);
        return cmd;
    }
}

