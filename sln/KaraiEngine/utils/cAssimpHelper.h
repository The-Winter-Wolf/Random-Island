#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <assimp/types.h>

class cAssimpHelper
{
public:
	glm::vec2 AssimpToGLM(const aiVector2D& input);

	glm::vec3 AssimpToGLM(const aiVector3D& input);

	glm::mat4 AssimpToGLM(const aiMatrix4x4& input);

	glm::quat AssimpToGLM(const aiQuaternion& input);
};