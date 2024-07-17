#include "cAudioInstaller.h"

#include "../../global/common.h"
#include "../../global/global.h"

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <sstream>
 
#include "../../graphics/models/cModelManager.h"
extern cModelManager* g_pModelManager;

#include "../camera/cCamera.h"
extern cCamera* g_pCamera;

extern cObject3D* g_pFindMeshByFriendlyName(std::string friendlyNameToFind);

extern std::vector< cObject3D* > g_vpDebugObjectToDraw;

#define CheckGLError() _CheckGLError(__FILE__, __LINE__)
void _CheckGLError(const char* file, int line)
{
	GLenum err(glGetError());

	while (err != GL_NO_ERROR)
	{
		std::string error;
		switch (err)
		{
		case GL_INVALID_OPERATION:				error = "INVALID_OPERATION";				break;
		case GL_INVALID_ENUM:					error = "INVALID_ENUM";					    break;
		case GL_INVALID_VALUE:					error = "INVALID_VALUE";					break;
		case GL_OUT_OF_MEMORY:					error = "OUT_OF_MEMORY";					break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";    break;
		}
		std::stringstream ss;
		ss << "[" << file << ":" << line << "] " << error;

		printf("OpenGL Error: %s\n", ss.str().c_str());

		err = glGetError();
	}

	return;
}

cAudioInstaller* cAudioInstaller::m_pAudioInstaller = NULL;

cAudioInstaller* cAudioInstaller::GetAudioInstaller()
{
	if (cAudioInstaller::m_pAudioInstaller == NULL)
	{
		cAudioInstaller::m_pAudioInstaller = new cAudioInstaller();
	}
	return cAudioInstaller::m_pAudioInstaller;
}

cAudioInstaller::cAudioInstaller()
{
	m_AudioManager = new cAudioManager();
	m_AudioManager->Initialize();

	this->minSelect = 0;
	this->maxSelect = 0;

	this->selectVolume = 0.3f;
	this->selectPitch = 1.0f;

	this->isReverb = false;
	this->isChorus = false;
	this->isLowPass = false;
	this->isHighPass = false;
	this->isDistortion = false;
}

cAudioInstaller::~cAudioInstaller()
{
	assert(m_AudioManager);
	m_AudioManager->Destroy();
}

void cAudioInstaller::Init()
{
	m_AudioManager->LoadAudio("../../extern/assets/audios/EpicBattle.mp3");

	// 0 //
	glm::vec3 position = glm::vec3(WORLD_POS);
	glm::vec3 color = glm::vec3(1.0, 1.0, 1.0);
	int channelId = m_AudioManager->PlayAudio("../../extern/assets/audios/EpicBattle.mp3", 0, position);
	this->channels.push_back(channelId);
	//this->m_CreateDebugSphere(position, color, channelId);
	m_AudioManager->SetChannelVolume(channelId, 0.1);
	m_AudioManager->SetPaused(channelId, false);
}

void cAudioInstaller::SetPaused(int channelID, bool true_false)
{
	m_AudioManager->SetPaused(channelID, true_false);
}
bool cAudioInstaller::GetPaused(int channelID)
{
	return m_AudioManager->GetPaused(channelID);
}

void cAudioInstaller::Update(float dt)
{
	if (g_pCamera->GetCameraMod() == STATIC)
	{
		m_AudioManager->SetListenerAttributes(
			g_pCamera->GetPosition(),
			glm::vec3(0.0),
			glm::vec3(0.0f, 0.0f, -1.0f),
			g_pCamera->GetUpVec());
	}
	else
	{
		m_AudioManager->SetListenerAttributes(g_pCamera->GetPosition(), glm::vec3(0.0),
											  g_pCamera->GetDirection() * glm::vec3(-1.0f),
											  g_pCamera->GetUpVec());
	}

	m_AudioManager->Update();
}

