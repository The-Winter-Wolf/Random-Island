#pragma once

#include "../world/enemy/cEnemyBuilder.h"
#include "../global/common.h"

class cScene01
{
public:
	cScene01();
	~cScene01();

	void Init(GLFWwindow* window);

	void Update(float deltaTime);

	void InputKey(int event);

private:
	GLFWwindow* m_Window;
	cEnemyBuilder* m_pEnemyBuilder;

	std::vector<iEnemy*> m_vpEnemies;

	void m_HandLoad();

	void m_InitPlayer();
	void m_InitEnemies();

};
