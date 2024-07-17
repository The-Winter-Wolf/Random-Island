#include "cSkybox.h"

#include "../../global/global.h"

#include "../../graphics/textures/cTextureManager.h"
extern cTextureManager* g_pTextureManager;

#include "../../graphics/shaders/cShaderManager.h"
extern cShaderManager* g_pShaderManager;

#include "../../objects/camera/cCamera.h"
extern cCamera* g_pCamera;

#include "../../system/time/cTime.h"
extern cTime* g_pTime;

cSkybox::cSkybox()
{
    this->m_CreateSkybox();

    this->m_time = 0.0f;
    this->m_multiplier = 1.0f;
    this->m_ratioIndex = 0.0f;
    this->m_cloudSpeed = 0.0f;
    this->m_planetSpeed = 0.0f;

    this->skyboxIndex01 = -1;
    this->skyboxIndex02 = -1;
    this->skyboxIndex03 = -1;
    this->skyboxIndex04 = -1;

    this->bHasTexture01 = false;
    this->bHasTexture02 = false;
    this->bHasTexture03 = false;
    this->bHasTexture04 = false;

    this->skyboxColor01 = glm::vec4(81.0f, 143.0f, 210.0f, 255.0f) / 255.0f;// light blue
    this->skyboxColor02 = glm::vec4(0.0f, 0.0f, 0.0f, 255.0f) / 255.0f;     // black
    this->skyboxColor03 = glm::vec4(253.0f, 112.0f, 1.0f, 255.0f) / 255.0f; // light purple
    this->skyboxColor04 = glm::vec4(8.0f, 25.0f, 35.0f, 255.0f) / 255.0f;   // dark gray
}

cSkybox::~cSkybox()
{
}

void cSkybox::SetBasePath(std::string directory)
{
    this->m_directory = directory;
}

void cSkybox::LoadSkyboxTextures(const std::string& friendlySkybox, bool flippedUV,
								const std::vector<std::string>& skyTextures)
{
	this->m_vecSkyboxTextures.clear();
	m_vecSkyboxTextures = skyTextures;

    g_pTextureManager->SetBasePath(m_directory);
	int cubeIndex = g_pTextureManager->LoadTextureCubemap(m_vecSkyboxTextures, flippedUV);

	if (cubeIndex == -1)
	{
		return;
	}
	else
	{
		m_mapSkyboxes[friendlySkybox] = cubeIndex;
		return;
	}
}

void cSkybox::LoadSkyboxTextures(const std::string& friendlySkybox, bool flippedUV,
								const std::string& rightTexture, const std::string& leftTexture,
								const std::string& topTexture, const std::string& bottomTexture,
								const std::string& frontTexture, const std::string& backTexture)
{
	this->m_vecSkyboxTextures.clear();
	this->m_vecSkyboxTextures.push_back(rightTexture);
	this->m_vecSkyboxTextures.push_back(leftTexture);
	this->m_vecSkyboxTextures.push_back(topTexture);
	this->m_vecSkyboxTextures.push_back(bottomTexture);
	this->m_vecSkyboxTextures.push_back(frontTexture);
	this->m_vecSkyboxTextures.push_back(backTexture);

    g_pTextureManager->SetBasePath(m_directory);
	int cubeIndex = g_pTextureManager->LoadTextureCubemap(m_vecSkyboxTextures, flippedUV);

	if (cubeIndex == -1)
	{
		return;
	}
	else
	{
		m_mapSkyboxes[friendlySkybox] = cubeIndex;
		return;
	}
}

