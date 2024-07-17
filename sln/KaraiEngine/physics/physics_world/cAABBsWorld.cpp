#include "cAABBsWorld.h"

#include "iostream"

#include "../../graphics/models/cModelManager.h"
extern cModelManager* g_pModelManager;
extern std::vector< cObject3D* > g_vpDebugObjectToDraw;

cAABBsWorld::cAABBsWorld(const glm::vec3& childNum, const glm::vec3& childExtent, const glm::vec3& shift, bool negativeSide)
{
	this->m_shift = shift;
	this->m_childNum = childNum;
	this->m_childExtents = childExtent;
	this->m_negativeSide = negativeSide;
}

cAABBsWorld::~cAABBsWorld()
{
	std::map< unsigned int, cAABB* >::iterator itAABB = m_map_pAABBS_by_ID.begin();
	for (itAABB; itAABB != m_map_pAABBS_by_ID.end(); itAABB++)
	{
		delete itAABB->second;
	}
}

void cAABBsWorld::PrepareAABBScene()
{
	int startX, startY, startZ;
	int endX, endY, endZ;

	if (m_negativeSide)
	{
		endX = static_cast<int>((m_childNum.x) * 0.5f);
		endY = static_cast<int>((m_childNum.y) * 0.5f);
		endZ = static_cast<int>((m_childNum.z) * 0.5f);
		startX = endX * -1;
		startY = endY * -1;
		startZ = endZ * -1;
	}
	else
	{
		endX = static_cast<int>(m_childNum.x);
		endY = static_cast<int>(m_childNum.y);
		endZ = static_cast<int>(m_childNum.z);
		startX = 0;
		startY = 0;
		startZ = 0;

	}

	for (int y = startY; y != endY; y++)
	{
		for (int x = startX; x != endX; x++)
		{
			for (int z = startZ; z != endZ; z++)
			{
				cAABB* pAABB = new cAABB();

				pAABB->minXYZ.x = x * m_childExtents.x + m_shift.x;
				pAABB->minXYZ.y = y * m_childExtents.y + m_shift.y;
				pAABB->minXYZ.z = z * m_childExtents.z + m_shift.z;

				pAABB->maxXYZ.x = pAABB->minXYZ.x + m_childExtents.x;
				pAABB->maxXYZ.y = pAABB->minXYZ.y + m_childExtents.y;
				pAABB->maxXYZ.z = pAABB->minXYZ.z + m_childExtents.z;

				unsigned int AABB_ID = cAABB::GetLocationIndex(pAABB->minXYZ, m_childExtents);

				m_map_pAABBS_by_ID[AABB_ID] = pAABB;
			}
		}
	}

	return;
}

cAABB* cAABBsWorld::AddObjectToAABBScene(cObject3D* physObject)
{
	cPhysicsBody::eCollType collType = physObject->GetPhysBody()->GetCollType();
	cPhysicsBody::eBodyType bodyType = physObject->GetPhysBody()->GetBodyType();

	if (bodyType == cPhysicsBody::KINEMATIC)
	{
		if (collType == cPhysicsBody::MESH)
		{	
			cModel* pTempModelPtr = physObject->GetModel();
			if (pTempModelPtr == nullptr)
			{
				return nullptr;
			}

			glm::mat4 matModel = physObject->GetMatModel();

			for (unsigned int k = 0; k != pTempModelPtr->vpMeshesOfModel.size(); k++)
			{
				int numOfvertices = static_cast<int>(pTempModelPtr->vpMeshesOfModel[k]->vertices.size());

				for (unsigned int index = 0; index != numOfvertices; index++)
				{
					glm::vec3 vert = pTempModelPtr->vpMeshesOfModel[k]->vertices[index].position;

					vert = matModel * glm::vec4(vert, 1.0f);

					unsigned int AABB_ID = cAABB::GetLocationIndex(vert, m_childExtents);

					std::map< unsigned int /*index*/, cAABB* >::iterator itAABB = m_map_pAABBS_by_ID.find(AABB_ID);

					if (itAABB == m_map_pAABBS_by_ID.end())
					{
						m_map_pAABBS_by_ID[AABB_ID] = new cAABB();
					}

					m_map_pAABBS_by_ID[AABB_ID]->vecKinematicVertices.push_back(vert);
				}
			}
		}

		if (collType == cPhysicsBody::SPHERE)
		{
			glm::vec3 pos = physObject->GetPosition();

			unsigned int AABB_ID = cAABB::GetLocationIndex(pos, m_childExtents);

			std::map< unsigned int /*index*/, cAABB* >::iterator itAABB = m_map_pAABBS_by_ID.find(AABB_ID);

			if (itAABB == m_map_pAABBS_by_ID.end())
			{
				m_map_pAABBS_by_ID[AABB_ID] = new cAABB();
			}

			m_map_pAABBS_by_ID[AABB_ID]->vecKinematicObjects.push_back(physObject);
		}
	}

	if (bodyType == cPhysicsBody::DYNAMIC)
	{
		if (collType == cPhysicsBody::SPHERE || collType == cPhysicsBody::MESH)
		{
			glm::vec3 pos = physObject->GetPosition();

			unsigned int AABB_ID = cAABB::GetLocationIndex(pos, m_childExtents);

			std::map< unsigned int /*index*/, cAABB* >::iterator itAABB = m_map_pAABBS_by_ID.find(AABB_ID);

			if (itAABB == m_map_pAABBS_by_ID.end())
			{
				m_map_pAABBS_by_ID[AABB_ID] = new cAABB();
			}

			m_map_pAABBS_by_ID[AABB_ID]->vecDynamicObjects.push_back(physObject);
			
			if (m_map_pAABBS_by_ID[AABB_ID]->vecDynamicObjects.size() == 1)
			{
				return m_map_pAABBS_by_ID[AABB_ID];
			}

			return nullptr;
		}
	}

	return nullptr;
}

