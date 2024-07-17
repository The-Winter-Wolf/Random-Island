#pragma once

#include <glm/glm.hpp>

#include "../objects/object_3D/cObject3D.h"

class cRenderManager
{
public:
	
	~cRenderManager();
	static cRenderManager* GetRenderManager();

	void InitObjectNUB();

	void DrawObject(cObject3D* pCurrentObject, glm::mat4 matModelParent);
	void DrawObject(std::string friendlyName, glm::mat4 matModelParent);

	void DrawDebugSphere(glm::vec3 position, float scale, glm::vec4 colourRGBA, bool isTwoSided = false);

	void DrawLightDebugSpheres();
	
	bool bIsBoneColor = false;

private:
	cRenderManager();
	static cRenderManager* m_pRenderManager;


};
