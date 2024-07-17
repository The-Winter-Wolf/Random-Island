#pragma once
#include "cAABB.h"

#include "../../objects/object_3D/cObject3D.h"

class cAABBsWorld
{
public:
	cAABBsWorld(const glm::vec3& childNum, const glm::vec3& childExtent, const glm::vec3& shift, bool negativeSide);
	~cAABBsWorld();

	void PrepareAABBScene();

	cAABB* AddObjectToAABBScene(cObject3D* physObject);

	cAABB* GetAABBFromPos(const glm::vec3& position);

	bool IsTriangleInAABB(sTriangle theTri, cAABB* pAABB);

	void CreateAABBdebSpheres();

	void ProcessABBBsWorld(cAABB& parentAABB,						// The parent containing the AABBS
							std::vector<sTriangle>& vecTriangles,	// Triangles in model
							glm::ivec3& numberBoxesPerSide,			// eg: 10x10x10 or whatever
							glm::vec3& extentLength);				// How big is the ENTIRE 'parent' AABB
																	// (i.e. how big are each 'child' AABB
private:
	glm::vec3 m_childExtents;
	glm::vec3 m_childNum;
	glm::vec3 m_shift;
	bool m_negativeSide;

	std::map< unsigned int /*index*/, cAABB* > m_map_pAABBS_by_ID;
};