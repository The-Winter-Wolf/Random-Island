#pragma once
#include "cBehaviourType.h"

class cBehaviour
{
public:
	cBehaviour();
	~cBehaviour();

	void AddBehaviour(cBehaviourType* behaviour, const std::string& friendlyName);
	void PlayBehaviour(const std::string& friendlyName);
	void UpdateBehaviour(float deltaTime);

private:
	std::map<std::string /*behavName*/, cBehaviourType* /*behaviour*/> m_mapBehaviours;
	std::map<std::string, cBehaviourType*>::iterator m_behavIterator;

	cBehaviourType* m_currentBehav;
};

