#pragma once

#include <glm/glm.hpp>

class cShadow
{
public:
	cShadow();
	~cShadow();

	void InitShadows();
	void RenderDepthMap();
	void DebugDepthRender();

private:
	int shadow_width;
	int shadow_height;
	glm::mat4 lightMatrix;
};

