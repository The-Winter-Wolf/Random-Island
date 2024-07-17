#pragma once

#include "../storage/cConfigData.h"

class cLoadManager
{
public:
	~cLoadManager();
	static cLoadManager* GetLoadManager();

	void LoadModels(int numLoad = 0);
	void InitModels(int numInit = 0);

	//void ObjectToSettings(std::vector<sObject>& vObjects);
	//void ObjectFromSettings(const std::vector<sObject>& vObjects);

	void SetShaderId(int shadID);

private:
	cLoadManager();
	static cLoadManager* m_pLoadManager;

	int shaderID;
};

