#pragma once

#include "../../objects/object_3D/cObject3D.h"

struct sContactPoint
{
	glm::vec3 localA;
	glm::vec3 localB;
	glm::vec3 normal;
	float penetration;
	glm::vec3 closestPoint;
	int closestIndex;
};

struct sCollisionInfo
{
	cObject3D* pPhysObjectA;
	cObject3D* pPhysObjectB;

	sContactPoint point;

	void AddContactPoint(const glm::vec3& localA, const glm::vec3& localB,
		const glm::vec3& normal, float p, float index, const glm::vec3& close);
};

class cIntersections
{
public:
	cIntersections();
	~cIntersections();

	bool ObjectsIntersection(cObject3D* A, cObject3D* B, sCollisionInfo& collisionInfo);

	void ClearAllIntersections();

private:
	std::vector<sCollisionInfo> m_vecCollisions;
	std::vector<sCollisionInfo>::iterator itCollisions;

	bool m_Sphere_Sphere_Intersection(sCollisionInfo& collisionInfo);

	bool m_AABB_AABB_Intersection(sCollisionInfo& collisionInfo);

	bool m_Sphere_AABB_Intersection(sCollisionInfo& collisionInfo);

	bool m_Sphere_Mesh_Intersection(sCollisionInfo& collisionInfo);

	bool m_Vert_Mesh_Intersection(glm::vec3& vert, cObject3D* physObject);

	bool m_CheckPastIntersections(cObject3D* A, cObject3D* B);

	bool m_Soft_Sphere_Mesh_Intersection(sCollisionInfo& collisionInfo);

	// Physics Basic Tests
	bool m_TestAABB_AABB(glm::vec3& posA, glm::vec3& posB, glm::vec3& halfSizeA, glm::vec3& halfSizeB);

	bool m_TestSphereTriangle(float& sphereRadius, glm::vec3& vert0, glm::vec3& vert1, glm::vec3& vert2, glm::vec3& sphereCentre);

	glm::vec3 m_ClosestPtPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c);


};

