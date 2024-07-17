#include "cFlora.h"

#include "../../global/global.h"

#include "../../graphics/models/cModelManager.h"
extern cModelManager* g_pModelManager;

#include "../../graphics/textures/cTextureManager.h"
extern cTextureManager* g_pTextureManager;
#include "../../objects/light/cLightManager.h"
extern cLightManager* g_pLightManager;

#include "../../objects/object_3D/cObject3D.h"
extern cObject3D* g_pFindMeshByFriendlyName(std::string friendlyNameToFind);

#include "../terrain/cTerrain.h"
extern cTerrain* g_pTerrain;

#include "../../graphics/instancing/cInstance.h"
cInstance instancing;

#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/random.hpp>

cFlora::cFlora()
{
	this->m_newDir = glm::vec2(0.0f);
	this->m_oldDir = glm::vec2(0.0f);
	this->m_timer = 10.0f;
}

cFlora::~cFlora(){}

void cFlora::InitFloraElements()
{
	object = g_pFindMeshByFriendlyName("Moon");

	g_pTextureManager->SetBasePath("../../extern/assets/textures/terrain");
	sTexture* texture = g_pTextureManager->LoadTextureFromFile(texture_diffuse, "water03.jpg", "water03", false);
	g_pModelManager->SetTextureToModel("water03.jpg", "Flat_1x1_xyz_n_rgba_corner.ply", 0);
	g_pModelManager->SetTextureScale("water03.jpg", "Flat_1x1_xyz_n_rgba_corner.ply", 300.0f);

	texture = g_pTextureManager->LoadTextureFromFile(texture_diffuse, "grass_diffuse01.jpg", "grass", false);
	g_pModelManager->SetTextureToModel("grass_diffuse01.jpg", "Flat_1x1_xyz_n_rgba_center.ply", 0);
	g_pModelManager->SetTextureScale("grass_diffuse01.jpg", "Flat_1x1_xyz_n_rgba_center.ply", 300.0f);

	g_pTextureManager->SetBasePath("../../extern/assets/models/nature/trees/tree01");
	texture = g_pTextureManager->LoadTextureFromFile(texture_opacity_black, "blatt1_a.jpg", "treeOpacity01", false);
	g_pModelManager->SetTextureToModel("blatt1_a.jpg", "Tree1.3ds", 4);
	g_pModelManager->SetTextureToModel("blatt1_a.jpg", "Tree1.3ds", 5);

	g_pTextureManager->SetBasePath("../../extern/assets/models/nature/trees/tree03");
	texture = g_pTextureManager->LoadTextureFromFile(texture_opacity_black, "c04a_white.png", "treeOpacity02", false);
	g_pModelManager->SetTextureToModel("c04a_white.png", "Quercus glauca_FREEMODEL_std_obj.obj", 1);

	g_pTextureManager->SetBasePath("../../extern/assets/models/nature/grass");
	texture = g_pTextureManager->LoadTextureFromFile(texture_opacity_black, "Fern_Plant_Preview_FREE_Model_OPACITY_MAP.png", "grassOpacity01", false);
	g_pModelManager->SetTextureToModel("Fern_Plant_Preview_FREE_Model_OPACITY_MAP.png", "Fern_Plant_Preview_FREE_Model.fbx", 0);

	cInstance instancing;
	instancing.InitInstancing("Tree01", 40, 10.0f, 40.0f, 5.0f, 6.0f, true, true);
	instancing.InitInstancing("Tree02", 25, 10.0f, 40.0f, 5.0f, 4.0f, true, true);
	instancing.InitInstancing("Tree03", 90, 10.0f, 40.0f, 5.0f, 5.0f, true, true);
	instancing.InitInstancing("Grass01", 5000, 50.0f, 40.0f, 3.0f, 0.0f, true, false);

	return;
}

void cFlora::UpdateFlora(float deltaTime)
{
	m_oldDir = glm::vec2( 2.0f * cos( 0.1f * glfwGetTime() ), 2.0f * sin( 0.1f * glfwGetTime() ) );

	g_pModelManager->SetTextureUVOffset("water03.jpg", "Flat_1x1_xyz_n_rgba_corner.ply", m_oldDir);

	glm::vec3 pos = glm::vec3( 384.0f * -sin(0.02 * glfwGetTime() ) + 128, 384 * cos(0.02 * glfwGetTime()), 384.0f );
	object->SetPosition(pos);

	glm::vec3 dir = glm::normalize(WORLD_POS - pos);

	cLight* dirLight = g_pLightManager->GetLight(10);
	dirLight->direction = glm::vec4(dir, 1.0f);

}
