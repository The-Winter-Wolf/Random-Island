#pragma once

#include "../textures/cTextureManager.h"

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <map>

#define MAX_BONE_INFLUENCE 4

struct sVertex
{
	glm::vec4 position;
	glm::vec4 normal;
	glm::vec4 color;
	glm::vec2 texCoords;

	int boneIDs[MAX_BONE_INFLUENCE];
	float weights[MAX_BONE_INFLUENCE];
};

struct sBoneInfo
{
	int id;
	glm::mat4 offset;
};

class cMeshDrawInfo
{
public:
	cMeshDrawInfo();
	~cMeshDrawInfo();

	std::string meshName;
	bool bIsDynamic;

	unsigned int VAO_ID;

	unsigned int VertexBufferID;
	unsigned int numberOfVertices;

	unsigned int MatModelBufferID;
	unsigned int numberOfMatrices;

	unsigned int IndexBufferID;
	unsigned int numberOfIndices;

	unsigned int numberOfTriangles;

	unsigned int numberOfBones;

	glm::vec3 maxExtents_XYZ;	
	glm::vec3 minExtents_XYZ;	
	glm::vec3 deltaExtents_XYZ;	
	float maxExtent;

	void CalcExtents(void);

	auto& GetBoneInfoMap();
	int& GetBoneCount();

	std::vector<sVertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<sTexture*> textures;
};

class cModel
{
public:
	cModel();
	~cModel();

	std::string modelPath;
	std::string modelName;

	std::vector<cMeshDrawInfo*> vpMeshesOfModel;

	std::map<std::string, sBoneInfo>& GetBoneInfoMap();
	int& GetBoneCount();

	unsigned int GetUniqueID(void);

	std::vector<glm::mat4> matrices;

private:
	std::map<std::string /*name*/, sBoneInfo> m_mapModelBoneInfo;
	int m_ModelBoneCounter = 0;

	unsigned int m_UniqueID;
	static const unsigned int FIRST_UNIQUE_ID = 1;
	static unsigned int m_nextUniqueID;
};


