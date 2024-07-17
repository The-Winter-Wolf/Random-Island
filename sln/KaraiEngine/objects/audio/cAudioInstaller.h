#pragma once
#include "cAudioManager.h"

#include "../object_3D/cObject3D.h"

class cAudioInstaller
{
public:
	~cAudioInstaller();
	static cAudioInstaller* GetAudioInstaller();

	void Init();
	void Update(float dt);

	void SetPaused(int channelID, bool true_false);
	bool GetPaused(int channelID);

	int minSelect;
	int maxSelect;

	float selectVolume;
	float selectPitch;
	float selectPan;

	bool isReverb;
	bool isChorus;
	bool isDistortion;
	bool isHighPass;
	bool isLowPass;

private:
	cAudioInstaller();
	static cAudioInstaller* m_pAudioInstaller;

	cAudioManager* m_AudioManager;

	std::vector<int> channels;

	void m_CreateDebugSphere(glm::vec3 pos, glm::vec3 color, int id);
	void m_CreateDebugMesh(std::vector < glm::vec3> vertices, cObject3D* originMesh);
	void m_BuildPolygonFromModel(cModel* modelInfo, std::vector<glm::vec3>& vert);

	void m_CreateWallsFromModel(std::string friendlyName);
	void m_CreateWallsManually();
};
