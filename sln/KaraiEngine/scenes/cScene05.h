#pragma once

#include "../global/common.h"
#include "../objects/object_3D/cObject3D.h"

class cScene05
{
public:
	cScene05();
	~cScene05();

	void Init(GLFWwindow* window);

	void Update(float deltaTime);

	void InputKey(int event);

	void TurnOnOff();

private:
	GLFWwindow* m_Window;

	cObject3D* pScreen1;
	cObject3D* pScreen2;

	bool m_bScreen01TurnOn;
	bool m_bScreen02TurnOn;

	bool m_bIsAberration01;
	bool m_bIsNightVision01;
	bool m_bIsBlurScreenBasic01;
	bool m_bIsBlurScreenSlower01;
	bool m_bIsBlurScreenFaster01;

	bool m_bIsAberration02;
	bool m_bIsNightVision02;
	bool m_bIsBlurScreenBasic02;
	bool m_bIsBlurScreenSlower02;
	bool m_bIsBlurScreenFaster02;

	cObject3D* warrok01;
	cObject3D* warrok02;
	cObject3D* warrok03;

	cObject3D* vampire01;
	cObject3D* vampire02;
	cObject3D* vampire03;

	void m_HandLoad();

	void m_InitFBO();
	void m_InitPlayer();
	void m_InitEnemies();

	void m_UpdateScreen01();
	void m_UpdateScreen02();

	void m_DisplayScreen01();
	void m_DisplayScreen02();

	void m_UpdateCamera();
};
