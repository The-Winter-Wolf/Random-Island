#include "cIntersections.h"

#include "../../graphics/models/cModelManager.h"
extern cModelManager* g_pModelManager;

#include "../../global/global.h"
#include <iostream>

cIntersections::cIntersections()
{
}

cIntersections::~cIntersections()
{
}

bool cIntersections::ObjectsIntersection(cObject3D* A, cObject3D* B, sCollisionInfo& collInfo)
{
	if (A == B) return false;

	if (A->GetPhysBody()->GetCollType() == cPhysicsBody::NONE || 
		B->GetPhysBody()->GetCollType() == cPhysicsBody::NONE) return false;

	if (this->m_CheckPastIntersections(A, B)) return false;

	sCollisionInfo collisionInfo;
	collisionInfo.pPhysObjectA = A;
	collisionInfo.pPhysObjectB = B;

	if (collisionInfo.pPhysObjectA->GetPhysBody()->GetCollType() == cPhysicsBody::SPHERE)
	{
		if (collisionInfo.pPhysObjectB->GetPhysBody()->GetCollType() == cPhysicsBody::SPHERE)
		{
			if (! this->m_Sphere_Sphere_Intersection(collisionInfo)) return false;
		}
		else if (collisionInfo.pPhysObjectB->GetPhysBody()->GetCollType() == cPhysicsBody::AABB)
		{
			if (! this->m_Sphere_AABB_Intersection(collisionInfo)) return false;
		}
		else if (collisionInfo.pPhysObjectB->GetPhysBody()->GetCollType() == cPhysicsBody::MESH)
		{
			if (collisionInfo.pPhysObjectB->GetPhysBody()->GetPhysType() == cPhysicsBody::SOFT)
			{
				if (!this->m_Soft_Sphere_Mesh_Intersection(collisionInfo)) return false;
			}
			else
			{
				if (! this->m_Sphere_Mesh_Intersection(collisionInfo)) return false;
			}
		}

		m_vecCollisions.push_back(collisionInfo);
		collInfo = collisionInfo;

		return true;
	}

	if (collisionInfo.pPhysObjectA->GetPhysBody()->GetCollType() == cPhysicsBody::AABB)
	{
		if (collisionInfo.pPhysObjectB->GetPhysBody()->GetCollType() == cPhysicsBody::AABB)
		{
			if (! this->m_AABB_AABB_Intersection(collisionInfo)) return false;
		}
		else if (collisionInfo.pPhysObjectB->GetPhysBody()->GetCollType() == cPhysicsBody::SPHERE)
		{
			collisionInfo.pPhysObjectA = B;
			collisionInfo.pPhysObjectB = A;

			if (! this->m_Sphere_AABB_Intersection(collisionInfo)) return false;
		}

		m_vecCollisions.push_back(collisionInfo);
		collInfo = collisionInfo;

		return true;
	}

	if (collisionInfo.pPhysObjectA->GetPhysBody()->GetCollType() == cPhysicsBody::MESH)
	{
		if (collisionInfo.pPhysObjectB->GetPhysBody()->GetCollType() == cPhysicsBody::SPHERE)
		{
			collisionInfo.pPhysObjectA = B;
			collisionInfo.pPhysObjectB = A;

			if (collisionInfo.pPhysObjectB->GetPhysBody()->GetPhysType() == cPhysicsBody::SOFT)
			{
				if (!this->m_Soft_Sphere_Mesh_Intersection(collisionInfo)) return false;
			}
			else
			{
				if (! this->m_Sphere_Mesh_Intersection(collisionInfo)) return false;
			}
		}

		m_vecCollisions.push_back(collisionInfo);
		collInfo = collisionInfo;

		return true;
	}

	return false;
}

void cIntersections::ClearAllIntersections()
{
	m_vecCollisions.clear();
}

bool cIntersections::m_Sphere_Sphere_Intersection(sCollisionInfo& collisionInfo)
{
	cObject3D* pSphereA = collisionInfo.pPhysObjectA;
	cObject3D* pSphereB = collisionInfo.pPhysObjectB;

	cPhysicsBody::sSphereCollider* pSphereCollA = static_cast<cPhysicsBody::sSphereCollider*>(pSphereA->GetPhysBody()->GetCollider());
	cPhysicsBody::sSphereCollider* pSphereCollB = static_cast<cPhysicsBody::sSphereCollider*>(pSphereB->GetPhysBody()->GetCollider());

	float radii = pSphereCollA->collRadius + pSphereCollB->collRadius;
	glm::vec3 sphereAPos = glm::vec4(pSphereA->GetPosition(), 1.0f) + glm::vec4(pSphereCollA->shift, 1.0f) * pSphereA->GetMatModel();
	glm::vec3 sphereBPos = glm::vec4(pSphereB->GetPosition(), 1.0f) + glm::vec4(pSphereCollB->shift, 1.0f) * pSphereA->GetMatModel();

	glm::vec3 delta = sphereBPos - sphereAPos;
	float deltaLength = glm::distance(sphereBPos, sphereAPos);

	if (deltaLength < radii)
	{
		float penetration = radii - deltaLength;
		glm::vec3 normal = glm::normalize(delta);
		glm::vec3 localA = normal * pSphereCollA->collRadius;
		glm::vec3 localB = -normal * pSphereCollB->collRadius;

		collisionInfo.AddContactPoint(localA, localB, normal, penetration, -1, glm::vec3(0.0f));
		//pSphereA->pObject->SetColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		//pSphereB->pObject->SetColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		g_pConsole->cMessage("Sphere to Sphere");
		return true;
	}
	return false;
}

