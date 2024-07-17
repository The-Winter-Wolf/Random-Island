#pragma once
#include <string>
#include <vector>
#include <map>

#include <glm/glm.hpp>

class cSkybox
{
public:
	cSkybox();
	~cSkybox();

	void SetBasePath(std::string directory);
	
	void LoadSkyboxTextures(const std::string& friendlySkybox, bool flippedUV,
							const std::vector<std::string>& skyTextures);

	void LoadSkyboxTextures(const std::string& friendlySkybox, bool flippedUV,
							const std::string& rightTexture, const std::string& leftTexture,
							const std::string& topTexture, const std::string& bottomTexture,
							const std::string& frontTexture, const std::string& backTexture);

	void RenderSkybox(float deltaTime);

	void SetSkyboxUpdate(float dayLength, float currentTime, float cloudSpeed, float planetSpeed);

private:
	std::vector<std::string> m_vecSkyboxTextures;
	std::map<std::string /*skyName*/, unsigned int /*skyIndex*/> m_mapSkyboxes;

	void m_CreateSkybox();
	void m_UpdateSkybox(float deltaTime);
	int m_GetTextureIndex(std::string friendlySkyboxName);

	int skyboxIndex01;
	bool bHasTexture01;
	glm::vec4 skyboxColor01;

	int skyboxIndex02;
	bool bHasTexture02;
	glm::vec4 skyboxColor02;

	int skyboxIndex03;
	bool bHasTexture03;
	glm::vec4 skyboxColor03;

	int skyboxIndex04;
	bool bHasTexture04;
	glm::vec4 skyboxColor04;

	float m_time;
	float m_multiplier;
	float m_ratioIndex;
	float m_cloudSpeed;
	float m_planetSpeed;

	unsigned int m_skyboxVAO;
	std::string m_directory;
};

