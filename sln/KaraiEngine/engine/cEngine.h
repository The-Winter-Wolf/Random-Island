#pragma once

#include "../global/common.h"

class cEngine
{
public:
	cEngine();
	~cEngine();

	bool Initialize(GLFWwindow* window);

	void Update();

	void Render();

	void WindowResize(int width, int height);

	void CursorCallback(double xpos, double ypos);

	void ScrollCallback(double xoffset, double yoffset);

	void MouseCallback(int button, int action, int mods);

	void KeyCallback(int key, int scancode, int action, int mods);

	void m_SQLiteExample();

private:
	GLFWwindow* m_window;

	int m_width;
	int m_height;
	float m_deltaTime;
	float m_currentTime;
};

