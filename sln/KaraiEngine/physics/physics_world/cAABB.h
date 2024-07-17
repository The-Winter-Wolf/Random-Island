#pragma once

#include "../../objects/object_3D/cObject3D.h"

#include <glm/glm.hpp>

#include <math.h>
#include <string>
#include <vector>
#include <map>

struct sTriangle
{
	std::string meshFriendlyName;
	glm::vec3 vertices[3];
	float GetSideLength(int sideIndex);
};

class cAABB
{
public:
	cAABB();
	~cAABB();

	glm::vec3 minXYZ;
	glm::vec3 maxXYZ;
	glm::vec3 GetCentreXYZ(void);
	glm::vec3 GetExtentsXYZ(void);

	unsigned int GetLocationIndex(void);

	static unsigned int GetLocationIndex(glm::vec3 minXYZ, glm::vec3 externXYZ);

	std::map< unsigned int /*index*/, cAABB* > mapChild_pAABBs;

	std::vector< sTriangle > vecTriangles;

	std::vector<cObject3D*> vecDynamicObjects;
	std::vector<cObject3D*> vecKinematicObjects;
	std::vector<glm::vec3> vecKinematicVertices;
};
