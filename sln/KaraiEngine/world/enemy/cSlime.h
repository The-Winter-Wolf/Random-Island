#pragma once

#include "iEnemy.h"

#include "../../objects/object_3D/cObject3D.h"
#include "../../behaviour/cBehaviour.h"

class cSlime : public iEnemy
{
public:
	cSlime(unsigned int index);
	~cSlime();

	void InitEnemy();
	void UpdateEnemy(float dt);

	cObject3D* GetEnemyObject();
	unsigned int GetEnemyIndex();

	void SetBehaviour(cBehaviour* behaviour);
	cBehaviour* GetBehaviour() const;

private:
	cObject3D* m_pObject;
	cBehaviour* m_pBehaviour;

	unsigned int m_enemyIndex;

	bool bIsTrigger1;
	bool bIsTrigger2;
	float m_triggerRadius1;
	float m_triggerRadius2;

	void m_InitObject();
	void m_InitBehaviour();

	void m_TriggerEvents(float dt);
	bool m_CheckFieldOfView();
};