bool cIntersections::m_AABB_AABB_Intersection(sCollisionInfo& collisionInfo)
{
	cObject3D* pAABB_A = collisionInfo.pPhysObjectA;
	cObject3D* pAABB_B = collisionInfo.pPhysObjectB;

	cPhysicsBody::sAABBCollider* pAABBCollA = static_cast<cPhysicsBody::sAABBCollider*>(pAABB_A->GetPhysBody()->GetCollider());
	cPhysicsBody::sAABBCollider* pAABBCollB = static_cast<cPhysicsBody::sAABBCollider*>(pAABB_B->GetPhysBody()->GetCollider());

	glm::vec3 boxAPos = pAABB_A->GetPosition();
	glm::vec3 boxBPos = pAABB_B->GetPosition();

	glm::vec3 boxASize = pAABBCollA->GetHalfSize();
	glm::vec3 boxBSize = pAABBCollB->GetHalfSize();

	bool overlap = m_TestAABB_AABB(boxAPos, boxBPos, boxASize, boxBSize);

	if (overlap)
	{
		glm::vec3 faces[6] = { glm::vec3(-1,  0,  0),  glm::vec3(1,  0,  0),
							   glm::vec3(0, -1,  0),  glm::vec3(0,  1,  0),
							   glm::vec3(0,  0, -1),  glm::vec3(0,  0,  1) };

		glm::vec3 maxA = boxAPos + boxASize;
		glm::vec3 minA = boxAPos - boxASize;

		glm::vec3 maxB = boxBPos + boxBSize;
		glm::vec3 minB = boxBPos - boxBSize;

		float distances[6] =
		{
			(maxB.x - minA.x),
			(maxA.x - minB.x),
			(maxB.y - minA.y),
			(maxA.y - minB.y),
			(maxB.z - minA.z),
			(maxA.z - minB.z)
		};
		float penetration = FLT_MAX;
		glm::vec3 normal;

		for (int i = 0; i < 6; i++)
		{
			if (distances[i] < penetration)
			{
				penetration = distances[i];
				normal = faces[i];
			}
		}

		collisionInfo.AddContactPoint(glm::vec3(), glm::vec3(), normal, penetration, -1, glm::vec3(0.0f));
		g_pConsole->cMessage("AABB to AABB");
		return true;
	}

	return false;
}

bool cIntersections::m_Sphere_AABB_Intersection(sCollisionInfo& collisionInfo)
{
	cObject3D* pAABB = collisionInfo.pPhysObjectA;
	cObject3D* pSphere = collisionInfo.pPhysObjectB;

	cPhysicsBody::sAABBCollider* pAABBColl = static_cast<cPhysicsBody::sAABBCollider*>(pAABB->GetPhysBody()->GetCollider());
	cPhysicsBody::sSphereCollider* pSphereColl = static_cast<cPhysicsBody::sSphereCollider*>(pSphere->GetPhysBody()->GetCollider());

	glm::vec3 boxSize = pAABBColl->GetHalfSize();

	glm::vec3 delta = pSphere->GetPosition() - pAABB->GetPosition();

	glm::vec3 closestPoint = glm::clamp(delta, -boxSize, boxSize);
	glm::vec3 localPoint = delta - closestPoint;
	float distance = glm::distance(delta, closestPoint);

	if (distance < pSphereColl->collRadius)
	{
		float penetration = pSphereColl->collRadius - distance;
		glm::vec3 normal = glm::normalize(localPoint);
		glm::vec3 localA = glm::vec3();
		glm::vec3 localB = -normal * pSphereColl->collRadius;

		collisionInfo.AddContactPoint(localA, localB, normal, penetration, -1, glm::vec3(0.0f));
		g_pConsole->cMessage("AABB to Sphere");
		return true;
	}

	return false;
}

