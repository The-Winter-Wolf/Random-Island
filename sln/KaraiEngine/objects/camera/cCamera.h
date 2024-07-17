#pragma once

#include "../../global/common.h"
#include "../../world/player/cPlayer.h"

enum eCameraMods
{
	STATIC,
	DYNAMIC,
	FIRST_PERSON,
	THIRD_PERSON,
};

enum eCameraEvents
{
	CAMERA_FORWARD,
	CAMERA_BACKWARD,
	CAMERA_RIGHT,
	CAMERA_LEFT,
	CAMERA_UP,
	CAMERA_DOWN,
	CAMERA_BOOST,
};

class cCamera
{
public:
	~cCamera();
	static cCamera* GetCamera(GLFWwindow* window);

	void Update();

	bool SetCameraMod(eCameraMods cameraMod);
	bool SetCameraMod(int cameraMod);
	eCameraMods GetCameraMod(void) const;

	bool SetPlayer(cPlayer* player);
	cPlayer* GetPlayer(void);

	void SetPosition(const glm::vec3& position);
	glm::vec3 GetPosition(void) const;

	void SetDirection(const glm::vec3& direction);
	glm::vec3 GetDirection(void) const;

	glm::vec3 GetUpVec(void) const;
	glm::vec3 GetRightVec(void) const;

	void SetAngles(const glm::vec2& angles);
	glm::vec2 GetAngles(void) const;

	void SetVelocity(const glm::vec3& velocity);
	glm::vec3 GetVelocity(void) const;

	void SetFOVMinMax(double FOV, double minFOV = -1.0f, double maxFOV = -1.0f);
	void SetFOVMinMax(const glm::vec3& fovMinMax);
	glm::vec3 GetFOVMinMax(void) const;

	void SetPitchMinMax(float pitch, float minPitch = -1.0f, float maxPitch = -1.0f);
	void SetPitchMinMax(const glm::vec3& pitchMinMax);
	glm::vec3 GetPitchMinMax(void) const;

	void SetYawMinMax(float yaw, float minYaw = -1.0f, float maxYaw = -1.0f);
	void SetYawMinMax(const glm::vec3& yawMinMax);
	glm::vec3 GetYawMinMax(void) const;

	void SetPitchYawSpeed(const glm::vec2& pitchYawSpeed);
	glm::vec2 GetPitchYawSpeed(void) const;

	void SetDistMinMax(float distance, float minDist = -1.0f, float maxDist = -1.0f);
	void SetDistMinMax(const glm::vec3& distMinMax);
	glm::vec3 GetDistMinMax(void) const;

	void SetMoveSpeed(float speed);
	float GetMoveSpeed(void) const;

	void SetSpeedScale(float scale);
	float GetSpeedScale(void) const;

	void MoveCamera(eCameraEvents direction, float deltaTime);
	void ScrollAction(double yScrollPos);

	void SetCursorPos(double xPos, double yPos);

	void SetBufferSize(double width, double height);
	void GetBufferSize(double& width, double& height);

	void SetWindowSize(int width, int height);
	void GetWindowSize(int& width, int& height);

	glm::mat4 GetMatView();
	glm::mat4 GetMatProjection();

	bool bIsBackward = true;
	bool bIsForward = true;
	bool bIsRight = true;
	bool bIsLeft = true;

private:
	cCamera(GLFWwindow* window);
	static cCamera* m_pCamera;

	void m_UpdateBuffer();
	GLFWwindow* m_pWindow;

	glm::dvec2 m_MousePos;
	glm::dvec2 m_BufferSize;
	glm::dvec2 m_BufferCenter;

	cPlayer* m_pPlayer;
	glm::vec3 m_playerPos, m_Offset;

	eCameraMods m_eCameraMod;

	glm::mat4 m_view, m_projection;

	glm::vec3 m_Position, m_Direction;

	glm::vec3 m_Up, m_Right;

	glm::vec3 m_Velocity;

	float m_OldTheta;

	bool bFirstMouse;

	double m_FoV, m_MinFoV, m_MaxFoV;

	float m_Speed, m_MoveSpeed, m_SpeedScale;

	float m_Distance, m_MaxDistance, m_MinDistance;

	float m_Yaw, m_MinYaw, m_MaxYaw, m_YawSpeed;

	float m_Pitch, m_MinPitch, m_MaxPitch, m_PitchSpeed;
};

