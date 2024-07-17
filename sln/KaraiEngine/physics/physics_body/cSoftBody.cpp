#include "cSoftBody.h"

#include "../../graphics/models/cModelManager.h"
extern cModelManager* g_pModelManager;

#include "../../utils/cRandomUtils.h"
extern cRandomUtils* g_pRandUtils;

#include "../../global/global.h"

cSoftBody::cSoftBody() 
{
	this->bAttach = false;
	this->bIsConvert = false;

	m_pSoftModel = nullptr;

	this->m_center = glm::vec3(0.0f);
	this->m_matModel = glm::mat4(1.0f);
}

cSoftBody::~cSoftBody() 
{
	for (unsigned int i = 0; i < m_vecSoftMeshes.size(); i++)
	{
		for (unsigned int j = 0; j < m_vecSoftMeshes[i]->vecConstraints.size(); j++)
		{
			delete m_vecSoftMeshes[i]->vecConstraints[j];
		}
		for (unsigned int k = 0; k < m_vecSoftMeshes[i]->vecParticles.size(); k++)
		{
			delete m_vecSoftMeshes[i]->vecParticles[k];
		}
	}
}

cModel* cSoftBody::CreateSoftModel(const std::string& modelName, const glm::mat4& matModel)
{
	cModel* oldModel = g_pModelManager->FindDrawInfoByModelName(modelName);

	this->m_CopyModelInfo(oldModel);

	this->m_matModel = matModel;

	for (cMeshDrawInfo* meshInfo : m_pSoftModel->vpMeshesOfModel)
	{
		sSoftMesh* pSoftMesh = new sSoftMesh();

		for (unsigned int index = 0; index < meshInfo->vertices.size(); index++)
		{
			sParticle* curParticle = new sParticle();

			curParticle->pVertex = &meshInfo->vertices[index];

			// Apply transformation
			glm::vec4 theVertex = glm::vec4( meshInfo->vertices[index].position.x,
											 meshInfo->vertices[index].position.y,
											 meshInfo->vertices[index].position.z, 1.0f);		

			theVertex = m_matModel * theVertex;

			curParticle->position = theVertex;
			curParticle->old_position = curParticle->position;
			pSoftMesh->vecParticles.push_back(curParticle);
		}

		for (unsigned int index = 0; index < meshInfo->numberOfIndices; index += 3)
		{
			sParticle* pParticle1 = pSoftMesh->vecParticles[meshInfo->indices[index + 0]];
			sParticle* pParticle2 = pSoftMesh->vecParticles[meshInfo->indices[index + 1]];
			sParticle* pParticle3 = pSoftMesh->vecParticles[meshInfo->indices[index + 2]];

			sConstraint* pEdge1 = new sConstraint();
			pEdge1->pParticleA = pParticle1;
			pEdge1->pParticleB = pParticle2;
			pEdge1->restLength = this->m_Distance(pEdge1->pParticleA, pEdge1->pParticleB);

			sConstraint* pEdge2 = new sConstraint();
			pEdge2->pParticleA = pParticle2;
			pEdge2->pParticleB = pParticle3;
			pEdge2->restLength = this->m_Distance(pEdge2->pParticleA, pEdge2->pParticleB);

			sConstraint* pEdge3 = new sConstraint();
			pEdge3->pParticleA = pParticle3;
			pEdge3->pParticleB = pParticle1;
			pEdge3->restLength = this->m_Distance(pEdge3->pParticleA, pEdge3->pParticleB);

			pSoftMesh->vecConstraints.push_back(pEdge1);
			pSoftMesh->vecConstraints.push_back(pEdge2);
			pSoftMesh->vecConstraints.push_back(pEdge3);
		}

		this->m_vecSoftMeshes.push_back(pSoftMesh);
	}

	this->m_UpdateCenterPos();

	return m_pSoftModel;
}

void cSoftBody::UpdateSoftBody(const glm::vec3& velocity, float deltaTime)
{
	for (sSoftMesh* softMesh : m_vecSoftMeshes)
	{
		for (sParticle* curParticle : softMesh->vecParticles)
		{
			glm::vec3 current_pos = curParticle->position;
			glm::vec3 old_pos = curParticle->old_position;

			// This is the actual Verlet integration step	
			curParticle->position += velocity * deltaTime;

			curParticle->old_position = current_pos;

			this->m_CleanZeros(curParticle->position);
			this->m_CleanZeros(curParticle->old_position);
		}
	}

	if (bAttach)
	{
		for (int index = 0; index != m_vecAttach.size(); index++)
		{
			m_vecSoftMeshes[0]->vecParticles[m_vecAttach[index]->particleIndex]->position = m_vecAttach[index]->attachPos;
		}
	}

	this->m_UpdateCenterPos();
	this->m_SatisfyConstraints();
	this->m_UpdateVertexPositions();
	this->m_UpdateVertexNormals();
	this->m_UpdateDrawInfo();

	return;
}

