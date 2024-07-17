#pragma once

#include "../global/common.h"
#include "../objects/object_3D/cObject3D.h"

#include "../world/enemy/cBigEnemy.h"

class cScene04
{
public:
	cScene04();
	~cScene04();

	void Init(GLFWwindow* window);

	void Update(float deltaTime);

	void InputKey(int event);

private:
	GLFWwindow* m_Window;

	bool m_HandLoad();

	bool m_InitPlayer();
	bool m_InitEnemies();

	bool m_UpdateCamera();

	std::vector<cBigEnemy*> m_vpEnemies;
	static const unsigned int NUM_OF_ENEMIES;
};