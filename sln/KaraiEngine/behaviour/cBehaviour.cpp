#include "cBehaviour.h"

cBehaviour::cBehaviour(){}

cBehaviour::~cBehaviour()
{
    m_behavIterator = m_mapBehaviours.begin();
    for (m_behavIterator; m_behavIterator != m_mapBehaviours.end(); m_behavIterator++)
    {
        delete m_behavIterator->second;
        m_mapBehaviours.erase(m_behavIterator);
        m_behavIterator--;
    }
}

void cBehaviour::AddBehaviour(cBehaviourType* behaviour, const std::string& friendlyName)
{
    this->m_mapBehaviours[friendlyName] = behaviour;
}

void cBehaviour::PlayBehaviour(const std::string& friendlyName)
{
    m_behavIterator = m_mapBehaviours.find(friendlyName);
    if (m_behavIterator == m_mapBehaviours.end())
    {
        return;
    }

    this->m_currentBehav = m_behavIterator->second;
}

void cBehaviour::UpdateBehaviour(float deltaTime)
{
    if (!m_currentBehav)
    {
        return;
    }

    this->m_currentBehav->Update(deltaTime);
}