bool cSoftBody::AttachParticle(const glm::vec3& oldPos, const glm::vec3& attachPos)
{
	for (int index = 0; index != m_vecSoftMeshes[0]->vecParticles.size(); index++)
	{
		if (m_vecSoftMeshes[0]->vecParticles[index]->position == oldPos)
		{
			sAttachedParticles* attach = new sAttachedParticles();
			attach->particleIndex = index;
			attach->oldPosition = oldPos;
			attach->attachPos = attachPos;
			m_vecAttach.push_back(attach);

			this->bAttach = true;

			return true;
		}
	}
	return false;
}

bool cSoftBody::AttachParticle(int index, const glm::vec3& attachPos)
{
	if (index >= m_vecSoftMeshes[0]->vecParticles.size())
	{
		return false;
	}

	sAttachedParticles* attach = new sAttachedParticles();
	attach->particleIndex = index;
	attach->oldPosition = m_vecSoftMeshes[0]->vecParticles[index]->old_position;
	attach->attachPos = attachPos;
	m_vecAttach.push_back(attach);

	this->bAttach = true;

	return true;
}

bool cSoftBody::AttachParticle(int index)
{
	if (index >= m_vecSoftMeshes[0]->vecParticles.size())
	{
		return false;
	}

	sAttachedParticles* attach = new sAttachedParticles();
	attach->particleIndex = index;
	attach->oldPosition = m_vecSoftMeshes[0]->vecParticles[index]->old_position;
	attach->attachPos = m_vecSoftMeshes[0]->vecParticles[index]->old_position;
	m_vecAttach.push_back(attach);

	this->bAttach = true;

	return true;
}

void cSoftBody::FastenParticles()
{
	this->bAttach = true;
}
void cSoftBody::UnfastenParticles()
{
	this->bAttach = false;
}

void cSoftBody::BrokeRandConstr()
{
	//for (sConstraint* pConstr : m_vecSoftMeshes[0]->vecConstraints)
	//{
	//	pConstr->tightnessFactor = 2.0f;
	//}

	//int size = m_vecSoftMeshes[0]->vecConstraints.size();
	//int randIndex = rand() % size - 1;

	//for (int i = randIndex - 50; i != randIndex + 50; i++)
	//{
	//	m_vecSoftMeshes[0]->vecConstraints[i]->tightnessFactor = 2.0f;
	//	m_vecSoftMeshes[0]->vecConstraints[i]->bIsActive = false;
	//	//delete m_vecSoftMeshes[0]->vecConstraints[i]->pParticleA->pVertex;
	//	//delete m_vecSoftMeshes[0]->vecConstraints[i]->pParticleB->pVertex;
	//	std::vector<sConstraint*>::iterator it = m_vecSoftMeshes[0]->vecConstraints.begin() + i;
	//	m_vecSoftMeshes[0]->vecConstraints.erase(it);
	//}
}

glm::vec3 cSoftBody::GetCenterPos(void) const
{
	return this->m_center;
}

