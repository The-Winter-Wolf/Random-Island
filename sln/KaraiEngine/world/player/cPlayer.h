#pragma once

#include "../../objects/object_3D/cObject3D.h"

enum ePlayerEvents
{
	PLAYER_FORWARD,
	PLAYER_BACKWARD,
	PLAYER_RIGHT,
	PLAYER_LEFT,
	PLAYER_JUMP,
	PLAYER_SQUAT,
	PLAYER_BOOST,
	PLAYER_DASH,
	PLAYER_LEFT_ATTACK,
	PLAYER_RIGHT_ATTACK,
};

class cPlayer
{
public:
	cPlayer();
	~cPlayer();

	void InitPlayer();
	void MovePlayer(ePlayerEvents event, float eventValue);
	void UpdatePlayer(float deltaTime);

	void SetPlayerObject(cObject3D* playerObj);
	cObject3D* GetPlayerObject();
	glm::vec3 GetPlayerPos();

	bool bUseFear;

private:
	cObject3D* m_pObject;

	float m_speed;
	int m_HP, m_MP, m_XP;

/////////////// Flags //////////////////

	bool bIsLeft, bIsRight;
	bool bIsJump, bIsSquat;
	bool bIsAttack, bIsDash;
	bool bIsForward, bIsBackward;
	bool bIsLeftAttack, bIsRightAttack;

	bool bJumpMovements;
	bool bHasAnimations;

////////////////////////////////////////

	float m_maxSpeed;
	float m_moveSpeed;
	float m_jumpSpeed;
	float m_squatSpeed;
	float m_boostSpeed;

	glm::vec3 m_playerPos;

	float m_jumpTime;

	void m_InitAnimation();
	void m_Slowdown(float deltaTime);
	void m_UpdateAnimation(float deltaTime);
};

