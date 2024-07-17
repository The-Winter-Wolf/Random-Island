#pragma once

#include "../../global/common.h"

#include <other/stb_image.h>
#include <glm/glm.hpp>
#include <map>

enum eTextureType
{
	texture_diffuse,
	texture_specular,
	texture_normal,
	texture_height,
	texture_opacity_black,
	texture_opacity_white,
	texture_emission
};

struct sTexture
{
	unsigned int textureID = -1;
	eTextureType textureType = texture_diffuse;

	bool bIsFlippedUVs = false;

	std::string directory;
	std::string textureFile;
	std::string friendlyName;

	float mixRatio = 0.0f;
	float texScale = 1.0f;
	float heightMapScale = 0.0f;
	glm::vec2 UVOffset = glm::vec2(0.0f, 0.0f);
};

class cTextureManager
{
public:
	~cTextureManager();
	static cTextureManager* GetTextureManager();

	

	sTexture* LoadTextureFromFile(const eTextureType& textureType, const std::string& fileName,
							 const std::string& friendName, bool flippedUVs = false);

	int LoadTextureCubemap(std::vector<std::string> faces, bool flippedUV);

	sTexture* GetTextureByFriendlyName(const std::string& friendlyTextureName);

	sTexture* GetTextureByFileName(const std::string& textureFileName);

	std::string GetStringFromType(eTextureType type);

	void SetBasePath(const std::string& basePath);
	std::string GetBasePath(void);

private:
	cTextureManager();
	static cTextureManager* m_pTextureManager;

	std::string m_directory;
	std::map<std::string /*fileNames*/, sTexture* /*texture*/> m_mapTextures;
	std::map<std::string /*fileNames*/, sTexture* /*texture*/>::iterator mapIterator;
};