void cSoftBody::CreateRandomBracing(int numberOfBraces, float minDistance)
{
	for (unsigned int count = 0; count != numberOfBraces; count++)
	{
		// Assume the distance is OK
		bool bKeepLookingForParticles = false;

		do
		{
			// Assume the distance is OK
			bKeepLookingForParticles = false;

			unsigned int particleIndex1 = g_pRandUtils->GetRandomInt(0, m_vecSoftMeshes[0]->vecParticles.size()-1);
			unsigned int particleIndex2 = g_pRandUtils->GetRandomInt(0, m_vecSoftMeshes[0]->vecParticles.size()-1);

			sParticle* pParticle1 = m_vecSoftMeshes[0]->vecParticles[particleIndex1];
			sParticle* pParticle2 = m_vecSoftMeshes[0]->vecParticles[particleIndex2];
			float distBetween = this->m_Distance(pParticle1, pParticle2);

			// Distance OK?
			if (distBetween < minDistance)
			{
				// No
				bKeepLookingForParticles = true;
			}
			else
			{
				// Distance is OK, so make a constraint
				sConstraint* pBracingConstraint = new sConstraint();
				pBracingConstraint->pParticleA = pParticle1;
				pBracingConstraint->pParticleB = pParticle2;
				pBracingConstraint->restLength = this->m_Distance(pBracingConstraint->pParticleA, pBracingConstraint->pParticleB);

				m_vecSoftMeshes[0]->vecConstraints.push_back(pBracingConstraint);
			}

		} while (bKeepLookingForParticles);

	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////////////

bool cSoftBody::m_UpdateDrawInfo(void)
{
	if (! g_pModelManager->UpdateVertexBuffers(m_pSoftModel))
	{
		return false;
	}

	return true;
}

void cSoftBody::m_UpdateCenterPos(void)
{
	glm::vec3 minExtentXYZ = glm::vec3(FLT_MAX);
	glm::vec3 maxExtentXYZ = glm::vec3(-FLT_MAX);

	for (sSoftMesh* softMesh : m_vecSoftMeshes)
	{
		for (sParticle* curParticle : softMesh->vecParticles)
		{
			if (curParticle->position.x < minExtentXYZ.x) { minExtentXYZ.x = curParticle->position.x; }
			if (curParticle->position.y < minExtentXYZ.y) { minExtentXYZ.y = curParticle->position.y; }
			if (curParticle->position.z < minExtentXYZ.z) { minExtentXYZ.z = curParticle->position.z; }

			if (curParticle->position.x > maxExtentXYZ.x) { maxExtentXYZ.x = curParticle->position.x; }
			if (curParticle->position.y > maxExtentXYZ.y) { maxExtentXYZ.y = curParticle->position.y; }
			if (curParticle->position.z > maxExtentXYZ.z) { maxExtentXYZ.z = curParticle->position.z; }
		}
	}

	glm::vec3 delta = maxExtentXYZ - minExtentXYZ;
	this->m_center = minExtentXYZ + delta / 2.0f;

	return;
}

void cSoftBody::m_SatisfyConstraints(void)
{
	const unsigned int NUM_ITERATIONS = 5;

	for (unsigned int iteration = 0; iteration != NUM_ITERATIONS; iteration++)
	{
		for (sSoftMesh* softMesh : m_vecSoftMeshes)
		{
			for (sConstraint* pCurConstraint : softMesh->vecConstraints)
			{
				if (pCurConstraint->bIsActive)
				{
					cSoftBody::sParticle* pX1 = pCurConstraint->pParticleA;
					cSoftBody::sParticle* pX2 = pCurConstraint->pParticleB;

					glm::vec3 delta = pX2->position - pX1->position;

					float deltaLength = glm::length(delta);


					float diff = (deltaLength - pCurConstraint->restLength) / deltaLength;

					// If we were having this 'tear' or break apart, 
					//	you could check some maximum length and if it's 'too long'
					//	then the constraint 'breaks'
					// Handle this by:
					// - Setting a bool (where it doesn't check the constraint any more)
					// - Remove the constraint (but removing from a vector is sketchy...)

					//if ( diff > 0.1f )
					//{
					//	pCurConstraint->bIsActive = false;
					//}

					// Making this non-one, will change how quickly the objects move together
					// For example, making this < 1.0 will make it "bouncier"
					pCurConstraint->tightnessFactor = 1.0f;

					pX1->position += delta * 0.5f * diff * pCurConstraint->tightnessFactor;
					pX2->position -= delta * 0.5f * diff * pCurConstraint->tightnessFactor;

					this->m_CleanZeros(pX1->position);
					this->m_CleanZeros(pX2->position);
				}
			}
		}	
	}

	return;
}

void cSoftBody::m_UpdateVertexNormals(void)
{
	for (cMeshDrawInfo* meshInfo : m_pSoftModel->vpMeshesOfModel)
	{
		// Clear the existing normals because they are invalid
		for (unsigned int index = 0; index < meshInfo->numberOfVertices; index++)
		{
			meshInfo->vertices[index].normal.x = 0.0f;
			meshInfo->vertices[index].normal.y = 0.0f;
			meshInfo->vertices[index].normal.z = 0.0f;
		}

		for (unsigned int triIndex = 0; triIndex < meshInfo->numberOfIndices;)
		{
			unsigned int vertAIndex = meshInfo->indices[triIndex++];
			unsigned int vertBIndex = meshInfo->indices[triIndex++];
			unsigned int vertCIndex = meshInfo->indices[triIndex++];

			glm::vec3 vertA = meshInfo->vertices[vertAIndex].position;
			glm::vec3 vertB = meshInfo->vertices[vertBIndex].position;
			glm::vec3 vertC = meshInfo->vertices[vertCIndex].position;

			glm::vec3 triangleEdgeAtoB = vertB - vertA;
			glm::vec3 triangleEdgeAtoC = vertC - vertA;

			glm::vec3 theNormal = glm::cross(triangleEdgeAtoB, triangleEdgeAtoC);
			theNormal = glm::normalize(theNormal);

			meshInfo->vertices[vertAIndex].normal.x += theNormal.x;
			meshInfo->vertices[vertAIndex].normal.y += theNormal.y;
			meshInfo->vertices[vertAIndex].normal.z += theNormal.z;
													
			meshInfo->vertices[vertBIndex].normal.x += theNormal.x;
			meshInfo->vertices[vertBIndex].normal.y += theNormal.y;
			meshInfo->vertices[vertBIndex].normal.z += theNormal.z;
													
			meshInfo->vertices[vertCIndex].normal.x += theNormal.x;
			meshInfo->vertices[vertCIndex].normal.y += theNormal.y;
			meshInfo->vertices[vertCIndex].normal.z += theNormal.z;
		}

		// Now normalize the accumulated normals
		for (unsigned int index = 0; index < meshInfo->numberOfVertices; index++)
		{
			
			glm::vec3 tempNormal = glm::vec3(meshInfo->vertices[index].normal.x,
											meshInfo->vertices[index].normal.y,
											meshInfo->vertices[index].normal.z);

			tempNormal = glm::normalize(tempNormal);

			glm::vec3 vertPos = meshInfo->vertices[index].position;
			glm::vec3 direction = vertPos - m_center;

			float dot = glm::dot(tempNormal, direction);
			glm::vec3 theNormal = glm::vec3(0.0f);

			theNormal = tempNormal;

			if (dot < 0.0f && bIsConvert)
			{
				theNormal = -tempNormal;
			}

			meshInfo->vertices[index].normal.x = theNormal.x;
			meshInfo->vertices[index].normal.y = theNormal.y;
			meshInfo->vertices[index].normal.z = theNormal.z;
		}
	}

	return;
}

void cSoftBody::m_UpdateVertexPositions(void)
{
	for (sSoftMesh* pSoftMesh : m_vecSoftMeshes)
	{
		for (unsigned int index = 0; index < pSoftMesh->vecParticles.size(); index++)
		{
			pSoftMesh->vecParticles[index]->pVertex->position.x = pSoftMesh->vecParticles[index]->position.x;
			pSoftMesh->vecParticles[index]->pVertex->position.y = pSoftMesh->vecParticles[index]->position.y;
			pSoftMesh->vecParticles[index]->pVertex->position.z = pSoftMesh->vecParticles[index]->position.z;
		}
	}

	return;
}

void cSoftBody::m_CleanZeros(glm::vec3& value)
{
	const float minFloat = 1.192092896e-07f;
	if ((value.x < minFloat) && (value.x > -minFloat))
	{
		value.x = 0.0f;
	}
	if ((value.y < minFloat) && (value.y > -minFloat))
	{
		value.y = 0.0f;
	}
	if ((value.z < minFloat) && (value.z > -minFloat))
	{
		value.z = 0.0f;
	}
}

void cSoftBody::m_CopyModelInfo(cModel* oldModel)
{
	cModel* newModel = new cModel();
	newModel->modelName = "Soft_" + oldModel->modelName + std::to_string(newModel->GetUniqueID());
	
	for (cMeshDrawInfo* oldMeshInfo : oldModel->vpMeshesOfModel)
	{
		cMeshDrawInfo* newMeshInfo = new cMeshDrawInfo();

		newMeshInfo->numberOfVertices = oldMeshInfo->numberOfVertices;

		for (unsigned int i = 0; i < oldMeshInfo->vertices.size(); i++)
		{
			newMeshInfo->vertices.push_back(oldMeshInfo->vertices[i]);
		}

		newMeshInfo->numberOfIndices = oldMeshInfo->numberOfIndices;

		for (unsigned int index : oldMeshInfo->indices)
		{
			newMeshInfo->indices.push_back(index);
		}

		newMeshInfo->numberOfTriangles = oldMeshInfo->numberOfTriangles;

		for (sTexture* texture : oldMeshInfo->textures)
		{
			newMeshInfo->textures.push_back(texture);
		}

		newModel->vpMeshesOfModel.push_back(newMeshInfo);
	}

	if (g_pModelManager->LoadModelIntoVAO(newModel))
	{
		this->m_pSoftModel = newModel;
	}

	return;
}

float cSoftBody::m_Distance(sParticle* pPartA, sParticle* pPartB)
{
	return glm::distance(pPartA->position, pPartB->position);
}
