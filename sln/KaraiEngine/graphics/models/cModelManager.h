#pragma once
#include "cModel.h"

#include "../../utils/cAssimpHelper.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <map>

class cModelManager
{
public:
	~cModelManager();
	static cModelManager* GetModelManager();

	void SetBasePath(const std::string& basePathWithoutSlash);
	std::string GetBasePath(void);


	cModel* LoadModelIntoVAO(const std::string& fileName, 
						  unsigned int shaderProgramID,
						  bool flippedUVs = false,
                          bool bIsDynamicBuffer = false);

	bool LoadModelIntoVAO(cModel* modelInfo);

	cModel* FindDrawInfoByModelName(const std::string& filename);


	bool CheckNameInMap(const std::string& filename);

	bool UpdateVAOBuffers(const cModel* updatedModelDrawInfo);

	bool UpdateVertexBuffers(const cModel* updatedDrawInfo);
	bool UpdateMatrixBuffers(const cModel* updatedDrawInfo);

	void SetTextureToModel(const std::string& textureName, const std::string& modelName, int meshIndex = -1);

	void SetTextureUVOffset(const std::string& textureName, const std::string& modelName, const glm::vec2& UVOffset);
	glm::vec2 GetTextureUVOffset(const std::string& textureName, const std::string& modelName);

	void SetTextureScale(const std::string& textureName, const std::string& modelName, float texScale);
	float GetTextureScale(const std::string& textureName, const std::string& modelName);

	void SetHeightMapScale(const std::string& textureName, const std::string& modelName, const float& heightScale);
	float GetHeightMapScale(const std::string& textureName, const std::string& modelName);

private:
	cModelManager();
	static cModelManager* m_pModelManager;

	std::map<std::string /*model name*/, cModel* /*model info*/> m_map_pModelsByName;

	cModel* m_pTempModel;

	unsigned int shaderProgID = 0;

	bool bIsDynamicBuff = false;

	bool bIsFlippedUVs = false;

	bool bIsColor = false;

	std::string directory;

	bool m_LoadModelAssimp(const std::string& path);

	void m_ProcessNode(aiNode* node, const aiScene* scene);

	void m_SetupMesh(cMeshDrawInfo* drawInfo);

	void m_ProcessMesh(aiMesh* mesh, const aiScene* scene, cMeshDrawInfo* meshDrawInfo);

	bool m_CheckPlyFileFormat(const std::string& fileName);

	void m_LoadPly_XYZ_N_RGBA_UV(const std::string& fileName, cMeshDrawInfo* drawInfo);

	void m_SetDefaultBoneData(sVertex& vertex);
	void m_ExtractBoneWeights(cMeshDrawInfo* meshInfo, aiMesh* mesh);
	void m_FindNumberOfTypes(aiMaterial* material);

	std::vector<sTexture*> m_LoadMaterialTextures(aiMaterial* mat, aiTextureType type, eTextureType typeName) const;
};

