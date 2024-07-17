#pragma once

#include "physics_world/cAABBsWorld.h"
#include "physics_basic/cIntersections.h"

class cPhysicsManager
{
public:
	~cPhysicsManager();
	static cPhysicsManager* GetPhysicsManager();

	void Update(float deltaTime);

	void AddPhysObject(cObject3D* object);

	void RemovePhysObject(cObject3D* object);

	void CreateAABBdebSpheres();

private:
	cPhysicsManager();
	static cPhysicsManager* m_pPhysicsManager;

	void m_UpdatePhysics(float deltaTime);

	void m_CheckIntersections();

	void m_UpdateObjectsInAABB();

	void m_GetTerrainCollision(cObject3D* obj, float deltaTime);

	void m_GetTerrainCollision_Soft(cObject3D* obj, float deltaTime);

	void m_CollisionEvent(cObject3D* pObjA, cObject3D* pObjB, sCollisionInfo& collInfo);

	void m_CollisionEvent_Soft(cObject3D* pObjA, cObject3D* pObjB, sCollisionInfo& collInfo);

	float m_deltaTime;

	cAABBsWorld* m_pAABBsWorld;
	cIntersections* m_pIntersection;

	sCollisionInfo m_CollisionInfo;

	std::vector< cAABB* > m_vpDynamicAABBs;
	std::vector<cObject3D*> m_vpMeshObjects;

	static glm::vec3 XYZ_CHILD_AABB_SHIFT;
	static glm::vec3 XYZ_CHILD_AABB_EXTENT;
	static glm::vec3 XYZ_NUM_OF_CHILD_AABB;
	static bool NEGATIVE_SIDE;
};