void cAudioInstaller::m_CreateWallsManually()
{
	// Create our occlusion walls polygons
	std::vector<glm::vec3> vertices;

	vertices.push_back(glm::vec3(0.f, 0.f, -3.f));
	vertices.push_back(glm::vec3(0.f, 3.f, -3.f));
	vertices.push_back(glm::vec3(0.f, 3.f, 3.f));
	vertices.push_back(glm::vec3(0.f, 0.f, 3.f));

	m_AudioManager->AddPolygon(0.9f, 0.9f, true, vertices);

	glm::vec3 position(5.25f, 0.0f, 0.0f);
	glm::vec3 forward(0.0f, 0.0f, 1.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 scale(1.0f, 1.0f, 1.0f);

	m_AudioManager->SetGeometryPosition(position);
	m_AudioManager->SetGeometryRotation(forward, up);
	m_AudioManager->SetGeometryScale(scale);
}

void cAudioInstaller::m_CreateWallsFromModel(std::string friendName)
{
	cObject3D* pObject = g_pFindMeshByFriendlyName(friendName);

	if (pObject == nullptr)
	{
		return;
	}

	glm::vec3 position = pObject->GetPosition();
	glm::vec3 scale = pObject->GetScale();
	glm::quat orientation = pObject->GetQuatOrientation();
	
	glm::vec3 forward = orientation * glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up = orientation * glm::vec3(0.0f, 1.0f, 0.0f);

	cModel* pTempModel = pObject->GetModel();

	std::vector<glm::vec3> vertices;

	this->m_BuildPolygonFromModel(pTempModel, vertices);

	m_AudioManager->CreateNewGeometry(100, 100);

	for (int i = 0; i != vertices.size(); i+=4)
	{
		std::vector<glm::vec3> square;
		square.push_back(vertices[0 + i]);
		square.push_back(vertices[1 + i]);
		square.push_back(vertices[2 + i]);
		square.push_back(vertices[3 + i]);

		m_AudioManager->AddPolygon(0.9f, 0.9f, true, square);
	}

	m_AudioManager->SetGeometryPosition(position);
	m_AudioManager->SetGeometryRotation(forward, up);
	m_AudioManager->SetGeometryScale(scale);

#if AUDIO_CHANGE_MODE
	this->CreateDebugMesh(vertices, mesh);
#endif // AUDIO_CHANGE_MODE

	return;
}

void cAudioInstaller::m_CreateDebugSphere(glm::vec3 pos, glm::vec3 color, int id)
{
	cObject3D* pObject = new cObject3D();
	pObject->SetPosition(pos);
	pObject->bIsVisible = true;
	pObject->bHasColor = true;
	pObject->bHasLight = false;
	pObject->SetColor(glm::vec4(color, 1.0f));
	pObject->friendlyName = "Sphere" + std::to_string(id);
	g_vpDebugObjectToDraw.push_back(pObject);
}

void cAudioInstaller::m_CreateDebugMesh(std::vector < glm::vec3> vertices, cObject3D* originMesh)
{
	cModel* newModel = new cModel();
	newModel->modelName = originMesh->friendlyName;

	for (int i = 0; i != vertices.size(); i += 4)
	{
		cMeshDrawInfo* meshInfo = new cMeshDrawInfo();

		meshInfo->vertices.resize(4);
		meshInfo->vertices[0].position = glm::vec4(vertices[0 + i], 1.0f);
		meshInfo->vertices[1].position = glm::vec4(vertices[1 + i], 1.0f);
		meshInfo->vertices[2].position = glm::vec4(vertices[2 + i], 1.0f);
		meshInfo->vertices[3].position = glm::vec4(vertices[3 + i], 1.0f);

		meshInfo->vertices[0].color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		meshInfo->vertices[1].color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		meshInfo->vertices[2].color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		meshInfo->vertices[3].color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

		meshInfo->numberOfVertices = static_cast<unsigned int>(meshInfo->vertices.size());

		meshInfo->indices.resize(6);
		meshInfo->indices[0] = 0;
		meshInfo->indices[1] = 1;
		meshInfo->indices[2] = 2;
		meshInfo->indices[3] = 0;
		meshInfo->indices[4] = 2;
		meshInfo->indices[5] = 3;

		meshInfo->numberOfIndices = static_cast<unsigned int>(meshInfo->indices.size());
		meshInfo->numberOfTriangles = meshInfo->numberOfIndices / 3;
	
		newModel->vpMeshesOfModel.push_back(meshInfo);
	}
	
	g_pModelManager->LoadModelIntoVAO(newModel);

	cObject3D* polygonObject = new cObject3D();
	polygonObject->BindModelToObject(originMesh->friendlyName);
	polygonObject->friendlyName = originMesh->friendlyName;
	polygonObject->SetPosition(originMesh->GetPosition());
	polygonObject->SetQuatOrientation(originMesh->GetQuatOrientation());
	polygonObject->SetScale(originMesh->GetScale());
	polygonObject->SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	polygonObject->bHasLight = false;
	polygonObject->bIsVisible = true;
	polygonObject->bIsWireframe = false;
	polygonObject->bHasColor = true;

	g_vpDebugObjectToDraw.push_back(polygonObject);
}

void cAudioInstaller::m_BuildPolygonFromModel(cModel* modelInfo, std::vector<glm::vec3>& vert)
{
	glm::vec3 maxModel = glm::vec3(FLT_MIN);
	glm::vec3 minModel = glm::vec3(FLT_MAX);

	for (cMeshDrawInfo* meshDrawInfo : modelInfo->vpMeshesOfModel)
	{
		meshDrawInfo->CalcExtents();
		glm::vec3 max = meshDrawInfo->maxExtents_XYZ;
		glm::vec3 min = meshDrawInfo->minExtents_XYZ;

		if (max.x > maxModel.x) maxModel.x = max.x;
		if (max.y > maxModel.y) maxModel.y = max.y;
		if (max.z > maxModel.z) maxModel.z = max.z;

		if (min.x < minModel.x) minModel.x = min.x;
		if (min.y < minModel.y) minModel.y = min.y;
		if (min.z < minModel.z) minModel.z = min.z;
	}

	std::vector<glm::vec3> vertices;

	if (maxModel.x == minModel.x)
	{
		vertices.push_back(glm::vec3(maxModel.x, maxModel.y, maxModel.z));
		vertices.push_back(glm::vec3(maxModel.x, maxModel.y, minModel.z));
		vertices.push_back(glm::vec3(maxModel.x, minModel.y, minModel.z));
		vertices.push_back(glm::vec3(maxModel.x, minModel.y, maxModel.z));

		vert = vertices;
		return;
	}
	if (maxModel.y == minModel.y)
	{
		vertices.push_back(glm::vec3(maxModel.x, maxModel.y, maxModel.z));
		vertices.push_back(glm::vec3(minModel.x, maxModel.y, maxModel.z));
		vertices.push_back(glm::vec3(minModel.x, maxModel.y, minModel.z));
		vertices.push_back(glm::vec3(maxModel.x, maxModel.y, minModel.z));

		vert = vertices;
		return;
	}
	if (maxModel.z == minModel.z)
	{
		vertices.push_back(glm::vec3(maxModel.x, maxModel.y, maxModel.z));
		vertices.push_back(glm::vec3(minModel.x, maxModel.y, maxModel.z));
		vertices.push_back(glm::vec3(minModel.x, minModel.y, maxModel.z));
		vertices.push_back(glm::vec3(maxModel.x, minModel.y, maxModel.z));

		vert = vertices;
		return;
	}

	// X
	vertices.push_back(glm::vec3(maxModel.x, maxModel.y, maxModel.z));
	vertices.push_back(glm::vec3(maxModel.x, maxModel.y, minModel.z));
	vertices.push_back(glm::vec3(maxModel.x, minModel.y, minModel.z));
	vertices.push_back(glm::vec3(maxModel.x, minModel.y, maxModel.z));
	// Y
	vertices.push_back(glm::vec3(maxModel.x, maxModel.y, maxModel.z));
	vertices.push_back(glm::vec3(minModel.x, maxModel.y, maxModel.z));
	vertices.push_back(glm::vec3(minModel.x, maxModel.y, minModel.z));
	vertices.push_back(glm::vec3(maxModel.x, maxModel.y, minModel.z));
	// Z
	vertices.push_back(glm::vec3(maxModel.x, maxModel.y, maxModel.z));
	vertices.push_back(glm::vec3(minModel.x, maxModel.y, maxModel.z));
	vertices.push_back(glm::vec3(minModel.x, minModel.y, maxModel.z));
	vertices.push_back(glm::vec3(maxModel.x, minModel.y, maxModel.z));
	// -X
	vertices.push_back(glm::vec3(minModel.x, minModel.y, minModel.z));
	vertices.push_back(glm::vec3(minModel.x, minModel.y, maxModel.z));
	vertices.push_back(glm::vec3(minModel.x, maxModel.y, maxModel.z));
	vertices.push_back(glm::vec3(minModel.x, maxModel.y, minModel.z));
	// -Y
	vertices.push_back(glm::vec3(minModel.x, minModel.y, minModel.z));
	vertices.push_back(glm::vec3(minModel.x, minModel.y, maxModel.z));
	vertices.push_back(glm::vec3(maxModel.x, minModel.y, maxModel.z));
	vertices.push_back(glm::vec3(maxModel.x, minModel.y, minModel.z));
	// -Z
	vertices.push_back(glm::vec3(minModel.x, minModel.y, minModel.z));
	vertices.push_back(glm::vec3(maxModel.x, minModel.y, minModel.z));
	vertices.push_back(glm::vec3(maxModel.x, maxModel.y, minModel.z));
	vertices.push_back(glm::vec3(minModel.x, maxModel.y, minModel.z));

	vert = vertices;

	return;
}
