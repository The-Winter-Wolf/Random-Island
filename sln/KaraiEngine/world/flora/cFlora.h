#pragma once
#include "../../objects/object_3D/cObject3D.h"
#include <string>
#include <glm/glm.hpp>

class cFlora
{
public:
	cFlora();
	~cFlora();

	void InitFloraElements();
	void UpdateFlora(float deltaTime);

private:
	cObject3D* object;
	glm::vec2 m_newDir;
	glm::vec2 m_oldDir;
	float m_timer;
};