bool cIntersections::m_Sphere_Mesh_Intersection(sCollisionInfo& collisionInfo)
{
	if (!g_pModelManager) { return false; }
	if (!collisionInfo.pPhysObjectA) { return false; }
	if (!collisionInfo.pPhysObjectB) { return false; }

	cObject3D* pSphere = collisionInfo.pPhysObjectA;
	cObject3D* pMesh = collisionInfo.pPhysObjectB;

	glm::vec3 meshPos = pMesh->GetPosition();
	glm::quat meshRot = pMesh->GetQuatOrientation();
	glm::vec3 meshScale = pMesh->GetScale();

	glm::vec3 spherePos = pSphere->GetPosition();

	cPhysicsBody::sSphereCollider* pSphereColl = static_cast<cPhysicsBody::sSphereCollider*>(pSphere->GetPhysBody()->GetCollider());
	cPhysicsBody::sMeshCollider* pMeshColl = static_cast<cPhysicsBody::sMeshCollider*>(pMesh->GetPhysBody()->GetCollider());

	cModel* pTempModelPtr = g_pModelManager->FindDrawInfoByModelName(pMeshColl->meshName);

	if (pTempModelPtr == nullptr) { return false; }

	float closestDistance = FLT_MAX;
	unsigned int indexOfClosestIndices = INT_MAX;
	glm::vec3 closestPoint = glm::vec3(0.0f);
	glm::vec3 delta = glm::vec3(0.0f);
	glm::vec3 closestVertices[3] = { glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) };

	for (unsigned int k = 0; k != pTempModelPtr->vpMeshesOfModel.size(); k++)
	{
		for (unsigned int index = 0; index != pTempModelPtr->vpMeshesOfModel[k]->numberOfIndices; index += 3)
		{
			glm::vec3 verts[3] {};

			verts[0].x = pTempModelPtr->vpMeshesOfModel[k]->vertices[pTempModelPtr->vpMeshesOfModel[k]->indices[index]].position.x;
			verts[0].y = pTempModelPtr->vpMeshesOfModel[k]->vertices[pTempModelPtr->vpMeshesOfModel[k]->indices[index]].position.y;
			verts[0].z = pTempModelPtr->vpMeshesOfModel[k]->vertices[pTempModelPtr->vpMeshesOfModel[k]->indices[index]].position.z;

			verts[1].x = pTempModelPtr->vpMeshesOfModel[k]->vertices[pTempModelPtr->vpMeshesOfModel[k]->indices[index + 1]].position.x;
			verts[1].y = pTempModelPtr->vpMeshesOfModel[k]->vertices[pTempModelPtr->vpMeshesOfModel[k]->indices[index + 1]].position.y;
			verts[1].z = pTempModelPtr->vpMeshesOfModel[k]->vertices[pTempModelPtr->vpMeshesOfModel[k]->indices[index + 1]].position.z;

			verts[2].x = pTempModelPtr->vpMeshesOfModel[k]->vertices[pTempModelPtr->vpMeshesOfModel[k]->indices[index + 2]].position.x;
			verts[2].y = pTempModelPtr->vpMeshesOfModel[k]->vertices[pTempModelPtr->vpMeshesOfModel[k]->indices[index + 2]].position.y;
			verts[2].z = pTempModelPtr->vpMeshesOfModel[k]->vertices[pTempModelPtr->vpMeshesOfModel[k]->indices[index + 2]].position.z;

			glm::mat4 matModel = glm::mat4(1.0f);

			matModel = pMesh->GetMatModel();

			glm::vec4 vertsWorld[3] {};
			vertsWorld[0] = (matModel * glm::vec4(verts[0], 1.0f));
			vertsWorld[1] = (matModel * glm::vec4(verts[1], 1.0f));
			vertsWorld[2] = (matModel * glm::vec4(verts[2], 1.0f));

			closestPoint = this->m_ClosestPtPointTriangle(spherePos,
															vertsWorld[0],
															vertsWorld[1],
															vertsWorld[2]);

			float distanceToThisTriangle = glm::distance(closestPoint, spherePos);

			if (distanceToThisTriangle < closestDistance)
			{
				closestDistance = distanceToThisTriangle;

				delta = closestPoint - spherePos;

				indexOfClosestIndices = index;

				closestVertices[0] = vertsWorld[0];
				closestVertices[1] = vertsWorld[1];
				closestVertices[2] = vertsWorld[2];
			}
		}
	}

	if (closestDistance < pSphereColl->collRadius)
	{
		float penetration = pSphereColl->collRadius - closestDistance;
		glm::vec3 normal = glm::normalize(delta);
		glm::vec3 localA = normal * pSphereColl->collRadius;
		glm::vec3 localB = -glm::vec3();

		collisionInfo.AddContactPoint(localA, localB, normal, penetration, indexOfClosestIndices, closestPoint);
		g_pConsole->cMessage("Sphere to Mesh");
		return true;
	}

	return false;
}

