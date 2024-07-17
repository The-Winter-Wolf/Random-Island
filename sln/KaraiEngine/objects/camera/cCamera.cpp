#include "cCamera.h"

#include "../../graphics/shaders/cShaderManager.h"
extern cShaderManager* g_pShaderManager;

#include <glm/gtc/type_ptr.hpp> 

#include "../../global/global.h"

cCamera* cCamera::m_pCamera = NULL;

cCamera* cCamera::GetCamera(GLFWwindow* window)
{
    if (cCamera::m_pCamera == NULL)
    {
        cCamera::m_pCamera = new cCamera(window);
    }
    return cCamera::m_pCamera;
}

cCamera::cCamera(GLFWwindow* window)
{
    this->m_pWindow = window;

    this->m_eCameraMod = STATIC;

    this->bFirstMouse = true;

    this->m_view = glm::mat4(1.0f);
    this->m_projection = glm::mat4(1.0f);

    this->m_Position = glm::vec3(0.0f);
    this->m_Direction = glm::vec3(0.0f, 0.0f, -1.0f);

    this->m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
    this->m_Right = glm::vec3(1.0f, 0.0f, 0.0f);

    this->m_pPlayer = nullptr;
    this->m_playerPos = glm::vec3(0.0f);
    this->m_Velocity = glm::vec3(0.0f);

    this->m_MaxFoV = 60.0;
    this->m_MinFoV = 20.0;
    this->m_FoV = 60.0;

    this->m_Speed = 0.0f;
    this->m_MoveSpeed = 10.0f;
    this->m_SpeedScale = 2.0f;
     
    this->m_Offset.y = 2.0f;
    this->m_OldTheta = 0.0f;

    this->m_Distance = 2.0f;
    this->m_MaxDistance = 10.0f;
    this->m_MinDistance = 0.1f;

    this->m_Yaw = 0.0f;
    this->m_MaxYaw = 360.0f;
    this->m_MinYaw = -360.0f;
    this->m_YawSpeed = 0.1f;

    this->m_Pitch = 0.0f;
    this->m_MaxPitch = 30.0f;
    this->m_MinPitch = 0.0f;
    this->m_PitchSpeed = 0.1f;
}

cCamera::~cCamera() {}

