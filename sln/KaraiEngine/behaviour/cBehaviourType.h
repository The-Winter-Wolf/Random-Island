#pragma once

#include "../objects/object_3D/cObject3D.h"

#include <glm/glm.hpp>

enum eBehaviour
{
	NONE,
	LOOK,
	SEEK,
	FLEE,
	PURSUE,
	EVADE,
	APPROACH,
	WANDER_IN_RADIUS,
	WANDER_FREE,
	PATH_FOLLOWING,
};

class cBehaviourType
{
public:
	cBehaviourType();
	~cBehaviourType();

	bool Update(float deltaTime);

	bool LookBehaviour(cObject3D* object, cObject3D* subject, float maxRotate);

	bool SeekBehaviour(cObject3D* object, cObject3D* subject, float maxSpeed, float maxRotate, float stopDist);
	bool FleeBehaviour(cObject3D* object, cObject3D* subject, float maxSpeed, float maxRotate);

	bool PursueBehaviour(cObject3D* object, cObject3D* subject, float maxSpeed, float maxRotate, float stopDist);
	bool EvadeBehaviour(cObject3D* object, cObject3D* subject, float maxSpeed, float maxRotate);

	bool ApproachBehaviour(cObject3D* object, cObject3D* subject, float maxSpeed, float maxRotate, float slowRadius);

	bool WanderFreeBehaviour(cObject3D* object, float maxSpeed, float maxRotate, float startRadius, float wanderRadius, float delayTime);
	bool WanderRadiusBehaviour(cObject3D* object, float maxSpeed, float maxRotate, float startRadius, float delayTime);

	bool PathFollowingBehaviour(cObject3D* object, const std::vector<glm::vec3>& pathPoints, float maxSpeed, float maxRotate, float stopDist);

private:
	cObject3D* m_object;
	cObject3D* m_subject;
	eBehaviour m_behaviour;

	glm::vec3 m_targetPosition;
	glm::vec3 m_futurePosition;
	glm::vec3 m_objectPosition;
	glm::vec3 m_wanderConstPos;
	float m_maxSpeed;
	float m_maxRotate;
	float m_deltaTime;
	float m_stopDist;

	float m_slowRadius;

	float m_startRadius;
	float m_wanderRadius;

	float m_delayTime;
	float m_countTime;

	// Path Following
	std::vector<glm::vec3> m_vecPathPoints;
	float m_pathRadius;
	int m_indexA;
	int m_indexB;
	bool bIsConsistent;
	bool bIsReverse;
	bool bIsLoop;

	bool m_RotateToGoal(cObject3D* object, const glm::vec3& goalDir);
	bool m_LookProcess(cObject3D* object, const glm::vec3& targetPos);
	bool m_SeekProcess(cObject3D* object, const glm::vec3& targetPos);
	bool m_FleeProcess(cObject3D* object, const glm::vec3& targetPos);
	bool m_PursueProcess(cObject3D* object, const glm::vec3& targetPos);
	bool m_EvadeProcess(cObject3D* object, const glm::vec3& targetPos);
	bool m_ApproachProcess(cObject3D* object, const glm::vec3& targetPos);
	bool m_WanderFreeProcess(cObject3D* object, const glm::vec3& targetPos);
	bool m_WanderRadiusProcess(cObject3D* object, const glm::vec3& targetPos);
	bool m_PathFollowing();
	glm::vec3 m_ClosestPointOnLineAB(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
};


