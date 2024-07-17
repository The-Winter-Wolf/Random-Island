#include "cAABB.h"


cAABB::cAABB()
{
}

cAABB::~cAABB()
{
	std::map< unsigned int /*index*/, cAABB* >::iterator itAABB;
	for (itAABB = mapChild_pAABBs.begin(); itAABB != mapChild_pAABBs.end(); itAABB++)
	{
		delete itAABB->second;
	}
}

glm::vec3 cAABB::GetExtentsXYZ(void)
{
	return maxXYZ - minXYZ;
}

glm::vec3 cAABB::GetCentreXYZ(void)
{
	return (maxXYZ - minXYZ)/2.0f;
}

unsigned int cAABB::GetLocationIndex(void)
{
	unsigned int theX = static_cast<unsigned int> ( floor(this->minXYZ.x / GetExtentsXYZ().x) );
	unsigned int theY = static_cast<unsigned int> ( floor(this->minXYZ.y / GetExtentsXYZ().y) );
	unsigned int theZ = static_cast<unsigned int> ( floor(this->minXYZ.z / GetExtentsXYZ().z) );

	unsigned int theIndex = (theX * 1000 * 1000) + (theY * 1000) + (theZ);

	if (this->minXYZ.x < 0)
	{
		theIndex += 100'000'000;
	}
	if (this->minXYZ.y < 0)
	{
		theIndex += 100'000;
	}
	if (this->minXYZ.z < 0)
	{
		theIndex += 100;
	}
	return theIndex;
}

unsigned int cAABB::GetLocationIndex(glm::vec3 minXYZ, glm::vec3 externXYZ)
{
	unsigned int theX = static_cast<unsigned int> ( floor(minXYZ.x / externXYZ.x) );
	unsigned int theY = static_cast<unsigned int> ( floor(minXYZ.y / externXYZ.y) );
	unsigned int theZ = static_cast<unsigned int> ( floor(minXYZ.z / externXYZ.z) );

	unsigned int theIndex = (theX * 1000 * 1000) + (theY * 1000) + (theZ);

	if (minXYZ.x < 0)
	{
		theIndex += 100'000'000;
	}
	if (minXYZ.y < 0)
	{
		theIndex += 100'000;
	}
	if (minXYZ.z < 0)
	{
		theIndex += 100;
	}
	return theIndex;
}

float sTriangle::GetSideLength(int sideIndex)
{
	float dist0 = glm::distance(vertices[0], vertices[1]);
	float dist1 = glm::distance(vertices[0], vertices[2]);
	float dist2 = glm::distance(vertices[1], vertices[2]);

	if (sideIndex == 0)
		return dist0;
	else if (sideIndex == 1)
		return dist1;
	else if (sideIndex == 2)
		return dist2;
	else return 0.0f;
}
