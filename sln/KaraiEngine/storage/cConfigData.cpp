#include "cConfigData.h"

cConfigData::cConfigData()
{
////////////////////////////////////////////////////////

	camera.width = 1200;
	camera.height = 800;

	camera.mod = 0;

	camera.position = glm::vec3(0.0f, 0.0f, 0.0f);
	camera.direction = glm::vec3(0.0f, 0.0f, -1.0f);

	camera.fovMinMax = glm::vec3(60.0f, 20.0f, 60.0f);
	camera.pitchMinMax = glm::vec3(30.0f, 0.0f, 60.0f);
	camera.yawMinMax = glm::vec3(0.0f, -360.0f, 360.0f);
	camera.distMinMax = glm::vec3(2.0f, 0.1f, 10.0f);
	camera.pitchYawSpeed = glm::vec2(0.1, 0.1);

	camera.moveSpeed = 10.0f;
	camera.speedScale = 2.0f;

/////////////////////////////////////////////////////////	

	light.name = "Light ";
	light.type = "Direct";
	light.state = "Off";
	light.prm1 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	light.prm2 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	light.diff = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	light.spec = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	light.atten = glm::vec4(0.0f, 0.5f, 0.5f, 1.0f);
	light.pos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	light.dir = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);

/////////////////////////////////////////////////////////

	object.isVisible = true;
	object.fileName = "File ";
	object.friendlyName = "FriendlyName ";
	object.filePath = "../../extern/assets/models";
	object.hasLightColor = glm::vec2(1.0f, 0.0f);
	object.isSidedWireframe = glm::vec2(0.0f, 0.0f);
	object.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	object.position = glm::vec3(0.0f, 0.0f, 0.0f);
	object.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	object.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	object.hpXpSpeed = glm::vec3(0.0f, 0.0f, 0.0f);
	object.flipUVs = false;

//////////////////////////////////////////////////////////
}

cConfigData::~cConfigData(){}
