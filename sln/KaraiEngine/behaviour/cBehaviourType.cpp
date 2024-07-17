#include "cBehaviourType.h"

#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/random.hpp>

#include "../engine/cRenderManager.h"
extern cRenderManager* g_pRenderManager;

cBehaviourType::cBehaviourType()
{
	this->m_object = nullptr;
	this->m_subject = nullptr;
	this->m_behaviour = NONE;

	this->m_targetPosition = glm::vec3(0.0f);
	this->m_futurePosition = glm::vec3(0.0f);
	this->m_objectPosition = glm::vec3(0.0f);

	this->m_maxSpeed = 0.0f;
	this->m_maxRotate = 0.0f;
	this->m_deltaTime = 0.0f;
	this->m_stopDist = 1.0f;

	this->m_slowRadius = 0.0f;

	this->m_startRadius = 0.0f;
	this->m_wanderRadius = 0.0f;
	this->m_delayTime = 0.0f;
	this->m_countTime = 0.0f;
	this->m_wanderConstPos = glm::vec3(0.0f);

	this->m_pathRadius = 2.0f;
	this->m_indexA = 0;
	this->m_indexB = 1;
	this->bIsLoop = true;
}

cBehaviourType::~cBehaviourType()
{
}

bool cBehaviourType::Update(float deltaTime)
{
	m_deltaTime = deltaTime;

	if (m_subject != nullptr)
	{
		m_targetPosition = m_subject->GetPosition();
	}

	m_objectPosition = m_object->GetPosition();

	if (m_behaviour == NONE)
	{
		this->m_object->bHasColor = false;
		return false;
	}
	else if (m_behaviour == LOOK)
	{
		this->m_LookProcess(m_object, m_targetPosition);
		return true;
	}
	else if (m_behaviour == SEEK)
	{
		this->m_SeekProcess(m_object, m_targetPosition);
		return true;
	}
	else if (m_behaviour == FLEE)
	{
		this->m_FleeProcess(m_object, m_targetPosition);
		return true;
	}
	else if (m_behaviour == PURSUE)
	{
		this->m_PursueProcess(m_object, m_targetPosition);
		return true;
	}
	else if (m_behaviour == EVADE)
	{
		this->m_EvadeProcess(m_object, m_targetPosition);
		return true;
	}
	else if (m_behaviour == APPROACH)
	{
		this->m_ApproachProcess(m_object, m_targetPosition);
		return true;
	}
	else if (m_behaviour == WANDER_FREE)
	{
		this->m_WanderFreeProcess(m_object, m_targetPosition);
		return true;
	}
	else if (m_behaviour == WANDER_IN_RADIUS)
	{
		this->m_WanderRadiusProcess(m_object, m_targetPosition);
		return true;
	}
	else if (m_behaviour == PATH_FOLLOWING)
	{
		this->m_PathFollowing();
		return true;
	}

	return false;
}
bool cBehaviourType::LookBehaviour(cObject3D* object, cObject3D* subject, float maxRotate)
{
	this->m_object = object;
	this->m_subject = subject;
	this->m_behaviour = LOOK;

	this->m_maxRotate = maxRotate;

	return true;
}

bool cBehaviourType::SeekBehaviour(cObject3D* object, cObject3D* subject, float maxSpeed, float maxRotate, float stopDist)
{
	this->m_object = object;
	this->m_subject = subject;
	this->m_behaviour = SEEK;

	this->m_maxRotate = maxRotate;
	this->m_maxSpeed = maxSpeed;
	this->m_stopDist = stopDist;

	return true;
}

bool cBehaviourType::FleeBehaviour(cObject3D* object, cObject3D* subject, float maxSpeed, float maxRotate)
{
	this->m_object = object;
	this->m_subject = subject;
	this->m_behaviour = FLEE;

	this->m_maxRotate = maxRotate;
	this->m_maxSpeed = maxSpeed;

	return true;
}

bool cBehaviourType::PursueBehaviour(cObject3D* object, cObject3D* subject, float maxSpeed, float maxRotate, float stopDist)
{
	this->m_object = object;
	this->m_subject = subject;
	this->m_behaviour = PURSUE;

	this->m_maxRotate = maxRotate;
	this->m_maxSpeed = maxSpeed;
	this->m_stopDist = stopDist;

	return true;
}

bool cBehaviourType::EvadeBehaviour(cObject3D* object, cObject3D* subject, float maxSpeed, float maxRotate)
{
	this->m_object = object;
	this->m_subject = subject;
	this->m_behaviour = EVADE;

	this->m_maxRotate = maxRotate;
	this->m_maxSpeed = maxSpeed;

	return true;
}

bool cBehaviourType::ApproachBehaviour(cObject3D* object, cObject3D* subject, float maxSpeed, float maxRotate, float slowRadius)
{
	this->m_object = object;
	this->m_subject = subject;
	this->m_behaviour = APPROACH;

	this->m_maxRotate = maxRotate;
	this->m_maxSpeed = maxSpeed;
	this->m_slowRadius = slowRadius;

	return true;
}

