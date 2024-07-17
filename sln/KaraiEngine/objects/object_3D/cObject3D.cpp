#include "cObject3D.h"
#include "../../global/global.h"

#include "../../graphics/models/cModelManager.h"
extern cModelManager* g_pModelManager;

#include <iostream>
#include <glm/gtc/type_ptr.hpp> 

unsigned int cObject3D::m_nextUniqueID = cObject3D::FIRST_UNIQUE_ID;

cObject3D::cObject3D()
{
	this->m_scale = glm::vec3(1.0f);
	this->m_position = glm::vec3(0.0f);
	this->m_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->SetOrientationFromEuler(glm::vec3(0.0f, 0.0f, 0.0f));

	this->m_opacity = 1.0f;

	this->bHasColor = false;
	this->bHasLight = false;
	this->bIsVisible = true;
	this->bIsTwoSided = false;
	this->bIsWireframe = false;

	this->m_pModel = nullptr;
	this->m_pPhysBody = nullptr;
	this->m_pAnimator = nullptr;

	this->m_isModelUpdated = false;
	m_matModel = glm::mat4(1.0f);

	this->m_UniqueID = cObject3D::m_nextUniqueID;
	cObject3D::m_nextUniqueID++;

	this->m_pPhysBody = new cPhysicsBody();
	this->m_pGameplay = new sGameplay();
}

cObject3D::~cObject3D()
{
	if (m_pPhysBody)
	{
		delete m_pPhysBody;
	}
	if (m_pAnimator)
	{
		delete m_pAnimator;
	}
	if (m_pGameplay)
	{
		delete m_pGameplay;
	}
}

void cObject3D::SetScale(const glm::vec3& scale)
{
	this->m_scale = scale;
	this->m_isModelUpdated = false;
	return;
}
glm::vec3 cObject3D::GetScale() const
{
	return this->m_scale;
}

void cObject3D::SetPosition(const glm::vec3& newPosition)
{	
	this->m_position = newPosition;
	this->m_pPhysBody->SetPosition(m_position);
	this->m_isModelUpdated = false;
	
	return;
}
glm::vec3 cObject3D::GetPosition(void)
{
	if (m_pPhysBody->GetBodyType() == cPhysicsBody::DYNAMIC)
	{
		this->m_position = m_pPhysBody->GetPosition();
		this->m_isModelUpdated = false;
	}

	return this->m_position;
}

void cObject3D::SetQuatOrientation(const glm::quat& quatAngle)
{
	this->m_quatOrientation = quatAngle;
	this->m_isModelUpdated = false;
	return;
}
glm::quat cObject3D::GetQuatOrientation() const
{
	return this->m_quatOrientation;
}

glm::vec3 cObject3D::GetDirection() const
{
	glm::vec3 dir = m_quatOrientation * glm::vec3(0.0f, 0.0f, 1.0f);
	return dir;
}
glm::vec3 cObject3D::GetRight() const
{
	glm::vec3 dir = m_quatOrientation * glm::vec3(1.0f, 0.0f, 0.0f);
	return dir;
}
glm::vec3 cObject3D::GetUp() const
{
	glm::vec3 dir = m_quatOrientation * glm::vec3(0.0f, 1.0f, 0.0f);
	return dir;
}

void cObject3D::SetOrientationFromEuler(const glm::vec3& eulerAngle)
{
	this->m_quatOrientation = glm::quat(eulerAngle);
	this->m_isModelUpdated = false;
	return;
}
glm::vec3 cObject3D::GetEulerOrientation() const
{
	return glm::eulerAngles(this->m_quatOrientation);
}
void cObject3D::AdjustAngleFromEuler(const glm::vec3& eulerAngle)
{
	glm::quat change = glm::quat(eulerAngle);
	this->m_quatOrientation *= change;
	this->m_isModelUpdated = false;
	return;
}


void cObject3D::SetColor(glm::vec4 color)
{
	this->m_color.r = color.r;
	this->m_color.g = color.g;
	this->m_color.b = color.b;
	this->m_color.a = 1.0f;
	return;
}
glm::vec4 cObject3D::GetColor() const
{
	return this->m_color;
}


void cObject3D::SetOpacity(float alphaOpacity)
{
	this->m_opacity = alphaOpacity;
	return;
}
float cObject3D::GetOpacity() const
{
	return this->m_opacity;
}


void cObject3D::BindModelToObject(const std::string& modelName)
{
	m_pModel = g_pModelManager->FindDrawInfoByModelName(modelName);
}
void cObject3D::BindModelToObject(cModel* model)
{
	m_pModel = model;
}
cModel* cObject3D::GetModel()
{
	if (this->m_pModel == nullptr)
	{
		return nullptr;
	}

	return this->m_pModel;
}

cPhysicsBody* cObject3D::GetPhysBody()
{
	return this->m_pPhysBody;
}

int cObject3D::GetUniqueID(void) const
{
	return this->m_UniqueID;
}

glm::mat4 cObject3D::GetMatModel()
{
	if (m_pPhysBody->GetPhysType() == cPhysicsBody::SOFT)
	{
		return glm::mat4(1.0f);
	}
	if (this->m_isModelUpdated)
	{
		return this->m_matModel;
	}

	glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f), m_position);

	glm::mat4 matRotation = glm::mat4(m_quatOrientation);

	glm::mat4 matScale = glm::scale(glm::mat4(1.0f), m_scale);

	this->m_matModel = glm::mat4(1.0f) * matTranslate * matRotation * matScale;

	this->m_isModelUpdated = true;

	return this->m_matModel;
}

sGameplay* cObject3D::GetGameplayComponent()
{
	return this->m_pGameplay;
}


void cObject3D::SetAnimator(cAnimator* animator)
{
	this->m_pAnimator = animator;
}
cAnimator* cObject3D::GetAnimator() const
{
	return this->m_pAnimator;
}
