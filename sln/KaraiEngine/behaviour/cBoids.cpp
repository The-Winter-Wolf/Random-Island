#include "cBoids.h"
#include "../global/global.h"

#include "../graphics/models/cModelManager.h"
extern cModelManager* g_pModelManager;

#include "../system/time/cTime.h"
extern cTime* g_pTime;

#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/random.hpp>
#include <glm/glm.hpp>

cBoids::cBoids()
{
	// Boid's parameters
	this->boidSpeed = 50.0f;
	this->collDist = 4.0f;

	this->avoidScale = 2.0f;
	this->alignScale = 0.25f;
	this->centerScale = 0.2f;

	this->attractDist = 5.0f;
	this->attractPull = 2.0f;
	this->attractPush = 2.0f;

	// Boid's spawner (attractor)
	this->numBoids = 100;
	this->boidModel = nullptr;
	this->objName = "Eagle.fbx";
	this->spawnPos = WORLD_POS;
	spawnPos.y = 100.0f;
	this->spawnRadius = 20.0f;
	this->spawnDelay = 0.0f;

	this->xPhase = 0.2f;
	this->zPhase = 0.2f;
}

cBoids::~cBoids()
{

}

void cBoids::InitBoids()
{
	if (!boidModel)
	{
		this->boidModel = g_pModelManager->FindDrawInfoByModelName(objName);
	}
	
	if (!boidModel)
	{
		g_pModelManager->SetBasePath("../../extern/assets/models");
		this->boidModel = g_pModelManager->LoadModelIntoVAO("Eagle.fbx", g_ShaderID);
	}

	if (!boidModel)
	{
		return;
	}

	cObject3D* spawner = new cObject3D();
	spawner->friendlyName = "Boid" + std::to_string(0);
	spawner->BindModelToObject(boidModel);
	spawner->SetPosition(spawnPos);
	spawner->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));
	spawner->bHasColor = false;
	spawner->SetColor(glm::vec4(0.0f,0.0f,0.0f,1.0f));
	spawner->bHasLight = true;
	spawner->bIsVisible = true;
	g_vpObjectToDraw.push_back(spawner);

	this->spawnerObj = spawner;

	for (int i = 1; i <= numBoids; i++)
	{
		glm::vec3 velocity = glm::sphericalRand(1.0f) * boidSpeed;
		glm::vec3 position = spawnPos + glm::ballRand(spawnRadius);
		glm::vec4 color = glm::abs(glm::vec4(glm::ballRand(1.0f), 1.0f));
		glm::quat direction = glm::normalize(velocity);

		cObject3D* boidObj = new cObject3D();
		boidObj->friendlyName = "Boid" + std::to_string(i);
		boidObj->BindModelToObject(boidModel);
		boidObj->SetPosition(position);
		boidObj->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));
		boidObj->SetQuatOrientation(direction);
		boidObj->bHasColor = false;
		boidObj->SetColor(color);
		boidObj->bHasLight = true;
		boidObj->bIsVisible = true;
		g_vpObjectToDraw.push_back(boidObj);

		sBoid boid;
		boid.boidObj = boidObj;
		boid.velocity = velocity;
		m_vpBoids.push_back(boid);
	}
}

