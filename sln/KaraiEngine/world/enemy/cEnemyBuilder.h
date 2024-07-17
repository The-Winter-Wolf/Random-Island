#pragma once

#include "iEnemy.h"
#include <string>

enum eEnemy
{
	WARROK,
	VAMPIRE,
	SLIME,
};

class cEnemyBuilder
{
public:
	cEnemyBuilder();
	~cEnemyBuilder();

	iEnemy* CreateEnemy(eEnemy enemy, int index);

	glm::vec3 GetRandomPosition(const glm::vec3& centerPos, float spawnRadius, 
		float avoidRadius, bool terrainNormals);

private:

	void m_GetRandomBehaviour(iEnemy* enemy, eEnemy type, int numOfPaths);
};