void cSkybox::SetSkyboxUpdate(float dayLength, float currentTime, float cloudAngleSpeed, float planetAngleSpeed)
{
    g_pShaderManager->UseShader("SkyboxShader");

    this->skyboxIndex01 = m_GetTextureIndex("MiddaySkybox");
    if (skyboxIndex01 != -1)
    {
        bHasTexture01 = true;
        g_pShaderManager->SetUniform(g_ShaderID, "bHasTexture01", bHasTexture01);
        glUniform1i(glGetUniformLocation(g_ShaderID, "skybox01"), 0);
    }
    else
    {
        g_pShaderManager->SetUniform(g_ShaderID, "skyboxColor01", skyboxColor01);
    }

    this->skyboxIndex02 = m_GetTextureIndex("MidnightSkybox");
    if (skyboxIndex02 != -1)
    {
        bHasTexture02 = true;
        g_pShaderManager->SetUniform(g_ShaderID, "bHasTexture02", bHasTexture02);
        glUniform1i(glGetUniformLocation(g_ShaderID, "skybox02"), 1);
    }
    else
    {
        g_pShaderManager->SetUniform(g_ShaderID, "skyboxColor02", skyboxColor02);
    }

    this->skyboxIndex03 = m_GetTextureIndex("SunriseSkybox");
    if (skyboxIndex03 != -1)
    {
        bHasTexture03 = true;
        g_pShaderManager->SetUniform(g_ShaderID, "bHasTexture03", bHasTexture03);
        glUniform1i(glGetUniformLocation(g_ShaderID, "skybox03"), 1);
    }
    else
    {
        g_pShaderManager->SetUniform(g_ShaderID, "skyboxColor03", skyboxColor03);
    }

    this->skyboxIndex04 = m_GetTextureIndex("SunsetSkybox");
    if (skyboxIndex04 != -1)
    {
        bHasTexture04 = true;
        g_pShaderManager->SetUniform(g_ShaderID, "bHasTexture04", bHasTexture04);
        glUniform1i(glGetUniformLocation(g_ShaderID, "skybox04"), 1);
    }
    else
    {
        g_pShaderManager->SetUniform(g_ShaderID, "skyboxColor04", skyboxColor04);
    }

    g_pShaderManager->UseShader("MainShader");

    this->m_ratioIndex = 16.0f / (dayLength * 60.0f);
    this->m_time = m_ratioIndex * currentTime;
    this->m_cloudSpeed = cloudAngleSpeed;
    this->m_planetSpeed = planetAngleSpeed;
}

void cSkybox::RenderSkybox(float deltaTime)
{
    g_pShaderManager->UseShader("SkyboxShader");

    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    this->m_UpdateSkybox(deltaTime);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxIndex01);

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxIndex02);

    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxIndex03);

    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxIndex04);

    glBindVertexArray(m_skyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
    glDepthFunc(GL_LESS);

    g_pShaderManager->UseShader("MainShader");
}

void cSkybox::m_UpdateSkybox(float deltaTime)
{
    if (m_time >= 0.0f && m_time <= 4.0f)
    {
        m_time += deltaTime * m_ratioIndex * m_multiplier;
    }
    else if (m_time < 0.0f)
    {
        m_multiplier = 1.0f;
        m_time = 0.0f;
    }
    else if (m_time > 4.0f)
    {
        m_multiplier = -1.0f;
        m_time = 4.0f;
    }

    g_pShaderManager->SetUniform(g_ShaderID, "time", m_time);

    float cloudTime = g_pTime->GetGLFWTime() * m_cloudSpeed / 360.0f;
    float planetTime = g_pTime->GetGLFWTime() * m_planetSpeed / 360.0f;

    glm::mat4 cloudMatModel = glm::mat4(1.0f);
    glm::mat4 planetMatModel = glm::mat4(1.0f);

    cloudMatModel = glm::rotate(cloudMatModel, cloudTime, glm::vec3(0.0f, 1.0f, 0.0f));
    g_pShaderManager->SetUniform(g_ShaderID, "cloudMatModel", cloudMatModel);

    planetMatModel = glm::rotate(planetMatModel, planetTime, glm::vec3(0.0f, 1.0f, 0.0f));
    g_pShaderManager->SetUniform(g_ShaderID, "planetMatModel", planetMatModel);

    glm::mat4 matView = glm::mat4(glm::mat3(g_pCamera->GetMatView()));
    g_pShaderManager->SetUniform(g_ShaderID, "matView", matView);

    g_pShaderManager->SetUniform(g_ShaderID, "matProjection", g_pCamera->GetMatProjection());
}

void cSkybox::m_CreateSkybox()
{
    float skyboxVertices[] = {
        // Right
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        // Left
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        // Top
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        // Bottom
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        // Back
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        // Front
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    unsigned int skyboxVBO;
    glGenVertexArrays(1, &m_skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(m_skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

int cSkybox::m_GetTextureIndex(std::string friendlySkyboxName)
{
    std::map<std::string, unsigned int>::iterator mapIterator = m_mapSkyboxes.find(friendlySkyboxName);
    if (mapIterator == m_mapSkyboxes.end())
    {
        return -1;
    }
    else
    {
        return mapIterator->second;
    }
}
