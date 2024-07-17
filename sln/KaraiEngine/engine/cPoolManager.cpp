#include "cPoolManager.h"

#include "../global/global.h"

#include "cRenderManager.h"
extern cRenderManager* g_pRenderManager;


cPoolManager* cPoolManager::m_pPoolManager = nullptr;

cPoolManager* cPoolManager::GetPoolManager()
{
	if (cPoolManager::m_pPoolManager == nullptr)
	{
		cPoolManager::m_pPoolManager = new cPoolManager();
	}
	return cPoolManager::m_pPoolManager;
}

cPoolManager::cPoolManager(){}

cPoolManager::~cPoolManager()
{
	for (itObjectPool = m_mapPools.begin(); itObjectPool != m_mapPools.end(); itObjectPool++)
	{
		for (unsigned int index = 0; index != itObjectPool->second.size(); index++)
		{
			std::vector<cObject3D*>::iterator it = itObjectPool->second.begin() + index;
			delete itObjectPool->second[index];
			itObjectPool->second[index] = nullptr;
			itObjectPool->second.erase(it);
			index--;
		}
	}
}

bool cPoolManager::CreateObjectPool(std::string& poolName)
{
	std::vector<cObject3D*> objectsPool;
	this->m_mapPools[poolName] = objectsPool;
	return true;
}

bool cPoolManager::DeleteObjectPool(std::string& poolName)
{
	itObjectPool = this->m_mapPools.find(poolName);

	if (itObjectPool == m_mapPools.end())
	{
		return false;
	}

	m_mapPools.erase(itObjectPool);
	return true;
}

bool cPoolManager::AddObjectToPool(std::string& poolName, cObject3D* object)
{
	itObjectPool = this->m_mapPools.find(poolName);

	if (itObjectPool == m_mapPools.end())
	{
		return false;
	}

	itObjectPool->second.push_back(object);
	return true;
}

cObject3D* cPoolManager::GetObjectFromPool(std::string& poolName, std::string& friendlyObject)
{
	itObjectPool = this->m_mapPools.find(poolName);

	if (itObjectPool == m_mapPools.end())
	{
		return nullptr;
	}

	for (unsigned int index = 0; index != itObjectPool->second.size(); index++)
	{
		if (itObjectPool->second[index]->friendlyName == friendlyObject)
		{
			return itObjectPool->second[index];
		}
	}

	return nullptr;
}

bool cPoolManager::DeleteObjectFromPool(std::string& poolName, std::string& friendlyObject)
{
	itObjectPool = this->m_mapPools.find(poolName);

	if (itObjectPool == m_mapPools.end())
	{
		return false;
	}

	for (unsigned int index = 0; index != itObjectPool->second.size(); index++)
	{
		if (itObjectPool->second[index]->friendlyName == friendlyObject)
		{	
			std::vector<cObject3D*>::iterator it = itObjectPool->second.begin() + index;
			itObjectPool->second.erase(it);
			return true;
		}
	}

	return false;
}

bool cPoolManager::RenderPool(std::string poolName)
{
	itObjectPool = this->m_mapPools.find(poolName);

	if (itObjectPool == m_mapPools.end())
	{
		return false;
	}

	for (unsigned int index = 0; index != itObjectPool->second.size(); index++)
	{
		glm::mat4 matModel = glm::mat4(1.0);
		g_pRenderManager->DrawObject(itObjectPool->second[index], matModel);
	}
	
	return true;
}