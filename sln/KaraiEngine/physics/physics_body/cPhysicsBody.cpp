#include "cPhysicsBody.h"
#include "../../global/global.h"

cPhysicsBody::cPhysicsBody()
{
	this->m_mass = 0.0f;
	this->m_inv_mass = 0.0f;
	this->m_restitution = 1.0f;

	this->m_gravity = glm::vec3(0.0f, -9.8f, 0.0f);
	this->m_collAccel = glm::vec3(0.0f);
	this->m_userAccel = glm::vec3(0.0f);
	this->m_constAccel = glm::vec3(0.0f);

	this->m_position = glm::vec3(0.0f);
	this->m_velocity = glm::vec3(0.0f);

	this->m_collType = NONE;
	this->m_bodyType = STATIC;
	this->m_physicsType = RIGID;

	this->m_timer1 = 0.0f;
	this->bFirstStep = true;
	this->bOntheGround = false;
	this->bHasCollision = false;

	this->m_pCollider = nullptr;
	this->m_pSoftBody = nullptr;
}

cPhysicsBody::~cPhysicsBody()
{
	if (m_pCollider != nullptr)
	{
		delete m_pCollider;
	}
	if (m_pSoftBody != nullptr)
	{
		delete m_pSoftBody;
	}
}

void cPhysicsBody::SetSphereCollider(float radius, glm::vec3 shift)
{
	sSphereCollider* sphereColl = new sSphereCollider();
	sphereColl->collRadius = radius;
	sphereColl->shift = shift;
	this->m_pCollider = static_cast<void*>(sphereColl);
	this->m_collType = SPHERE;
	return;
}
void cPhysicsBody::SetAABBCollider(const glm::vec3& minPoint, const glm::vec3& maxPoint, float radius)
{
	sAABBCollider* aabbColl = new sAABBCollider();
	aabbColl->collRadius = radius;
	aabbColl->minPoint = minPoint;
	aabbColl->maxPoint = maxPoint;
	aabbColl->halfPoint = aabbColl->GetHalfSize();
	this->m_pCollider = static_cast<void*>(aabbColl);
	this->m_collType = AABB;
	return;
}
void cPhysicsBody::SetMeshCollider(const std::string& meshName, float radius)
{
	sMeshCollider* meshColl = new sMeshCollider();
	meshColl->collRadius = radius;
	meshColl->meshName = meshName;
	this->m_pCollider = static_cast<void*>(meshColl);
	this->m_collType = MESH;
	return;
}
cPhysicsBody::eCollType cPhysicsBody::GetCollType(void) const
{
	return this->m_collType;
}
void* cPhysicsBody::GetCollider(void) const
{
	if (m_pCollider)
	{
		return this->m_pCollider;
	}
	return nullptr;
}

void cPhysicsBody::SetBodyType(eBodyType bodyType)
{
	if (m_physicsType == SOFT)
	{
		m_bodyType = DYNAMIC;
	}

	this->m_bodyType = bodyType;
}
cPhysicsBody::eBodyType cPhysicsBody::GetBodyType(void) const
{
	return this->m_bodyType;
}

cModel* cPhysicsBody::SetSoftBody(const std::string& oldModel, const glm::mat4& matModel)
{
	cSoftBody* softBody = new cSoftBody();
	this->m_pSoftBody = softBody;

	this->m_physicsType = SOFT;
	this->m_bodyType = DYNAMIC;


	return softBody->CreateSoftModel(oldModel, matModel);
}
void cPhysicsBody::SetRigidBody(eBodyType bodyType)
{
	this->m_physicsType = RIGID;
	this->m_bodyType = bodyType;

	return;
}
cPhysicsBody::ePhysicsType cPhysicsBody::GetPhysType(void) const
{
	return this->m_physicsType;
}
cSoftBody* cPhysicsBody::GetSoftBody(void) const
{
	if (m_pSoftBody)
	{
		return m_pSoftBody;
	}

	return nullptr;
}

void cPhysicsBody::SetOnTheGroundFlag(bool true_false)
{
	this->bOntheGround = true_false;
}
bool cPhysicsBody::GetOnTheGroundFlag(void) const
{
	return this->bOntheGround;
}

void cPhysicsBody::SetHasCollisionFlag(bool true_false)
{
	this->bHasCollision = true_false;

}
bool cPhysicsBody::GetHasCollisionFlag(void) const
{
	return this->bHasCollision;
}

