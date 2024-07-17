#pragma once

#include "../objects/object_3D/cObject3D.h"

#include <string>
#include <vector>
#include <map>

class cPoolManager
{
public:
	~cPoolManager();
	static cPoolManager* GetPoolManager();

	bool CreateObjectPool(std::string& poolName);
	bool DeleteObjectPool(std::string& poolName);

	bool AddObjectToPool(std::string& poolName, cObject3D* object);
	cObject3D* GetObjectFromPool(std::string& poolName, std::string& friendlyObject);
	bool DeleteObjectFromPool(std::string& poolName, std::string& friendLyObject);
	
	bool RenderPool(std::string poolName);

private:
	cPoolManager();
	static cPoolManager* m_pPoolManager;

	std::map<std::string, /*pool_name*/ std::vector<cObject3D*> /*objects*/> m_mapPools;
	std::map< std::string, std::vector<cObject3D*> >::iterator itObjectPool;
};