void cBoids::UpdateBoids(float deltaTime)
{
	if (!boidModel)
	{
		return;
	}

	this->m_deltaTime = deltaTime;

	this->m_MoveAttractor();

	int boidsSize = m_vpBoids.size();

	if (boidsSize == 0)
	{
		return;
	}

	glm::vec3 avgPos = glm::vec3(0.0f);
	glm::vec3 avgVel = glm::vec3(0.0f);

	for (int i = 0; i < boidsSize; i++)
	{
		avgPos += m_vpBoids[i].boidObj->GetPosition();
		avgVel += m_vpBoids[i].velocity;
	}

	avgPos /= boidsSize;
	avgVel /= boidsSize;

	// calculate velocity and position each boid
	for (int i = 0; i < boidsSize; i++)
	{
		float numOfCloseBoids = 0.0f;
		glm::vec3 avgClosePos = glm::vec3(0.0f);
		glm::vec3 posA = m_vpBoids[i].boidObj->GetPosition();

		for (int j = 0; j < boidsSize; j++)
		{
			if (i != j)
			{
				glm::vec3 posB = m_vpBoids[j].boidObj->GetPosition();
				float dist = glm::distance(posA, posB);

				if (dist < collDist)
				{
					avgClosePos += posB;
					numOfCloseBoids++;
				}
			}
		}

		if (numOfCloseBoids != 0)
		{
			avgClosePos /= numOfCloseBoids;
		}

		// avoid other neighbor boids		
		glm::vec3 velAvoid = glm::vec3(0.0f);

		if (avgClosePos != glm::vec3(0.0f)) 
		{
			velAvoid = posA - avgClosePos;
			velAvoid = glm::normalize(velAvoid);
			velAvoid *= boidSpeed;
		}

		// align boid speed
		glm::vec3 velAlign = avgVel;

		if (velAlign != glm::vec3(0.0f))
		{
			velAlign = glm::normalize(velAlign);
			velAlign *= boidSpeed;
		}

		// center all boids
		glm::vec3 velCenter = glm::vec3(0.0f);

		if (avgPos != glm::vec3(0.0f)) 
		{
			velCenter = avgPos - posA;
			velCenter = glm::normalize(velCenter);
			velCenter *= boidSpeed;
		}

		// attract boids to the spawn object
		glm::vec3 velAttract = glm::vec3(0.0f);
		bool bAttracted = false;

		if (glm::distance(spawnPos, posA) > attractDist)
		{
			velAttract = spawnPos - posA;
			velAttract = glm::normalize(velAttract);
			velAttract *= boidSpeed;
			bAttracted = true;
		}
		
		// apply all velocities
		glm::vec3 tempVel = m_vpBoids[i].velocity;

		if (velAvoid != glm::vec3(0.0f))
		{
			tempVel = glm::mix(tempVel, velAvoid, avoidScale * m_deltaTime);
		}
		else 
		{
			if (velAlign != glm::vec3(0.0f))
			{
				tempVel = glm::mix(tempVel, velAlign, alignScale * m_deltaTime);
			}
			if (velCenter != glm::vec3(0.0f)) 
			{
				tempVel = glm::mix(tempVel, velCenter, centerScale * m_deltaTime);
			}
			if (velAttract != glm::vec3(0.0f)) 
			{
				if (bAttracted) 
				{
					tempVel = glm::mix(tempVel, velAttract, attractPull * m_deltaTime);
				}
				else 
				{
					tempVel = glm::mix(tempVel, -velAttract, attractPush * m_deltaTime);
				}
			}
		}

		tempVel = glm::normalize(tempVel) * boidSpeed;

		m_vpBoids[i].velocity = tempVel;

		glm::vec3 oldPos = posA;
		posA += tempVel * m_deltaTime;
		m_vpBoids[i].boidObj->SetPosition(posA);

		glm::vec3 dir = glm::normalize(posA - oldPos);
		m_vpBoids[i].boidObj->SetQuatOrientation(glm::quat(dir));
	}
}

void cBoids::SetBoidModel(cModel* boid)
{
	this->boidModel = boid;
}

void cBoids::m_MoveAttractor()
{
	glm::vec3 oldPos = spawnPos;
	glm::vec3 scale = glm::vec3(0.5f);

	glm::vec3 dir = glm::vec3(0.0f);

	dir.x = glm::cos(xPhase * g_pTime->GetGLFWTime());
	dir.z = glm::sin(zPhase * g_pTime->GetGLFWTime());
	dir = glm::normalize(dir);

	spawnPos += dir * boidSpeed * m_deltaTime;

	glm::vec3 direction = glm::normalize(oldPos - spawnPos);
	spawnerObj->SetPosition(spawnPos);
	spawnerObj->SetQuatOrientation(glm::quat(direction));

}