cAABB* cAABBsWorld::GetAABBFromPos(const glm::vec3& position)
{
	unsigned int AABB_ID = cAABB::GetLocationIndex(position, m_childExtents);

	std::map< unsigned int /*index*/, cAABB* >::iterator itAABB = m_map_pAABBS_by_ID.find(AABB_ID);

	if (itAABB == m_map_pAABBS_by_ID.end())
	{
		return nullptr;
	}
	else
	{
		return itAABB->second;
	}
}

void cAABBsWorld::ProcessABBBsWorld(cAABB& parentAABB, std::vector<sTriangle>& vecTriangles,
									glm::ivec3& NumberBoxesPerSide, glm::vec3& extentLength)					
{												
	parentAABB.minXYZ.x = -extentLength.x / 2.0f;
	parentAABB.minXYZ.y = -extentLength.y / 2.0f;
	parentAABB.minXYZ.z = -extentLength.z / 2.0f;

	parentAABB.maxXYZ.x = extentLength.x / 2.0f;
	parentAABB.maxXYZ.y = extentLength.y / 2.0f;
	parentAABB.maxXYZ.z = extentLength.z / 2.0f;

	glm::vec3 childAABBExtents = glm::vec3(0.0f);
	childAABBExtents.x = extentLength.x / NumberBoxesPerSide.x;
	childAABBExtents.y = extentLength.y / NumberBoxesPerSide.y;
	childAABBExtents.z = extentLength.z / NumberBoxesPerSide.z;

	for (std::vector<sTriangle>::iterator itTriangle = vecTriangles.begin();
		itTriangle != vecTriangles.end(); itTriangle++)
	{
		unsigned int AABB_ID = cAABB::GetLocationIndex(itTriangle->vertices[0], childAABBExtents);

		std::map< unsigned int /*index*/, cAABB* >::iterator itAABB = parentAABB.mapChild_pAABBs.find(AABB_ID);

		if (itAABB == parentAABB.mapChild_pAABBs.end())
		{
			cAABB newAABB;
		}
	}

	return;
}

bool cAABBsWorld::IsTriangleInAABB(sTriangle theTri, cAABB* pAABB)
{
	if (theTri.GetSideLength(0) > pAABB->GetExtentsXYZ().x)
	{
		sTriangle tri[3];
		for (int triIndex = 0; triIndex != 3; triIndex++)
		{
			if (IsTriangleInAABB(tri[triIndex], pAABB))
			{
				return true;
			}
		}
	}

	return false;
}

void cAABBsWorld::CreateAABBdebSpheres()
{
	std::map< unsigned int, cAABB* >::iterator itAABB = m_map_pAABBS_by_ID.begin();
	for (itAABB; itAABB != m_map_pAABBS_by_ID.end(); itAABB++)
	{
		cObject3D* debObj = new cObject3D();
		debObj->bHasColor = true;
		debObj->SetColor(glm::vec4(1.0, 0.0, 0.0, 1.0));
		debObj->bIsVisible = true;
		debObj->SetScale(glm::vec3(1.0f));
		debObj->SetPosition(itAABB->second->minXYZ);
		debObj->BindModelToObject("Cube_1x1x1_xyz_n_rgba.ply");
		g_vpDebugObjectToDraw.push_back(debObj);

		for (int index = 0; index < itAABB->second->vecKinematicObjects.size(); index++)
		{
			cObject3D* obj = itAABB->second->vecKinematicObjects[index];
			obj->BindModelToObject("Sphere_r1_xyz_n_rgba.ply");
			obj->bIsVisible = true;
			obj->SetColor(glm::vec4(0.0, 1.0, 1.0, 1.0));
			obj->bIsWireframe = true;
			g_vpDebugObjectToDraw.push_back(obj);
		}

		//for (int index = 0; index < itAABB->second->vecDynamicObjects.size(); index++)
		//{
		//	cObject3D* obj = itAABB->second->vecDynamicObjects[index];
		//	cObject3D* debObj = new cObject3D();
		//	debObj->BindModelToObject("Sphere_r1_xyz_n_rgba.ply");
		//	debObj->bIsVisible = true;
		//	debObj->SetColor(glm::vec4(0.0, 0.0, 1.0, 1.0));
		//	cPhysicsBody::sSphereCollider* pSphereColl = static_cast<cPhysicsBody::sSphereCollider*>(obj->GetPhysBody()->GetCollider());
		//	debObj->SetScale(glm::vec3(pSphereColl->collRadius));
		//	debObj->bIsWireframe = true;
		//	g_vpDebugObjectToDraw.push_back(debObj);
		//}
	}
}