void cCamera::m_UpdateBuffer()
{
    int width = 0, height = 0;

    glfwGetFramebufferSize(m_pWindow, &width, &height);

    glViewport(0, 0, width, height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glCullFace(GL_BACK);

    glDisable(GL_BLEND);

    //g_pCamera->SetCameraMod(3);

    this->SetBufferSize(width, height);

    return;
}

void cCamera::Update()
{
    this->m_UpdateBuffer();

    if (bFirstMouse)
    {
        m_MousePos = m_BufferCenter;
        bFirstMouse = false;
    }

    if (m_eCameraMod == STATIC)
    {
        m_Up = glm::vec3(0.0f, 1.0f, 0.0f);

        m_projection = glm::perspective(glm::radians(60.0),
                                         m_BufferSize.x / m_BufferSize.y,
                                         0.01, 500.0);

        m_view = glm::lookAt(m_Position, m_Position + m_Direction, m_Up);
    }

    if (m_eCameraMod == DYNAMIC)
    {
        glfwSetCursorPos(m_pWindow, m_BufferCenter.x, m_BufferCenter.y);

        double mouseX = m_BufferCenter.x - m_MousePos.x;
        double mouseY = m_BufferCenter.y - m_MousePos.y;
        m_MousePos = m_BufferCenter;

        m_Yaw += m_YawSpeed * static_cast<float>(mouseX);
        m_Pitch += m_PitchSpeed * static_cast<float>(mouseY);
        m_Pitch = glm::clamp(m_Pitch, -89.0f, 89.0f);

        m_Direction.x = cos(glm::radians(m_Pitch)) * sin(glm::radians(m_Yaw));
        m_Direction.y = sin(glm::radians(m_Pitch));
        m_Direction.z = cos(glm::radians(m_Pitch))* cos(glm::radians(m_Yaw));
        m_Direction = glm::normalize(m_Direction);

        m_Right.x = sin(glm::radians(m_Yaw) - 3.14 / 2.0);
        m_Right.y = 0;
        m_Right.z = cos(glm::radians(m_Yaw) - 3.14 / 2.0);
        m_Right = glm::normalize(m_Right);

        m_Up = glm::cross(m_Right, m_Direction);

        m_projection = glm::perspective(glm::radians(m_FoV),
                                         m_BufferSize.x / m_BufferSize.y,
                                         0.01, 500.0);

        m_view = glm::lookAt(m_Position, m_Position + m_Direction, m_Up);
    }

    if (m_eCameraMod == FIRST_PERSON) { }

    if (m_eCameraMod == THIRD_PERSON)
    {
        glfwSetCursorPos(m_pWindow, m_BufferCenter.x, m_BufferCenter.y);

        double mouseX = m_BufferCenter.x - m_MousePos.x;
        double mouseY = m_BufferCenter.y - m_MousePos.y;
        m_MousePos = m_BufferCenter;

        m_playerPos = m_pPlayer->GetPlayerPos();        
        glm::quat subjectOrient = m_pPlayer->GetPlayerObject()->GetQuatOrientation();

        m_Direction = subjectOrient * glm::vec3(0.0f, 0.0f, 1.0f);
        m_Right = subjectOrient * glm::vec3(1.0f, 0.0f, 0.0f);
        m_Up = glm::cross(m_Direction, m_Right);

        m_Yaw += m_YawSpeed * static_cast<float>(mouseX);
        if (m_Yaw > 360.0f)
        {
            m_Yaw -= 360.0f;
        }
        if (m_Yaw < -360.0f)
        {
            m_Yaw += 360.0f;
        }
        m_Yaw = glm::clamp(m_Yaw, m_MinYaw, m_MaxYaw);

        m_Pitch -= m_PitchSpeed * static_cast<float>(mouseY);
        m_Pitch = glm::clamp(m_Pitch, m_MinPitch, m_MaxPitch);

        float horizDis = m_Distance * cos(glm::radians(m_Pitch));
        float vertDist = m_Distance * sin(glm::radians(m_Pitch));

        float theta = subjectOrient.y + m_Yaw;
        float offsetX = horizDis * sin(glm::radians(theta));
        float offsetZ = horizDis * cos(glm::radians(theta));
        m_Position.x = m_playerPos.x - offsetX;
        m_Position.z = m_playerPos.z - offsetZ;
        m_Position.y = m_playerPos.y + vertDist + m_Offset.y;

        float deltatheta = m_OldTheta - theta;
        m_OldTheta = theta;
        subjectOrient = glm::rotate(subjectOrient, glm::radians(-deltatheta), m_Up);
        m_pPlayer->GetPlayerObject()->SetQuatOrientation(subjectOrient);

        m_projection = glm::perspective(glm::radians(m_FoV),
                                         m_BufferSize.x / m_BufferSize.y,
                                         0.01, 500.0); 

        m_view = glm::lookAt(m_Position, m_playerPos + m_Offset, m_Up);
    }

    g_pShaderManager->SetUniform(g_ShaderID, "eyeLocation", m_Position.x, m_Position.y, m_Position.z, 1.0f);
    g_pShaderManager->SetUniform(g_ShaderID, "matProjection", m_projection);
    g_pShaderManager->SetUniform(g_ShaderID, "matView", m_view);
}

bool cCamera::SetCameraMod(eCameraMods cameraMod)
{
    this->SetCameraMod(static_cast<int>(cameraMod));
    return true;
}
bool cCamera::SetCameraMod(int cameraMod)
{
    if (cameraMod == 0)
    {
        m_eCameraMod = STATIC;
        glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        return true;
    }

    if (cameraMod == 1)
    {
        m_eCameraMod = DYNAMIC;
        glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        return true;
    }

    if (cameraMod == 2)
    {
        m_eCameraMod = FIRST_PERSON;
        glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        return true;
    }

    if (cameraMod == 3)
    {
        m_eCameraMod = THIRD_PERSON;
        glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        return true;
    }

    return false;
}
eCameraMods cCamera::GetCameraMod(void) const
{
    return this->m_eCameraMod;
}

bool cCamera::SetPlayer(cPlayer* player)
{
    if (player != nullptr)
    {
        this->m_pPlayer = player;
        return true;
    }

    return false;
}
cPlayer* cCamera::GetPlayer(void)
{
    return this->m_pPlayer;
}

void cCamera::SetPosition(const glm::vec3& pos)
{
	this->m_Position = pos;
}
glm::vec3 cCamera::GetPosition(void) const
{
    if (m_eCameraMod == STATIC || m_eCameraMod == DYNAMIC)
    {
    	return this->m_Position;
    }
    if (m_eCameraMod == THIRD_PERSON || m_eCameraMod == FIRST_PERSON)
    {
        return this->m_pPlayer->GetPlayerPos();
    }
}

void cCamera::SetDirection(const glm::vec3& dir)
{
    this->m_Direction = glm::normalize(dir);
}
glm::vec3 cCamera::GetDirection(void) const
{
	return glm::normalize(this->m_Direction);
}

void cCamera::SetVelocity(const glm::vec3& velocity)
{
    this->m_Velocity = velocity;
}
glm::vec3 cCamera::GetVelocity(void) const
{
    return this->m_Velocity;
}

glm::vec3 cCamera::GetRightVec(void) const
{
    return glm::normalize(this->m_Right);
}
glm::vec3 cCamera::GetUpVec(void) const
{
    return glm::normalize(this->m_Up);
}

void cCamera::SetFOVMinMax(double FOV, double minFOV, double maxFOV)
{
    this->m_FoV = FOV;

    if (minFOV != -1)
    {
        this->m_MinFoV = minFOV;
    }
    if (maxFOV != -1)
    {
        this->m_MaxFoV = maxFOV;
    }

    return;
}
void cCamera::SetFOVMinMax(const glm::vec3& fovMinMax)
{
    m_FoV = static_cast<float>(fovMinMax.x);
    m_MinFoV = static_cast<float>(fovMinMax.y);
    m_MaxFoV = static_cast<float>(fovMinMax.z);

    return;
}
glm::vec3 cCamera::GetFOVMinMax(void) const
{
    glm::vec3 temp = glm::vec3(0.0);
    temp.x = static_cast<float>(m_FoV);
    temp.y = static_cast<float>(m_MinFoV);
    temp.z = static_cast<float>(m_MaxFoV);

    return temp;
}

void cCamera::SetPitchMinMax(float pitch, float minPitch, float maxPitch)
{
    m_Pitch = pitch;

    if (minPitch != -1)
    {
        this->m_MinPitch = minPitch;
    }
    if (maxPitch != -1)
    {
        this->m_MaxPitch = maxPitch;
    }

    return;
}
void cCamera::SetPitchMinMax(const glm::vec3& pitchMinMax)
{
    m_Pitch = pitchMinMax.x;
    m_MinPitch = pitchMinMax.y;
    m_MaxPitch = pitchMinMax.z;

    return;
}
glm::vec3 cCamera::GetPitchMinMax(void) const
{
    glm::vec3 temp = glm::vec3(0.0f);
    temp.x = m_Pitch;
    temp.y = m_MinPitch;
    temp.z = m_MaxPitch;

    return temp;
}

void cCamera::SetYawMinMax(float yaw, float minYaw, float maxYaw)
{
    m_Yaw = yaw;

    if (minYaw != -1)
    {
        this->m_MinYaw = minYaw;
    }
    if (maxYaw != -1)
    {
        this->m_MaxYaw = maxYaw;
    }

    return;
}
void cCamera::SetYawMinMax(const glm::vec3& yawMinMax)
{
    m_Yaw = yawMinMax.x;
    m_MinYaw = yawMinMax.y;
    m_MaxYaw = yawMinMax.z;

    return;
}
glm::vec3 cCamera::GetYawMinMax(void) const
{
    glm::vec3 temp = glm::vec3(0.0f);
    temp.x = m_Yaw;
    temp.y = m_MinYaw;
    temp.z = m_MaxYaw;

    return temp;
}

void cCamera::SetPitchYawSpeed(const glm::vec2& pitchYawSpeed)
{
    m_PitchSpeed = pitchYawSpeed.x;
    m_YawSpeed = pitchYawSpeed.y;

    return;
}
glm::vec2 cCamera::GetPitchYawSpeed(void) const
{
    glm::vec2 temp = glm::vec2(0.0f);
    temp.x = m_PitchSpeed;
    temp.y = m_YawSpeed;

    return temp;
}

void cCamera::SetDistMinMax(float distance, float minDist, float maxDist)
{
    m_Distance = distance;

    if (minDist != -1)
    {
        this->m_MinDistance = minDist;
    }
    if (maxDist != -1)
    {
        this->m_MaxDistance = maxDist;
    }

    return;
}
void cCamera::SetDistMinMax(const glm::vec3& distMinMax)
{
    m_Distance = distMinMax.x;
    m_MinDistance = distMinMax.y;
    m_MaxDistance = distMinMax.z;

    return;
}
glm::vec3 cCamera::GetDistMinMax(void) const
{
    glm::vec3 temp = glm::vec3(0.0f);
    temp.x = m_Distance;
    temp.y = m_MinDistance;
    temp.z = m_MaxDistance;

    return temp;
}

void cCamera::SetMoveSpeed(float speed)
{
    this->m_MoveSpeed = speed;
}
float cCamera::GetMoveSpeed(void) const
{
    return this->m_MoveSpeed;
}

void cCamera::SetSpeedScale(float scale)
{
    this->m_SpeedScale = scale;
}
float cCamera::GetSpeedScale(void) const
{
    return this->m_SpeedScale;
}

void cCamera::MoveCamera(eCameraEvents event, float eventValue)
{
    if (event == CAMERA_BOOST && eventValue == 1)
    {
        m_Speed = m_MoveSpeed * m_SpeedScale;
        return;
    }
    if (event == CAMERA_BOOST && eventValue == 0)
    {
        m_Speed = m_MoveSpeed;
        return;
    }

    if (event == CAMERA_LEFT)
    { 
        m_Velocity = -(m_Speed * m_Right * eventValue);
        m_Position += m_Velocity;
        return;
    }
    if (event == CAMERA_RIGHT)
    {
        m_Velocity = m_Speed * m_Right * eventValue;
        m_Position += m_Velocity;
        return;
    }

    if (event == CAMERA_FORWARD)
    {
        m_Velocity = m_Speed * m_Direction * eventValue;
        m_Position += m_Velocity;
        return;
    }
    if (event == CAMERA_BACKWARD)
    {
        m_Velocity = -(m_Speed * m_Direction * eventValue);
        m_Position += m_Velocity;
        return;
    }

    if (event == CAMERA_UP)
    {
        m_Velocity = m_Speed * m_Up * eventValue;
        m_Position += m_Velocity;
        return;
    }
    if (event == CAMERA_DOWN)
    {
        m_Velocity = -(m_Speed * m_Up * eventValue);
        m_Position += m_Velocity;
        return;
    }
}
void cCamera::ScrollAction(double yScrollPos)
{
    if (m_eCameraMod == DYNAMIC || m_eCameraMod == FIRST_PERSON)
    {
        m_FoV -= yScrollPos;
        m_FoV = glm::clamp(m_FoV, m_MinFoV, m_MaxFoV);
    }
    if (m_eCameraMod == THIRD_PERSON)
    {
        m_Distance -= static_cast<float>(yScrollPos);
        m_Distance = glm::clamp(m_Distance, m_MinDistance, m_MaxDistance);
    }
}

void cCamera::SetCursorPos(double xPos, double yPos)
{
    this->m_MousePos.x = xPos;
    this->m_MousePos.y = yPos;
    return;
}

void cCamera::SetBufferSize(double width, double height)
{
    this->m_BufferSize.x = width;
    this->m_BufferSize.y = height;

    this->m_BufferCenter.x = width / 2;
    this->m_BufferCenter.y = height / 2;

    return;
}
void cCamera::GetBufferSize(double& width, double& height)
{
    width = this->m_BufferSize.x;
    height = this->m_BufferSize.y;
    return;
}

void cCamera::SetWindowSize(int width, int height)
{
    glfwSetWindowSize(m_pWindow, width, height);
    return;
}
void cCamera::GetWindowSize(int& width, int& height)
{
    glfwGetWindowSize(m_pWindow, &width, &height);
    return;
}

glm::mat4 cCamera::GetMatView()
{
    return this->m_view;
}
glm::mat4 cCamera::GetMatProjection()
{
    return this->m_projection;
}
