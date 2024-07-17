#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class cQuaternionUtils
{
public:
	cQuaternionUtils();
	~cQuaternionUtils();

	glm::quat RotationBetweenVectors(glm::vec3 startXYZ, glm::vec3 endXYZ);

	glm::quat LookAt(glm::vec3 direction);

	glm::quat RotateTowards(glm::quat startOrient, glm::quat endOrient, float maxAngle);

};

