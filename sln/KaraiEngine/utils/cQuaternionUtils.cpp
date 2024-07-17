#include "cQuaternionUtils.h"

cQuaternionUtils::cQuaternionUtils() {}

cQuaternionUtils::~cQuaternionUtils() {}

glm::quat cQuaternionUtils::RotationBetweenVectors(glm::vec3 startXYZ, glm::vec3 endXYZ)
{
	glm::vec3 start = normalize(startXYZ);
	glm::vec3 end = normalize(endXYZ);

	float cos = glm::dot(start, end);
	
	glm::vec3 rotationAxis;

	if (cos < -1 + 0.001f) 
	{
		rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);

		if (glm::length(rotationAxis) < 0.01)
			rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

		rotationAxis = normalize(rotationAxis);
		return glm::angleAxis(glm::radians(180.0f), rotationAxis);
	}

	rotationAxis = glm::cross(start, end);

	float s = sqrt((1 + cos) * 2);
	float invs = 1 / s;

	return glm::quat(s * 0.5f, rotationAxis.x * invs, rotationAxis.y * invs, rotationAxis.z * invs);
}

glm::quat cQuaternionUtils::LookAt(glm::vec3 direction)
{
	glm::quat rot1 = RotationBetweenVectors(glm::vec3(0.0f, 0.0f, -1.0f), direction);

	glm::vec3 oldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 right = glm::cross(direction, oldUp);

	oldUp = cross(right, direction);

	glm::vec3 newUp = rot1 * glm::vec3(0.0f, 1.0f, 0.0f);
	glm::quat rot2 = RotationBetweenVectors(newUp, oldUp);

	glm::quat targetOrientation = rot2 * rot1;

	return targetOrientation;
}

glm::quat cQuaternionUtils::RotateTowards(glm::quat startOrient, glm::quat endOrient, float maxAngle) 
{
	if (maxAngle <= 0.00000000f) {
		return startOrient;
	}

	float cosTheta = dot(startOrient, endOrient);

	if (cosTheta > 0.9999f) {
		return endOrient;
	}

	if (cosTheta < 0) 
	{
		startOrient = startOrient * -1.0f;
		cosTheta *= -1.0f;
	}

	float angle = acos(cosTheta);

	if (angle < maxAngle) {
		return endOrient;
	}

	float fT = maxAngle / angle;
	angle = maxAngle;

	glm::quat res = (sin((1.0f - fT) * angle) * startOrient + sin(fT * angle) * endOrient) / sin(angle);

	res = normalize(res);

	return res;
}