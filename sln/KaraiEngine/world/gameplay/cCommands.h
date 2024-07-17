#pragma once
#include "../../objects/object_3D/cObject3D.h"
#include "../enemy/iEnemy.h"

class cCommands
{
public:
	cCommands();
	~cCommands();

	void HitObject(cObject3D* object, cObject3D* subject);
	void KillObject(cObject3D* object);
	void RespawnObject(cObject3D* object);
	void EarnExp(cObject3D* object, float exp);
	void RestoreHP(cObject3D* object, float hp);
	void RestoreMP(cObject3D* object, float mp);
	void RestoreST(cObject3D* object, float st);

	void UpdateObjState(const std::vector<iEnemy*>& enemies);

private:

	bool m_CheckFieldOfView(cObject3D* object, cObject3D* subject, float minDist, float minAngle);

	bool bIsAutoRespawn;
};

