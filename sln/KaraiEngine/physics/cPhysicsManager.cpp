#include "cPhysicsManager.h"

#include "../world/terrain/cTerrain.h"
extern cTerrain* g_pTerrain;

#include "../system/window/cConsoleLogs.h"
extern cConsoleLogs* g_pConsole;

extern cObject3D* g_pFindMeshByFriendlyName(std::string friendlyNameToFind);

glm::vec3 cPhysicsManager::XYZ_CHILD_AABB_SHIFT = glm::vec3(64.0f, 0.0f, 64.0f);
glm::vec3 cPhysicsManager::XYZ_CHILD_AABB_EXTENT = glm::vec3(10.0f);
glm::vec3 cPhysicsManager::XYZ_NUM_OF_CHILD_AABB = glm::vec3(12.0f,5.0f,16.0f); //80x50, 10x50, 80x50
bool cPhysicsManager::NEGATIVE_SIDE = false;

cPhysicsManager* cPhysicsManager::m_pPhysicsManager = NULL;

cPhysicsManager* cPhysicsManager::GetPhysicsManager()
{
	if (cPhysicsManager::m_pPhysicsManager == NULL)
	{
		cPhysicsManager::m_pPhysicsManager = new cPhysicsManager();
	}
	return cPhysicsManager::m_pPhysicsManager;
}

cPhysicsManager::cPhysicsManager()
{
	this->m_pIntersection = new cIntersections();
	this->m_pAABBsWorld = new cAABBsWorld(XYZ_NUM_OF_CHILD_AABB, XYZ_CHILD_AABB_EXTENT, XYZ_CHILD_AABB_SHIFT, NEGATIVE_SIDE);
	this->m_pAABBsWorld->PrepareAABBScene();
}

cPhysicsManager::~cPhysicsManager()
{
	delete m_pAABBsWorld;
	delete m_pIntersection;
}

void cPhysicsManager::AddPhysObject(cObject3D* object)
{
	if (object->GetPhysBody()->GetBodyType() == cPhysicsBody::STATIC)
	{
		return;
	}

	if (object->GetPhysBody()->GetBodyType() == cPhysicsBody::DYNAMIC &&
		object->GetPhysBody()->GetCollType() == cPhysicsBody::MESH)
	{
		m_vpMeshObjects.push_back(object);
		return;
	}

	cAABB* dynamicAABB = nullptr;
	dynamicAABB = this->m_pAABBsWorld->AddObjectToAABBScene(object);

	if (dynamicAABB != nullptr)
	{
		this->m_vpDynamicAABBs.push_back(dynamicAABB);
		g_pConsole->cMessage("Number of dynamic AABB: " + std::to_string(m_vpDynamicAABBs.size()));
	}

	return;
}

void cPhysicsManager::RemovePhysObject(cObject3D* object)
{
	if (!object) return;

	for (int i = 0; i < m_vpDynamicAABBs.size(); i++)
	{
		for (int k = 0; k < m_vpDynamicAABBs[i]->vecDynamicObjects.size(); k++)
		{
			if (m_vpDynamicAABBs[i]->vecDynamicObjects[k] == object)
			{
				std::vector<cObject3D*>::iterator it;
				it = m_vpDynamicAABBs[i]->vecDynamicObjects.begin() + k;
				m_vpDynamicAABBs[i]->vecDynamicObjects.erase(it);
				break;
			}
		}
	}

	return;
}

void cPhysicsManager::Update(float deltaTime)
{
	this->m_deltaTime = deltaTime;

	this->m_CheckIntersections();

	this->m_UpdatePhysics(deltaTime);
}

void cPhysicsManager::CreateAABBdebSpheres()
{
	m_pAABBsWorld->CreateAABBdebSpheres();
}

