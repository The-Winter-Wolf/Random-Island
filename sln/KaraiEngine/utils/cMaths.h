#pragma once

#include <glm/glm.hpp>
#include <string>

class cMaths
{
public:
	cMaths();
	~cMaths();

	float Barrycentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos);

	void DisassembleMatrix4(glm::mat4 matrix, glm::vec3& position, glm::vec3& scale, glm::quat& rotation);
	void DisassembleMatrix3(glm::mat3 matrix, glm::vec3& scale, glm::quat& rotation);
};

