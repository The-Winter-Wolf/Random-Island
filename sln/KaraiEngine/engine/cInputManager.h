#pragma once

#include "../global/common.h"
#include <glm/glm.hpp>

class cInputManager
{
public:
	~cInputManager();
	static cInputManager* GetInputManager(GLFWwindow* window);

	void KeyCallback(int key, int scancode, int action, int mods);

	void MouseCallback(int button, int action, int mods);

	void ScrollCallback(double xoffset, double yoffset);

	void KeyboardInput(float deltaTime);

	bool TAB_MODE;
	bool CAPS_LOCK_MODE;

private:

	cInputManager(GLFWwindow* window);
	static cInputManager* m_pInputManager;

	GLFWwindow* window;

	bool tempTab;		// for LIGHT_MOVE_MODE

	bool tempCapsLock;	// for OBJECT_MOVE_MODE


};