void cPhysicsManager::m_CheckIntersections()
{
	m_pIntersection->ClearAllIntersections();

	this->m_UpdateObjectsInAABB();

	for (cAABB* pAABB : this->m_vpDynamicAABBs)
	{
		unsigned int index;

		for (index = 0; index < pAABB->vecDynamicObjects.size() - 1; index++)
		{
			cObject3D* pObjA = pAABB->vecDynamicObjects[index];

			for (unsigned int k = index + 1; k < pAABB->vecDynamicObjects.size(); k++)
			{
				cObject3D* pObjB = pAABB->vecDynamicObjects[k];

				if (m_pIntersection->ObjectsIntersection(pObjA, pObjB, m_CollisionInfo))
				{
					this->m_CollisionEvent(pObjA, pObjB, m_CollisionInfo);
				}
			}
		}

		for (index = 0; index < pAABB->vecDynamicObjects.size(); index++)
		{
			cObject3D* pObjA = pAABB->vecDynamicObjects[index];

			for (unsigned int k = index; k < pAABB->vecKinematicObjects.size(); k++)
			{
				cObject3D* pObjB = pAABB->vecKinematicObjects[k];

				if (m_pIntersection->ObjectsIntersection(pObjA, pObjB, m_CollisionInfo))
				{
					this->m_CollisionEvent(pObjA, pObjB, m_CollisionInfo);
				}
			}
		}

		for (index = 0; index < pAABB->vecDynamicObjects.size(); index++)
		{
			cObject3D* pObjA = pAABB->vecDynamicObjects[index];
			pObjA->GetPhysBody()->SetOnTheGroundFlag(false);

			for (int k = 0; k < m_vpMeshObjects.size(); k++)
			{
				cObject3D* pObjB = m_vpMeshObjects[k];

				if (m_pIntersection->ObjectsIntersection(pObjA, pObjB, m_CollisionInfo))
				{
					this->m_CollisionEvent_Soft(pObjA, pObjB, m_CollisionInfo);
					pObjA->GetPhysBody()->SetOnTheGroundFlag(true);
				}
			}
		}
	}
}

void cPhysicsManager::m_UpdatePhysics(float deltaTime)
{
	for (cAABB* pAABB : this->m_vpDynamicAABBs)
	{
		for (cObject3D* pPhysObject : pAABB->vecDynamicObjects)
		{
			if (!pPhysObject->bIsVisible)
			{
				continue;
			}
			if (pPhysObject->GetPhysBody()->GetPhysType() == cPhysicsBody::SOFT)
			{
				this->m_GetTerrainCollision_Soft(pPhysObject, deltaTime);
			}
			else
			{
				this->m_GetTerrainCollision(pPhysObject, deltaTime);
			}

			pPhysObject->GetPhysBody()->UpdateBody(deltaTime);
		}
	}

	for (int i = 0; i < m_vpMeshObjects.size(); i++)
	{
		this->m_GetTerrainCollision(m_vpMeshObjects[i], deltaTime);
		m_vpMeshObjects[i]->GetPhysBody()->UpdateBody(deltaTime);
	}
}

void cPhysicsManager::m_CollisionEvent(cObject3D* pObjA, cObject3D* pObjB, sCollisionInfo& collInfo)
{
	// Resolve collision
	glm::vec3 rv = pObjB->GetPhysBody()->GetVelocity() - pObjA->GetPhysBody()->GetVelocity();

	float velAlongNormal = glm::dot(rv, m_CollisionInfo.point.normal);
	//if (velAlongNormal > 0) return;

	float e = glm::min(pObjA->GetPhysBody()->GetRestitution(), pObjB->GetPhysBody()->GetRestitution());

	float j = -(1 + e) * velAlongNormal;
	j /= pObjA->GetPhysBody()->GetInvMass() + pObjB->GetPhysBody()->GetInvMass();
	glm::vec3 impulse = j * m_CollisionInfo.point.normal;

	float mass_sum = pObjA->GetPhysBody()->GetMass() + pObjB->GetPhysBody()->GetMass();

	if (mass_sum <= 0.0f)
	{
		return;
	}

	if (pObjA->GetPhysBody()->GetMass() <= 0.1f)
	{
		float ratio = pObjA->GetPhysBody()->GetMass() / mass_sum;
		glm::vec3 impulseA = -impulse * ratio;
		pObjA->GetPhysBody()->AddCollisionForce(impulseA);
	}
	if (pObjB->GetPhysBody()->GetMass() <= 0.1f)
	{
		float ratio = pObjB->GetPhysBody()->GetMass() / mass_sum;
		glm::vec3 impulseB = impulse * ratio;
		pObjB->GetPhysBody()->AddCollisionForce(impulseB);
	}

	// Positional correction
	const float percent = 5.0f; // 2.0f
	const float slop = 0.1f; // 0.1f
	glm::vec3 correction = glm::max(m_CollisionInfo.point.penetration - slop, 0.0f) *
		(pObjA->GetPhysBody()->GetMass() + pObjB->GetPhysBody()->GetMass()) / percent * m_CollisionInfo.point.normal;

	glm::vec3 corrImpulse = -correction / m_deltaTime / m_deltaTime;
	pObjA->GetPhysBody()->AddCollisionForce(corrImpulse);
	

	corrImpulse = -corrImpulse;
	pObjB->GetPhysBody()->AddCollisionForce(corrImpulse);
}

