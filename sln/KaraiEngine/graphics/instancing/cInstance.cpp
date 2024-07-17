#include "cInstance.h"

#include "../../graphics/models/cModelManager.h"
extern cModelManager* g_pModelManager;

#include "../../physics/cPhysicsManager.h"
extern cPhysicsManager* g_pPhysicsManager;

#include "../../objects/object_3D/cObject3D.h"
extern cObject3D* g_pFindMeshByFriendlyName(std::string friendlyNameToFind);

#include "../../world/terrain/cTerrain.h"
extern cTerrain* g_pTerrain;

#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/random.hpp>

cInstance::cInstance()
{
}

cInstance::~cInstance()
{
}

void cInstance::InitInstancing(const std::string& objectName, int numOfInstances, float scaleFactor, 
						float spawnRadius, float avoidRadius, float minBetweenDist, bool terrainNormals, bool bHasCollision)
{
	cObject3D* object = g_pFindMeshByFriendlyName(objectName);

	if (!object)
	{
		return;
	}

	object->GetModel()->matrices.resize(numOfInstances);

	glm::vec3 centerPos = object->GetPosition();
	glm::quat orientation = object->GetQuatOrientation();
	glm::vec3 objScale = object->GetScale();

	std::vector< glm::vec2 > vecPosition;
	for (int index = 0; index < numOfInstances; index++)
	{
		glm::mat4 matModel = glm::mat4(1.0f);

		glm::vec2 startPos = glm::vec2(centerPos.x, centerPos.z);
		if (avoidRadius > spawnRadius)
		{
			avoidRadius = 0.0f;
		}

		bool isLoop = true;
		glm::vec2 position = glm::vec2(0.0f);
		glm::vec2 direction = glm::vec2(0.0f);

		while (isLoop)
		{
			float radious = glm::linearRand(avoidRadius, spawnRadius);
			glm::vec2 direction = glm::normalize(glm::diskRand(10000.0f) + glm::diskRand(10000.0f));
			position = startPos + direction * radious;
			isLoop = false;
			if (minBetweenDist == 0.0f)
			{
				break;
			}

			for (int index = 0; index < vecPosition.size(); index++)
			{
				if (glm::distance(vecPosition[index], position) < minBetweenDist)
				{
					isLoop = true;
					break;
				}
			}
		}

		glm::vec3 terrainNormal;
		float hight = g_pTerrain->GetTerrainHight(position.x, position.y, terrainNormal);
		glm::vec3 posVec3 = glm::vec3(position.x, hight, position.y);
		glm::mat4 matTransform = glm::translate(matModel, posVec3);

		float scaleMin = 1 - scaleFactor / 100.0f;
		float scaleMax = 1 + scaleFactor / 100.0f;
		float scale = objScale.x * glm::linearRand(scaleMin, scaleMax);
		glm::mat4 matScale = glm::scale(matModel, glm::vec3(scale));

		float rotAngleX = glm::radians(glm::linearRand(0.0f,360.0f));
		float rotAngleZ = glm::radians(glm::linearRand(0.0f, 360.0f));
		//glm::quat orient = orientation;
		//glm::quat orient = glm::quat(glm::vec3(rotAngleX, 0.0f, rotAngleZ));

		glm::mat4 matRotate = glm::rotate(matModel, rotAngleX, glm::vec3(0.0f,1.0f,0.0f));

		matRotate *= glm::mat4(orientation);

		glm::mat4 model = matModel * matTransform * matRotate * matScale;

		object->GetModel()->matrices[index] = model;

		vecPosition.push_back(position);

		if (bHasCollision)
		{
			cObject3D* obj = new cObject3D();
			glm::vec3 up = orientation * glm::vec3(0.0f, 1.0f, 0.0f);
			float tempScale = scale / 4.0f / objScale.x;
			obj->GetPhysBody()->SetSphereCollider(tempScale);
			obj->GetPhysBody()->SetBodyType(cPhysicsBody::KINEMATIC);
			obj->GetPhysBody()->SetMass(0.0f);
			obj->SetPosition(posVec3 + glm::vec3(0.0f,0.2f,0.0f));
			obj->SetScale(glm::vec3(tempScale));

			g_pPhysicsManager->AddPhysObject(obj);
		}
	}

	g_pModelManager->UpdateMatrixBuffers(object->GetModel());
}