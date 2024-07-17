#pragma once

#include "../../global/common.h"

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <map>

struct sTerVertex
{
	glm::vec4 position;
	glm::vec4 normal;
	glm::vec2 texCoords;
	glm::vec4 color;
};

class cTerrain
{
public:
	cTerrain();
	~cTerrain();

	void SetBasePath(std::string directory);

	void SetTerrainSettings(float heightScale, float heightShift);

	void LoadTerrainTextures(const std::string& diffuseName, const std::string& heightName);

	void RenderTerrain();

	float GetTerrainHight(float x, float z, glm::vec3& normal) const;
	float GetTerrainHight(float x, float z) const;

	void SetPhysicsTerrainData(float& invMass, float& restitution);
	void GetPhysicsTerrainData(float& invMass, float& restitution) const;

private:
	std::string m_directory;

	int m_height, m_width;

	float m_restitution;
	float m_invMass;

	// height map attribute
	float m_heightScale, m_heightShift;

	unsigned int m_heightID;
	std::string m_heightType;
	
	// diffuse attribute
	unsigned int m_diffuseID;
	std::string m_diffuseType;

	// vertex attribute
	unsigned int terrainVAO;
	unsigned int terrainVBO;
	unsigned int terrainIBO;

	std::vector<unsigned int> indices;
	std::vector<sTerVertex> vertices;

	bool m_LoadTexture(const std::string& textureName, std::string modelType, bool flippedUV = false);

	void m_CreateTerrain(int width, int height, int nChannels, unsigned char* data);
};
	
