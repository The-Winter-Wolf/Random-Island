#pragma once

#include "cSoftBody.h"

#include <glm/glm.hpp>
#include <string>

class cPhysicsBody
{
public:
	enum eBodyType
	{
		STATIC,
		DYNAMIC,
		KINEMATIC
	};
	enum ePhysicsType
	{
		RIGID,
		SOFT
	};
	enum eCollType
	{
		SPHERE,
		CAPSULE,
		AABB,
		PLANE,
		MESH,
		NONE
	};

	cPhysicsBody();
	~cPhysicsBody();

	struct sSphereCollider
	{
		float collRadius = 0.0f;
		glm::vec3 shift = glm::vec3(0.0f);
	};
	struct sAABBCollider
	{
		float collRadius = 0.0f;
		glm::vec3 minPoint;
		glm::vec3 maxPoint;
		glm::vec3 halfPoint;
		glm::vec3 GetHalfSize(void) const;
	};
	struct sMeshCollider
	{
		float collRadius = 0.0f;
		std::string meshName;
	};

	void SetAABBCollider(const glm::vec3& minPoint, const glm::vec3& maxPoint, float radius = 0.0f);
	void SetMeshCollider(const std::string& meshName, float radius = 0.0f);
	void SetSphereCollider(float radius, glm::vec3 shift = glm::vec3());
	eCollType GetCollType(void) const;
	void* GetCollider(void) const;

	void SetBodyType(eBodyType bodyType);
	eBodyType GetBodyType(void) const;

	cModel* SetSoftBody(const std::string& oldModel, const glm::mat4& matModel);
	void SetRigidBody(eBodyType bodyType);
	ePhysicsType GetPhysType(void) const;
	cSoftBody* GetSoftBody(void) const;

	void SetOnTheGroundFlag(bool true_false);
	bool GetOnTheGroundFlag(void) const;

	void SetHasCollisionFlag(bool true_false);
	bool GetHasCollisionFlag(void) const;

	void UpdateBody(float deltaTime);

	void SetAutoSlowdown(float multiX, float multiY, float multiZ);
	void Slowdown(float multiX, float multiY, float multiZ);
	void Slowdown(glm::vec3 multiXYZ);

	glm::vec3 GetVelocity(void) const;
	glm::vec3 GetSoftCenterPos(void);

	void SetRestitution(float rest);
	float GetRestitution(void) const;

	void SetMass(float mass);
	float GetInvMass(void) const;
	float GetMass(void) const;

	void AddCollisionAccel(const glm::vec3& collAccel);
	void SetConstAccel(const glm::vec3& constAccel);
	void AddUserAccel(const glm::vec3& userAccel);
	void SetGravity(const glm::vec3& gravity);
	glm::vec3 GetResultAccel(void) const;

	void AddCollisionForce(const glm::vec3& collForce);
	void SetConstForce(const glm::vec3& constForce);
	void AddUserForce(const glm::vec3& userForce);
	glm::vec3 GetResultForce(void) const;

	void SetPosition(const glm::vec3& pos);
	glm::vec3 GetPosition(void);

private:
	float m_mass;
	float m_inv_mass;
	float m_restitution;
	glm::vec3 m_slowdownIndex;

	void* m_pCollider;
	cSoftBody* m_pSoftBody;

	glm::vec3 m_gravity;
	glm::vec3 m_collAccel;
	glm::vec3 m_userAccel;
	glm::vec3 m_constAccel;

	glm::vec3 m_position;
	glm::vec3 m_velocity;

	float m_timer1;

	bool bFirstStep;
	bool bOntheGround;
	bool bHasCollision;
	bool bIsAutoSlowdown;

	eBodyType m_bodyType;
	eCollType m_collType;
	ePhysicsType m_physicsType;

	void m_ResetAccel(void);
};