void cPhysicsManager::m_CollisionEvent_Soft(cObject3D* pObjA, cObject3D* pObjB, sCollisionInfo& collInfo)
{
	if (pObjA->GetPhysBody()->GetPhysType() == cPhysicsBody::SOFT &&
		pObjA->GetPhysBody()->GetCollType() == cPhysicsBody::SPHERE)
	{
		// Positional correction
		const float percent = 2.0f;
		const float slop = 0.1f;
		glm::vec3 correction = glm::max(m_CollisionInfo.point.penetration - slop, 0.0f) *
			(pObjA->GetPhysBody()->GetMass() + pObjB->GetPhysBody()->GetMass()) / percent * m_CollisionInfo.point.normal;

		//glm::vec3 corrImpulse = -correction / m_deltaTime / m_deltaTime;
		//pObjA->GetPhysBody()->AddCollisionForce(corrImpulse);

		//corrImpulse = -corrImpulse;
		//pObjB->GetPhysBody()->AddCollisionForce(corrImpulse);

		cSoftBody* softBody = pObjA->GetPhysBody()->GetSoftBody();

		for (cSoftBody::sSoftMesh* softMesh : softBody->m_vecSoftMeshes)
		{
			for (cSoftBody::sParticle* particle : softMesh->vecParticles)
			{
				float distanceToSphere = glm::distance(particle->position, m_CollisionInfo.point.closestPoint);
				if (distanceToSphere < 0.1f)
				{
					particle->position -= correction;
				}
			}
		}
	}

	if (pObjB->GetPhysBody()->GetPhysType() == cPhysicsBody::SOFT &&
		pObjB->GetPhysBody()->GetCollType() == cPhysicsBody::MESH)
	{
		cSoftBody* softBody = pObjB->GetPhysBody()->GetSoftBody();

		for (cSoftBody::sSoftMesh* softMesh : softBody->m_vecSoftMeshes)
		{
			for (cSoftBody::sParticle* particle : softMesh->vecParticles)
			{
				glm::vec3 spherePos = pObjA->GetPosition();
				float radius = static_cast<cPhysicsBody::sSphereCollider*>(pObjA->GetPhysBody()->GetCollider())->collRadius;

				float distanceToSphere = glm::distance(particle->position, spherePos);
				if (distanceToSphere < radius)
				{
					glm::vec3 particleToCentreRay = particle->position - radius;
					particleToCentreRay = glm::normalize(particleToCentreRay);
					particle->position += (particleToCentreRay * radius) + radius;
				}
			}
		}
	}
}

void cPhysicsManager::m_GetTerrainCollision(cObject3D* obj, float deltaTime)
{
	cPhysicsBody* physBody = obj->GetPhysBody();

	glm::vec3 objPos = obj->GetPosition();

	float radius = 0.0f;
	if (physBody->GetCollType() == cPhysicsBody::SPHERE)
	{
		radius = static_cast<cPhysicsBody::sSphereCollider*>(physBody->GetCollider())->collRadius;
	}
	if (physBody->GetCollType() == cPhysicsBody::MESH)
	{
		radius = static_cast<cPhysicsBody::sMeshCollider*>(physBody->GetCollider())->collRadius;
	}
	
	glm::vec3 normal = glm::vec3(0.0f);
	float height = g_pTerrain->GetTerrainHight(objPos.x, objPos.z, normal);
	float distance = glm::distance(glm::vec3(objPos.x, height, objPos.z), objPos);

	if (objPos.y < height)
	{
		float terrainInvMass = 0.0f;
		float terrainRestitution = 0.0f;
		glm::vec3 terrainVelocity = glm::vec3(0.0f);

		int impulseSize = 0;
		glm::vec3 impulse = glm::vec3(0.0f);

		g_pTerrain->GetPhysicsTerrainData(terrainInvMass, terrainRestitution);

		objPos.y = height;
		obj->SetPosition(objPos);
		physBody->SetOnTheGroundFlag(true);

		float penetration = radius - distance;
		glm::vec3 rv = terrainVelocity - physBody->GetVelocity();
		float velAlongNormal = glm::dot(rv, normal);

		float e = glm::min(obj->GetPhysBody()->GetRestitution(), terrainRestitution);

		float j = -e * velAlongNormal;
		j /= obj->GetPhysBody()->GetInvMass() + terrainInvMass;
		impulse = j * normal;

		physBody->AddCollisionForce(impulse);
	}
}

