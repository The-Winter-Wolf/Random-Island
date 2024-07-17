#pragma once

#include "../objects/object_3D/cObject3D.h"
#include "../graphics/models/cModel.h"

#include <vector>

class cBoids
{
public:
	cBoids();
	~cBoids();

	// Boid's parameters
	float boidSpeed;
	float collDist;

	float avoidScale;
	float alignScale;
	float centerScale;

	float attractDist;
	float attractPull;
	float attractPush;

	// Boid's spawner (attractor)
	int numBoids;
	std::string objName;

	glm::vec3 spawnPos;
	float spawnRadius;
	float spawnDelay;

	float radius;
	float xPhase;
	float zPhase;

	void InitBoids();
	void UpdateBoids(float deltaTime);
	void SetBoidModel(cModel* boid);

private:

	struct sBoid
	{
		cObject3D* boidObj = nullptr;
		glm::vec3 velocity = glm::vec3(0.0f);
	};

	std::vector<sBoid> m_vpBoids;

	cObject3D* spawnerObj;
	cModel* boidModel;

	float m_deltaTime;

	void m_MoveAttractor();
};