bool cBehaviourType::WanderFreeBehaviour(cObject3D* object, float maxSpeed, float maxRotate, float startRadius, float wanderRadius, float delayTime)
{
	this->m_object = object;
	this->m_behaviour = WANDER_FREE;

	this->m_maxSpeed = maxSpeed;
	this->m_maxRotate = maxRotate;

	this->m_startRadius = startRadius;
	this->m_wanderRadius = wanderRadius;

	this->m_delayTime = delayTime;

	return true;
}

bool cBehaviourType::WanderRadiusBehaviour(cObject3D* object, float maxSpeed, float maxRotate, float startRadius, float delayTime)
{
	this->m_object = object;
	this->m_behaviour = WANDER_IN_RADIUS;

	this->m_maxSpeed = maxSpeed;
	this->m_maxRotate = maxRotate;

	this->m_startRadius = startRadius;

	this->m_delayTime = delayTime;

	this->m_wanderConstPos = m_object->GetPosition();

	return true;
}

bool cBehaviourType::PathFollowingBehaviour(cObject3D* object, const std::vector<glm::vec3>& pathPoints, float maxSpeed, float maxRotate, float stopDist)
{
	this->m_object = object;
	this->m_behaviour = PATH_FOLLOWING;

	this->m_stopDist = stopDist;
	this->m_maxSpeed = maxSpeed;
	this->m_maxRotate = maxRotate;

	m_vecPathPoints = pathPoints;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

bool cBehaviourType::m_RotateToGoal(cObject3D* object, const glm::vec3& goalDir)
{
	glm::vec3 currentForward = object->GetDirection();

	float rotationStep = m_maxRotate * m_deltaTime;

	if (glm::angle(goalDir, currentForward) < rotationStep)
	{
		return true;
	}

	glm::vec3 rotationVector = glm::cross(currentForward, goalDir);
	if (rotationVector.y > 0.0f)
	{
		rotationVector = glm::vec3(0.0, 1.0f, 0.0f);
	}
	else
	{
		rotationVector = glm::vec3(0.0f, -1.0f, 0.0f);
	}
	glm::quat orient = object->GetQuatOrientation();
	orient = glm::rotate(orient, rotationStep, rotationVector);
	object->SetQuatOrientation(orient);

	return true;
}

bool cBehaviourType::m_LookProcess(cObject3D* object, const glm::vec3& targetPos)
{
	glm::vec3 goalDirection = targetPos - m_objectPosition;
	goalDirection.y = 0.0;
	goalDirection = glm::normalize(goalDirection);

	this->m_RotateToGoal(object, goalDirection);

	return true;
}

bool cBehaviourType::m_SeekProcess(cObject3D* object, const glm::vec3& targetPos)
{
	glm::vec3 goalDirection = targetPos - m_objectPosition;
	goalDirection.y = 0.0;
	goalDirection = glm::normalize(goalDirection);

	this->m_RotateToGoal(object, goalDirection);

	float dist = glm::distance(m_targetPosition, m_objectPosition);
	if (dist > m_stopDist)
	{
		glm::vec3 targetDirection = m_targetPosition - m_objectPosition;
		targetDirection = glm::normalize(targetDirection);
		targetDirection.y = 0.0f;

		glm::vec3 acceleration = targetDirection * m_maxSpeed;
		object->GetPhysBody()->AddUserAccel(acceleration);
	}

	return true;
}

bool cBehaviourType::m_FleeProcess(cObject3D* object, const glm::vec3& targetPos)
{
	glm::vec3 goalDirection = m_objectPosition - targetPos;
	goalDirection = glm::normalize(goalDirection);

	this->m_RotateToGoal(object, goalDirection);

	glm::vec3 targetDirection = m_objectPosition - m_targetPosition;
	targetDirection = glm::normalize(targetDirection);
	targetDirection.y = 0.0f;

	glm::vec3 acceleration = targetDirection * m_maxSpeed;
	object->GetPhysBody()->AddUserAccel(acceleration);

	return true;
}

bool cBehaviourType::m_PursueProcess(cObject3D* object, const glm::vec3& targetPos)
{
	float scale = 400.0f;
	m_futurePosition = m_targetPosition + m_subject->GetPhysBody()->GetVelocity() * m_deltaTime * scale;

	this->m_SeekProcess(object, m_futurePosition);

	float dist = glm::distance(m_futurePosition, m_objectPosition);
	if (dist > m_stopDist)
	{
		glm::vec3 targetDirection = m_futurePosition - m_objectPosition;
		targetDirection = glm::normalize(targetDirection);
		targetDirection.y = 0.0f;

		glm::vec3 acceleration = targetDirection * m_maxSpeed;
		object->GetPhysBody()->AddUserAccel(acceleration);
	}

	return true;
}

bool cBehaviourType::m_EvadeProcess(cObject3D* object, const glm::vec3& targetPos)
{
	float scale = 300.0f;
	m_futurePosition = m_targetPosition + m_subject->GetPhysBody()->GetVelocity() * m_deltaTime * scale;

	this->m_FleeProcess(object, m_futurePosition);

	float dist = glm::distance(m_futurePosition, m_objectPosition);

	glm::vec3 targetDirection = m_objectPosition - m_futurePosition;
	targetDirection = glm::normalize(targetDirection);
	targetDirection.y = 0.0f;

	glm::vec3 acceleration = targetDirection * m_maxSpeed;
	object->GetPhysBody()->AddUserAccel(acceleration);

	return true;
}

bool cBehaviourType::m_ApproachProcess(cObject3D* object, const glm::vec3& targetPos)
{
	float slowingRadius = m_slowRadius;
	this->m_SeekProcess(object, m_targetPosition);

	glm::vec3 targetDirection = m_targetPosition - m_objectPosition;
	targetDirection = glm::normalize(targetDirection);
	targetDirection.y = 0.0f;

	float distance = glm::distance(m_targetPosition, m_objectPosition);


	glm::vec3 acceleration = targetDirection * m_maxSpeed * (distance / slowingRadius);
	object->GetPhysBody()->AddUserAccel(acceleration);
	if (distance < slowingRadius)
	{
		glm::vec3 acceleration = targetDirection * m_maxSpeed * (distance / slowingRadius);
		object->GetPhysBody()->AddUserAccel(acceleration);
	}
	else
	{
		glm::vec3 acceleration = targetDirection * m_maxSpeed;
		object->GetPhysBody()->AddUserAccel(acceleration);
	}

	return true;
}

bool cBehaviourType::m_WanderFreeProcess(cObject3D* object, const glm::vec3& targetPos)
{
	m_countTime -= m_deltaTime;
	if (m_countTime < 0.0f)
	{
		m_countTime = m_delayTime;

		m_targetPosition = object->GetPosition() + object->GetDirection() * m_startRadius;
		float theta = glm::linearRand(0.0f, glm::two_pi<float>());

		float x = m_wanderRadius * glm::cos(theta);
		float z = m_wanderRadius * glm::sin(theta);

		m_targetPosition.x += x;
		m_targetPosition.z -= z;
	}

	this->m_SeekProcess(object, m_targetPosition);

	return true;
}

bool cBehaviourType::m_WanderRadiusProcess(cObject3D* object, const glm::vec3& targetPos)
{
	m_countTime -= m_deltaTime;
	if (m_countTime < 0.0f)
	{
		m_countTime = m_delayTime;

		glm::vec2 randomPoint = glm::diskRand<float>(m_startRadius);
		m_targetPosition.x = randomPoint.x + m_wanderConstPos.x;
		m_targetPosition.z = randomPoint.y + m_wanderConstPos.z;
		m_targetPosition.y = m_wanderConstPos.y;
	}

	this->m_SeekProcess(object, m_targetPosition);

	return true;
}

bool cBehaviourType::m_PathFollowing()
{
	for (int i = 0; i < m_vecPathPoints.size(); i++)
	{
		g_pRenderManager->DrawDebugSphere(m_vecPathPoints[i], 0.2f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	}

	m_futurePosition = m_objectPosition + m_object->GetPhysBody()->GetVelocity() * m_deltaTime;
	float shortestDistance = std::numeric_limits<float>::max();

	for (int i = 0; i < m_vecPathPoints.size(); i++)
	{
		m_indexA = i;
		m_indexB = i + 1;

		if (m_indexA == m_vecPathPoints.size() - 1 && bIsLoop)
		{
			m_indexB = 0;
			m_indexA = m_vecPathPoints.size() - 1;
		}
		else if (m_indexA == m_vecPathPoints.size())
		{
			m_indexB = 1;
			m_indexA = 0;
		}

		if (glm::distance(m_vecPathPoints[m_indexB], m_objectPosition) < m_pathRadius)
		{
			continue;
		}

		//if (i == m_vecPathPoints.size() - 1 && bIsLoop)
		//{
		//	m_indexB = 0;
		//	m_indexA = m_vecPathPoints.size() - 1;
		//}

		glm::vec3 a = glm::vec3(m_vecPathPoints[m_indexA].x, m_futurePosition.y, m_vecPathPoints[m_indexA].z);
		glm::vec3 b = glm::vec3(m_vecPathPoints[m_indexB].x, m_futurePosition.y, m_vecPathPoints[m_indexB].z);

		glm::vec3 closestPoint = m_ClosestPointOnLineAB(a, b, m_futurePosition);

		float distance = glm::distance(m_futurePosition, closestPoint);
		if (distance < shortestDistance)
		{
			shortestDistance = distance;
			glm::vec3 middlePos = (closestPoint + b) / 2.0f;
			m_targetPosition = middlePos;
		}
	}

	//if (glm::distance(b, m_futurePosition) < m_pathRadius + 1)
	//{
	//	m_indexA++;
	//	m_indexB++;
	//}

	this->m_SeekProcess(m_object, m_targetPosition);

	return true;
}

glm::vec3 cBehaviourType::m_ClosestPointOnLineAB(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
{
	float t = glm::dot(c - a, b - a) / glm::dot(b - a, b - a);
	t = glm::clamp(t, 0.0f, 1.0f);
	glm::vec3 closestPoint = a + t * (b - a);
	return closestPoint;
}
