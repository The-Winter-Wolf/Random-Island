#pragma once

#include "../global/common.h"

class cSceneManager
{
public:
	~cSceneManager();
	static cSceneManager* GetSceneManager();

	void Init(int numScene, GLFWwindow* window);

	void Update(int numScene, float deltaTime);

	void Input(int event);

private:
	cSceneManager();
	static cSceneManager* m_pSceneManager;

	GLFWwindow* m_Window;

	void m_DefaultSceneInit();
};