void cPhysicsBody::SetMass(float mass)
{
	if (mass <= 0)
	{
		this->m_mass = 0;
		this->m_inv_mass = 0;
	}
	else
	{
		this->m_mass = mass;
		this->m_inv_mass = 1 / mass;
	}
}
float cPhysicsBody::GetMass(void) const
{
	return this->m_mass;
}
float cPhysicsBody::GetInvMass(void) const
{
	return this->m_inv_mass;
}

void cPhysicsBody::AddCollisionAccel(const glm::vec3& collAccel)
{
	this->m_collAccel += collAccel;
}
void cPhysicsBody::SetConstAccel(const glm::vec3& constAccel)
{
	this->m_constAccel = constAccel;
}
void cPhysicsBody::AddUserAccel(const glm::vec3& userAccel)
{
	this->m_userAccel += userAccel;
}
void cPhysicsBody::SetGravity(const glm::vec3& gravity)
{
	this->m_gravity = gravity;
}
glm::vec3 cPhysicsBody::GetResultAccel(void) const
{
	return m_gravity + m_collAccel + m_userAccel + m_constAccel;
}

void cPhysicsBody::AddCollisionForce(const glm::vec3& collForce)
{
	this->m_collAccel += collForce * m_inv_mass;
}
void cPhysicsBody::SetConstForce(const glm::vec3& constForce)
{
	this->m_constAccel = constForce * m_inv_mass;
}
void cPhysicsBody::AddUserForce(const glm::vec3& userForce)
{
	this->m_userAccel += userForce * m_inv_mass;
}
glm::vec3 cPhysicsBody::GetResultForce(void) const
{
	glm::vec3 resultAccel = m_gravity + m_collAccel + m_userAccel + m_constAccel;
	return resultAccel * m_mass;
}

void cPhysicsBody::SetPosition(const glm::vec3& pos)
{
	this->m_position = pos;
	return;
}

glm::vec3 cPhysicsBody::GetPosition(void)
{
	if (m_physicsType == SOFT)
	{
		this->m_position = m_pSoftBody->GetCenterPos();
	}

	return this->m_position;
}

glm::vec3 cPhysicsBody::GetVelocity(void) const
{
	return this->m_velocity;
}
glm::vec3 cPhysicsBody::GetSoftCenterPos(void)
{
	return this->m_pSoftBody->GetCenterPos();
}

void cPhysicsBody::SetRestitution(float rest)
{
	this->m_restitution = rest;
}
float cPhysicsBody::GetRestitution(void) const
{
	return this->m_restitution;
}

void cPhysicsBody::UpdateBody(float deltaTime)
{
	if (bOntheGround)
	{	
		this->m_gravity = glm::vec3(0.0f, 20.0f, 0.0f);
	}
	else
	{
        this->m_gravity = glm::vec3(0.0f, -20.0f, 0.0f);
	}

	glm::vec3 resultAccel = m_gravity + m_collAccel + m_userAccel + m_constAccel;
	
	this->m_velocity += resultAccel * deltaTime;

	if (m_physicsType == SOFT)
	{
		this->m_pSoftBody->UpdateSoftBody(m_velocity, deltaTime);
	}
	else
	{
		m_position += m_velocity * deltaTime;
	}

	if (bOntheGround) if (bIsAutoSlowdown) this->Slowdown(m_slowdownIndex);

	this->m_ResetAccel();

	return;
}

void cPhysicsBody::SetAutoSlowdown(float multiX, float multiY, float multiZ)
{
	this->bIsAutoSlowdown = true;
	this->m_slowdownIndex = glm::vec3(multiX, multiY, multiZ);
}
void cPhysicsBody::Slowdown(float multiX, float multiY, float multiZ)
{
	this->m_velocity.x *= multiX;
	this->m_velocity.y *= multiY;
	this->m_velocity.z *= multiZ;
}
void cPhysicsBody::Slowdown(glm::vec3 multiXYZ)
{
	this->m_velocity.x *= multiXYZ.x;
	this->m_velocity.y *= multiXYZ.y;
	this->m_velocity.z *= multiXYZ.z;
}

glm::vec3 cPhysicsBody::sAABBCollider::GetHalfSize(void) const
{
	float halfX = (maxPoint.x - minPoint.x) / 2;
	float halfY = (maxPoint.y - minPoint.y) / 2;
	float halfZ = (maxPoint.z - minPoint.z) / 2;

	return glm::vec3(halfX, halfY, halfZ);
}

void cPhysicsBody::m_ResetAccel(void)
{
	this->m_collAccel = glm::vec3(0.0f);
	this->m_userAccel = glm::vec3(0.0f);
}