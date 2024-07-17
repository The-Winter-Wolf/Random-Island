#pragma once

#include <string>
#include <glm/glm.hpp>

class cInstance
{
public:
	cInstance();
	~cInstance();

	void InitInstancing(const std::string& objectName, int numOfInstances, float scaleFactor, 
				float spawnRadius, float avoidRadius, float minBetweenDist, bool terrainNormals, bool bHasCollision);

private:

};

