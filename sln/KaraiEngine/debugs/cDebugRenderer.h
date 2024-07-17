#pragma once

#include "../global/common.h"
#include <glm/glm.hpp>

class cDebugRenderer
{
public:
	cDebugRenderer();

	bool Initialize(void);
	void ShutDown(void);

	void RenderDebugObjects(double deltaTime, glm::mat4 matView, glm::mat4 matProjection);

	void AddLine(glm::vec3 startXYZ, glm::vec3 endXYZ, glm::vec4 RGBA);

	void AddSphere(glm::vec3 location, float radius, glm::vec4 RGBA);
	void AddCube(glm::vec3 centreLocation, float size, glm::vec4 RGBA);

	std::string GetLastError(void);

private:
	std::string m_lastError;

	struct sLineVertex_Buffer
	{
		float x, y, z, w;
		float r, g, b, a;
	};

	const unsigned int LINE_VERTEX_BUFFER_SIZE = 20'000;
	unsigned int m_sizeOfLineVertexBuffer;
	sLineVertex_Buffer* m_pLinesVerticesToDraw;

	struct sSphere
	{
		glm::vec3 centreXYZ;	
		float radius;	
		glm::vec4 RGBA;
	};

	struct sCube
	{
		glm::vec3 centreXYZ;	
		float size;		
		glm::vec4 RGBA;
	};

	struct sArrow
	{
		glm::vec3 centreXYZ;	
		glm::vec3 direction;	
		glm::vec4 RGBA;
	};

	std::vector<sSphere> m_vecSpheresToDraw;
	std::vector<sCube> m_vecCubesToDraw;
	std::vector<sArrow>	m_vecArrowsToDraw;

};
