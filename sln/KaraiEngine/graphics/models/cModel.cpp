#include "cModel.h"

unsigned int cModel::m_nextUniqueID = cModel::FIRST_UNIQUE_ID;

cModel::cModel()
{
	this->m_UniqueID = cModel::m_nextUniqueID;
	cModel::m_nextUniqueID++;

	unsigned int MatModelBufferID;
	matrices.push_back(glm::mat4(1.0f));
}

cModel::~cModel()
{
	for (unsigned int i = 0; i != vpMeshesOfModel.size(); i++)
	{
		delete vpMeshesOfModel[i];
		vpMeshesOfModel[i] = nullptr;
	}
}

std::map<std::string, sBoneInfo>& cModel::GetBoneInfoMap()
{
	return this->m_mapModelBoneInfo;
}

int& cModel::GetBoneCount()
{
	return this->m_ModelBoneCounter;
}

unsigned int cModel::GetUniqueID(void)
{
	return this->m_UniqueID;
}


cMeshDrawInfo::cMeshDrawInfo()
{
	this->bIsDynamic = false;

	this->VAO_ID = 0;

	this->VertexBufferID = 0;
	this->numberOfVertices = 0;

	this->MatModelBufferID = 0;
	this->numberOfMatrices = 1;

	this->IndexBufferID = 0;
	this->numberOfIndices = 0;

	this->numberOfTriangles = 0;
		
	this->maxExtents_XYZ = glm::vec3(0.0f);
	this->minExtents_XYZ = glm::vec3(0.0f);
	this->deltaExtents_XYZ = glm::vec3(0.0f);

	this->maxExtent = 0.0f;

	return;
}

cMeshDrawInfo::~cMeshDrawInfo() {}

void cMeshDrawInfo::CalcExtents(void)
{
	if ( this->vertices.size() == NULL)
	{
		return;
	}

	sVertex* pCurrentVert = &(this->vertices[0]);
	
	this->minExtents_XYZ.x = pCurrentVert->position.x;
	this->minExtents_XYZ.y = pCurrentVert->position.y;
	this->minExtents_XYZ.z = pCurrentVert->position.z;

	this->maxExtents_XYZ.x = pCurrentVert->position.x;
	this->maxExtents_XYZ.y = pCurrentVert->position.y;
	this->maxExtents_XYZ.z = pCurrentVert->position.z;

	for ( unsigned int vertIndex = 0; vertIndex != this->numberOfVertices; vertIndex++ )
	{
		sVertex* pCurrentVert = &(this->vertices[vertIndex]);

		if (pCurrentVert->position.x < this->minExtents_XYZ.x) { this->minExtents_XYZ.x = pCurrentVert->position.x; }
		if (pCurrentVert->position.y < this->minExtents_XYZ.y) { this->minExtents_XYZ.y = pCurrentVert->position.y; }
		if (pCurrentVert->position.z < this->minExtents_XYZ.z) { this->minExtents_XYZ.z = pCurrentVert->position.z; }

		if (pCurrentVert->position.x > this->maxExtents_XYZ.x) { this->maxExtents_XYZ.x = pCurrentVert->position.x; }
		if (pCurrentVert->position.y > this->maxExtents_XYZ.y) { this->maxExtents_XYZ.y = pCurrentVert->position.y; }
		if (pCurrentVert->position.z > this->maxExtents_XYZ.z) { this->maxExtents_XYZ.z = pCurrentVert->position.z; }

	}

	this->deltaExtents_XYZ.x = this->maxExtents_XYZ.x - this->minExtents_XYZ.x;
	this->deltaExtents_XYZ.y = this->maxExtents_XYZ.y - this->minExtents_XYZ.y;
	this->deltaExtents_XYZ.z = this->maxExtents_XYZ.z - this->minExtents_XYZ.z;

	this->maxExtent = this->deltaExtents_XYZ.x;
	if (this->maxExtent < this->deltaExtents_XYZ.y) { this->maxExtent = this->deltaExtents_XYZ.y; }
	if (this->maxExtent < this->deltaExtents_XYZ.z) { this->maxExtent = this->deltaExtents_XYZ.z; }

	return;
}

