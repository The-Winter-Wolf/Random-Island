#pragma once

#include "../../objects/object_3D/cObject3D.h"
#include "../../behaviour/cBehaviour.h"

class iEnemy
{
public:

	virtual ~iEnemy() {};

    virtual void InitEnemy() = 0;
	virtual void UpdateEnemy(float deltaTime) = 0;

	virtual unsigned int GetEnemyIndex() = 0;
    virtual cObject3D* GetEnemyObject() = 0;

	virtual void SetBehaviour(cBehaviour* behaviour) = 0;
	virtual cBehaviour* GetBehaviour() const = 0;
};
