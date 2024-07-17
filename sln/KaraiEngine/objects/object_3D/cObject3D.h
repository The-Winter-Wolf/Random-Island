#pragma once

#include "../../physics/physics_body/cPhysicsBody.h"
#include "../../graphics/models/cModel.h"
#include "../../animations/cAnimator.h"

#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <string>
#include <vector>

struct sGameplay
{
	bool bIsHit = false;
	bool bIsDead = false;
	bool bImmortal = false;
	bool bIsPlayer = false;
	float Damage = 0.0f;
	float Health = 100.0f;
	float Stamina = 100.0f;
	float Mana = 100.0f;
	float Exp = 0.0f;
};

class cObject3D
{
public:
	cObject3D();
	~cObject3D();

	std::string friendlyName;

	void SetScale(const glm::vec3& scale);
	glm::vec3 GetScale() const;

	void SetPosition(const glm::vec3& newPosition);
	glm::vec3 GetPosition();

	void SetQuatOrientation(const glm::quat& quatAngle);
	glm::quat GetQuatOrientation() const;

	void SetOrientationFromEuler(const glm::vec3& eulerAngle);
	glm::vec3 GetEulerOrientation() const;

	void AdjustAngleFromEuler(const glm::vec3& eulerAngle);

	glm::vec3 GetDirection() const;
	glm::vec3 GetRight() const;
	glm::vec3 GetUp() const;

	void SetColor(glm::vec4 color);
	glm::vec4 GetColor() const;

	void SetOpacity(float alphaOpacity);
	float GetOpacity() const;

	void BindModelToObject(const std::string& modelName);
	void BindModelToObject(cModel* model);
	cModel* GetModel();

	void SetAnimator(cAnimator* animator);
	cAnimator* GetAnimator() const;

	cPhysicsBody* GetPhysBody();

	int GetUniqueID(void) const;

	glm::mat4 GetMatModel();

	sGameplay* GetGameplayComponent();

	bool bHasColor;
	bool bHasLight;
	bool bIsVisible;
	bool bIsTwoSided;
	bool bIsWireframe;

private:
	cModel* m_pModel;
	cPhysicsBody* m_pPhysBody;
	cAnimator* m_pAnimator;
	sGameplay* m_pGameplay;

	float m_opacity;

	glm::vec4 m_color;
	glm::vec3 m_scale;
	glm::vec3 m_position;
	glm::quat m_quatOrientation;

	glm::mat4 m_matModel;
	bool m_isModelUpdated;

	unsigned int m_UniqueID;
	static const unsigned int FIRST_UNIQUE_ID = 1000;
	static unsigned int m_nextUniqueID;
};