void cPhysicsManager::m_GetTerrainCollision_Soft(cObject3D* obj, float deltaTime)
{
	cPhysicsBody* physBody = obj->GetPhysBody();

	glm::vec3 objPos = obj->GetPosition();

	float radius = 0.0f;
	if (physBody->GetCollType() == cPhysicsBody::SPHERE)
	{
		radius = static_cast<cPhysicsBody::sSphereCollider*>(physBody->GetCollider())->collRadius;
	}
	if (physBody->GetCollType() == cPhysicsBody::MESH)
	{
		radius = static_cast<cPhysicsBody::sMeshCollider*>(physBody->GetCollider())->collRadius;
	}

	glm::vec3 normal = glm::vec3(0.0f);
	float height = g_pTerrain->GetTerrainHight(objPos.x, objPos.z, normal);
	float distance = glm::distance(glm::vec3(objPos.x, height, objPos.z), objPos);

	if (distance < radius || objPos.y < height)
	{
		float terrainInvMass = 0.0f;
		float terrainRestitution = 0.0f;
		glm::vec3 terrainVelocity = glm::vec3(0.0f);

		int impulseSize = 0;
		glm::vec3 impulse = glm::vec3(0.0f);

		g_pTerrain->GetPhysicsTerrainData(terrainInvMass, terrainRestitution);

		float penetration = radius - distance;
		glm::vec3 rv = terrainVelocity - physBody->GetVelocity();
		float velAlongNormal = glm::dot(rv, normal);

		for (cSoftBody::sSoftMesh* softMesh : physBody->GetSoftBody()->m_vecSoftMeshes)
		{
			for (cSoftBody::sParticle* particle : softMesh->vecParticles)
			{
				glm::vec3 normal = glm::vec3(0.0f);
				glm::vec3 particlePos = particle->position;
				float height = g_pTerrain->GetTerrainHight(particlePos.x, particlePos.z, normal);

				if (particlePos.y < height)
				{
					particle->position.y = height + 0.1f;
					physBody->SetOnTheGroundFlag(true);
				}

				float velAlongNormal = glm::dot(rv, normal);
				float e = glm::min(0.5f, terrainRestitution);
				float j = -(1 + e) * velAlongNormal;
				j /= obj->GetPhysBody()->GetInvMass() + terrainInvMass;
				impulse += j * normal;
				impulseSize++;
			}
		}
		if (impulseSize != 0)
		{
			impulse = -impulse / static_cast<float>(impulseSize);
		}
		else
		{
			impulse = glm::vec3(0.0f);
		}

		physBody->AddCollisionForce(impulse);
	}
}

void cPhysicsManager::m_UpdateObjectsInAABB()
{
	for (int i = 0; i < m_vpDynamicAABBs.size(); i++)
	{
		for (int j = 0; j < m_vpDynamicAABBs[i]->vecDynamicObjects.size(); j++)
		{
			glm::vec3 objPos = m_vpDynamicAABBs[i]->vecDynamicObjects[j]->GetPosition();
			cAABB* newAABB = m_pAABBsWorld->GetAABBFromPos(objPos);

			if (newAABB == m_vpDynamicAABBs[i])
			{
				continue;
			}

			this->AddPhysObject(m_vpDynamicAABBs[i]->vecDynamicObjects[j]);
			
			std::vector<cObject3D*>::iterator iter = m_vpDynamicAABBs[i]->vecDynamicObjects.begin();

			m_vpDynamicAABBs[i]->vecDynamicObjects.erase(iter + j);
		}

		if (m_vpDynamicAABBs[i]->vecDynamicObjects.size() == 0)
		{
			std::vector<cAABB*>::iterator iter = m_vpDynamicAABBs.begin();

			m_vpDynamicAABBs.erase(iter + i);
		}
	}
}