bool cIntersections::m_CheckPastIntersections(cObject3D* A, cObject3D* B)
{
	for (unsigned int i = 0; i < m_vecCollisions.size(); i++)
	{
		if (m_vecCollisions[i].pPhysObjectA == A && m_vecCollisions[i].pPhysObjectB == B ||
			m_vecCollisions[i].pPhysObjectA == B && m_vecCollisions[i].pPhysObjectB == A)
		{
			return true;
		}
	}

	return false;
}

bool cIntersections::m_Soft_Sphere_Mesh_Intersection(sCollisionInfo& collisionInfo)
{
	if (!g_pModelManager) { return false; }
	if (!collisionInfo.pPhysObjectA) { return false; }
	if (!collisionInfo.pPhysObjectB) { return false; }

	cObject3D* pSphere = collisionInfo.pPhysObjectA;
	glm::vec3 spherePos = pSphere->GetPosition();

	cObject3D* pMesh = collisionInfo.pPhysObjectB;
	glm::vec3 meshPos = pMesh->GetPosition();

	cPhysicsBody::sSphereCollider* pSphereCollA = static_cast<cPhysicsBody::sSphereCollider*>(pSphere->GetPhysBody()->GetCollider());
	cPhysicsBody::sMeshCollider* pMeshCollB = static_cast<cPhysicsBody::sMeshCollider*>(pMesh->GetPhysBody()->GetCollider());

	if (pMeshCollB->collRadius != 0.0f)
	{
		float sumRadius = pSphereCollA->collRadius + pMeshCollB->collRadius;

		float distance = glm::distance(spherePos, meshPos);

		if (distance > sumRadius)
		{
			return false;
		}
	}
	
	float closestDistance = FLT_MAX;
	unsigned int indexOfClosestIndices = INT_MAX;
	glm::vec3 closestPoint = glm::vec3(0.0f);
	glm::vec3 delta = glm::vec3(0.0f);
	glm::vec3 closestVertices[3] = { glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) };

	for (cSoftBody::sSoftMesh* softMesh : pMesh->GetPhysBody()->GetSoftBody()->m_vecSoftMeshes)
	{
		for (unsigned int index = 0; index != softMesh->vecParticles.size(); index += 3)
		{
			glm::vec3 verts[3]{};
			
			verts[0].x = softMesh->vecParticles[index + 0]->position.x;
			verts[0].y = softMesh->vecParticles[index + 0]->position.y;
			verts[0].z = softMesh->vecParticles[index + 0]->position.z;

			verts[1].x = softMesh->vecParticles[index + 1]->position.x;
			verts[1].y = softMesh->vecParticles[index + 1]->position.y;
			verts[1].z = softMesh->vecParticles[index + 1]->position.z;

			verts[2].x = softMesh->vecParticles[index + 2]->position.x;
			verts[2].y = softMesh->vecParticles[index + 2]->position.y;
			verts[2].z = softMesh->vecParticles[index + 2]->position.z;

			closestPoint = this->m_ClosestPtPointTriangle(spherePos, verts[0], verts[1], verts[2]);

			float distanceToThisTriangle = glm::distance(closestPoint, spherePos);

			if (distanceToThisTriangle < closestDistance)
			{
				closestDistance = distanceToThisTriangle;

				delta = closestPoint - spherePos;

				indexOfClosestIndices = index;

				closestVertices[0] = verts[0];
				closestVertices[1] = verts[1];
				closestVertices[2] = verts[2];
			}
		}
	}

	if (closestDistance < pSphereCollA->collRadius)
	{
		float penetration = pSphereCollA->collRadius - closestDistance;
		glm::vec3 normal = glm::normalize(delta);
		glm::vec3 localA = normal * pSphereCollA->collRadius;
		glm::vec3 localB = -glm::vec3();
		//////////////////////////////////////////////////

		glm::vec3 pos = pMesh->GetPhysBody()->GetSoftBody()->m_vecSoftMeshes[0]->vecParticles[indexOfClosestIndices]->position;
		glm::vec3 spherePos = pSphere->GetPosition();

		///////////////////////////////////////////////////
		collisionInfo.AddContactPoint(localA, localB, normal, penetration, indexOfClosestIndices, closestPoint);
		g_pConsole->cMessage("Sphere to Soft mesh");
		return true;
	}

	return false;
}

void sCollisionInfo::AddContactPoint(const glm::vec3& localA, const glm::vec3& localB,
	const glm::vec3& normal, float p, float index, const glm::vec3& close)
{
	point.localA = localA;
	point.localB = localB;
	point.normal = normal;
	point.penetration = p;
	point.closestPoint = close;
	point.closestIndex = index;
}
