#pragma once

#include <glm/glm.hpp>

#include <string>
#include <vector>

struct sCamera
{
	int width;
	int height;

	int mod;
	std::string objectName;

	glm::vec3 position;
	glm::vec3 direction;

	glm::vec2 pitchYawSpeed;
	glm::vec3 fovMinMax;
	glm::vec3 pitchMinMax;
	glm::vec3 yawMinMax;
	glm::vec3 distMinMax;

	float moveSpeed;
	float speedScale;
};

struct sLight
{
	std::string name;
	std::string type;
	std::string state;
	glm::vec4 prm1;
	glm::vec4 prm2;
	glm::vec4 diff;
	glm::vec4 spec;
	glm::vec4 atten;
	glm::vec4 pos;
	glm::vec4 dir;
};

struct sObject
{	
	bool isVisible;
	std::string filePath;
	std::string fileName;
	std::string friendlyName;
	glm::vec2 hasLightColor;
	glm::vec2 isSidedWireframe;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	glm::vec4 color;
	glm::vec3 hpXpSpeed;
	bool flipUVs;
};

class cConfigData
{
public:
	cConfigData();
	~cConfigData();

	sCamera camera;
	sLight light;
	sObject object;

	std::vector<sLight> vLights;
	std::vector<sObject> vObjects;

	std::string xmlPath;
	std::string jsonPath;

	std::string cameraFileName;
	std::string lightsFileName;
	std::string audiosFileName;
	std::string objectsFileName;
	std::string texturesFileName;
	std::string animationsFileName;
};

