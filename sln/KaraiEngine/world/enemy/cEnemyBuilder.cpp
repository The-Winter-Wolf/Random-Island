#include "cEnemyBuilder.h"

#include "cVampire.h"
#include "cWarrok.h"
#include "cSlime.h"

#include "../../behaviour/cBehaviour.h"

#include "../../world/player/cPlayer.h"
extern cPlayer* g_pPlayer;

#include "../terrain/cTerrain.h"
extern cTerrain* g_pTerrain;

#include "../../global/global.h"
#include <glm/gtc/random.hpp>

cEnemyBuilder::cEnemyBuilder(){}

cEnemyBuilder::~cEnemyBuilder(){}

iEnemy* cEnemyBuilder::CreateEnemy(eEnemy enemy, int index)
{
	glm::vec3 playerPos = g_pPlayer->GetPlayerObject()->GetPosition();

	if (enemy == WARROK)
	{
		cWarrok* warrok = new cWarrok(index);
		warrok->InitEnemy();

		glm::vec3 pos = this->GetRandomPosition(playerPos, 40.0f, 15.0f, true);
		warrok->GetEnemyObject()->SetPosition(pos);

		this->m_GetRandomBehaviour(warrok, WARROK, 10.0f);

		return warrok;
	}
	if (enemy == VAMPIRE)
	{
		cVampire* vampire = new cVampire(index);
		vampire->InitEnemy();

		glm::vec3 pos = this->GetRandomPosition(playerPos, 40.0f, 15.0f, true);
		vampire->GetEnemyObject()->GetPhysBody()->SetPosition(pos);

		this->m_GetRandomBehaviour(vampire, VAMPIRE, 10.0f);

		return vampire;
	}
	if (enemy == SLIME)
	{
		cSlime* slime = new cSlime(index);
		slime->InitEnemy();

		this->m_GetRandomBehaviour(slime, SLIME, 10.0f);

		return slime;
	}
}

void cEnemyBuilder::m_GetRandomBehaviour(iEnemy* enemy, eEnemy type, int numOfPaths)
{
	cBehaviour* behaviour = new cBehaviour();
	enemy->SetBehaviour(behaviour);

	cBehaviourType* lookBehaviour = new cBehaviourType();
	cBehaviourType* seekBehaviour = new cBehaviourType();
	cBehaviourType* pathBehaviour = new cBehaviourType();
	cBehaviourType* evadeBehaviour = new cBehaviourType();

	cObject3D* object = enemy->GetEnemyObject();

	std::vector<glm::vec3> pathPositions;

	for (int index = 0; index < numOfPaths; index++)
	{
		glm::vec3 pos = this->GetRandomPosition(WORLD_POS, 40.0f, 20.0f, true);
		pathPositions.push_back(pos);
	}

	if (type == WARROK)
	{
		lookBehaviour->LookBehaviour(object, g_pPlayer->GetPlayerObject(), 3.0f);
		seekBehaviour->SeekBehaviour(object, g_pPlayer->GetPlayerObject(), 30.0f, 3.0f, 1.0f);
		pathBehaviour->PathFollowingBehaviour(object, pathPositions, 30.0f, 3.0f, 1.0f);
		evadeBehaviour->EvadeBehaviour(object, g_pPlayer->GetPlayerObject(), 30, 3.0f);
	}
	if (type == VAMPIRE)
	{
		lookBehaviour->LookBehaviour(object, g_pPlayer->GetPlayerObject(), 5.0f);		
		seekBehaviour->SeekBehaviour(object, g_pPlayer->GetPlayerObject(), 40.0f, 5.0f, 1.0f);
		pathBehaviour->PathFollowingBehaviour(object, pathPositions, 40.0f, 5.0f, 1.0f);
		evadeBehaviour->EvadeBehaviour(object, g_pPlayer->GetPlayerObject(), 40, 5.0f);
	}
	if (type == SLIME)
	{
		lookBehaviour->LookBehaviour(object, g_pPlayer->GetPlayerObject(), 1.0f);
		seekBehaviour->SeekBehaviour(object, g_pPlayer->GetPlayerObject(), 5.0f, 1.0f, 1.0f);
		pathBehaviour->PathFollowingBehaviour(object, pathPositions, 5.0f, 1.0f, 1.0f);
		evadeBehaviour->EvadeBehaviour(object, g_pPlayer->GetPlayerObject(), 50, 1.0f);
	}

	enemy->GetBehaviour()->AddBehaviour(lookBehaviour, "Look");
	enemy->GetBehaviour()->AddBehaviour(seekBehaviour, "Seek");
	enemy->GetBehaviour()->AddBehaviour(pathBehaviour, "Path");
	enemy->GetBehaviour()->AddBehaviour(evadeBehaviour, "Evade");
}

glm::vec3 cEnemyBuilder::GetRandomPosition(const glm::vec3& centerPos, 
	float spawnRadius, float avoidRadius, bool terrainNormals)
{
	glm::vec2 worldPos = glm::vec2(WORLD_POS.x, WORLD_POS.z);

	glm::vec2 startPos = glm::vec2(centerPos.x, centerPos.z);
	if (avoidRadius > spawnRadius)
	{
		avoidRadius = 0.0f;
	}

	bool isLoop = true;
	glm::vec2 position = glm::vec2(0.0f);
	glm::vec2 direction = glm::vec2(0.0f);

	while (isLoop)
	{
		float radius = glm::linearRand(avoidRadius, spawnRadius);
		glm::vec2 direction = glm::normalize(glm::diskRand(10000.0f) + glm::diskRand(10000.0f));
		position = startPos + direction * radius;
		isLoop = false;

		if (glm::distance(worldPos, position) > spawnRadius)
		{
			isLoop = true;
		}
	}

	glm::vec3 terrainNormal;
	float hight = g_pTerrain->GetTerrainHight(position.x, position.y, terrainNormal);
	glm::vec3 posVec3 = glm::vec3(position.x, hight, position.y);
	return posVec3;
}
