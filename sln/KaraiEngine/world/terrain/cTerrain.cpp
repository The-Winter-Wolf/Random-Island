#include "cTerrain.h"

#include "../../objects/light/cLightManager.h"
extern cLightManager* g_pLightManager;

#include "../../system/window/cConsoleLogs.h"
extern cConsoleLogs* g_pConsole;

#include "../../global/global.h"

#include "../../graphics/shaders/cShaderManager.h"
extern cShaderManager* g_pShaderManager;

#include "../../objects/camera/cCamera.h"
extern cCamera* g_pCamera;

#include "../../utils/cMaths.h"
extern cMaths* g_pMaths;

#include <glm/gtc/type_ptr.hpp> 
#include <other/stb_image.h>
#include <iostream>

cTerrain::cTerrain()
{
    this->m_height = 0;
    this->m_width = 0;

    this->m_heightScale = 0.0f;
    this->m_heightShift = 0.0f;

    this->m_heightID = 0;
    this->m_diffuseID = 0;

    this->terrainVAO = 0;
    this->terrainVBO = 0;
    this->terrainIBO = 0;

    this->m_invMass = 0.0f;
    this->m_restitution = 0.0f;
}

cTerrain::~cTerrain()
{

}

void cTerrain::SetBasePath(std::string directory)
{
    m_directory = directory;
}

void cTerrain::SetTerrainSettings(float heightScale, float heightShift)
{
    this->m_heightScale = heightScale;
    this->m_heightShift = heightShift;
}

void cTerrain::LoadTerrainTextures(const std::string& diffuseName, const std::string& heightName)
{
    this->m_LoadTexture(diffuseName, "diffuse", false);
    this->m_LoadTexture(heightName, "height", false);
}

void cTerrain::RenderTerrain()
{
    g_pShaderManager->UseShader("TerrainShader");

    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    g_pShaderManager->SetUniform(g_ShaderID, "matView", g_pCamera->GetMatView());
    g_pShaderManager->SetUniform(g_ShaderID, "matProjection", g_pCamera->GetMatProjection());
    g_pShaderManager->SetUniform(g_ShaderID, "matModel", glm::mat4(1.0f));
    g_pShaderManager->SetUniform(g_ShaderID, "eyeLocation", glm::vec4(g_pCamera->GetPosition(), 1.0f));

    glm::mat4 matModel_IT = glm::inverse(glm::transpose(glm::mat4(1.0f)));
    g_pShaderManager->SetUniform(g_ShaderID, "matModel_IT", matModel_IT);

    g_pShaderManager->SetUniform(g_ShaderID, "bHasLight", GL_TRUE);

    if (m_diffuseID == 0)
    {
        g_pShaderManager->SetUniform(g_ShaderID, "bHasTexture", GL_FALSE);
    }
    else
    {
        g_pShaderManager->SetUniform(g_ShaderID, "bHasTexture", GL_TRUE);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_diffuseID);

        std::string name = "texture_diffuse" + std::to_string(1);
        glUniform1i(glGetUniformLocation(g_ShaderID, (name).c_str()), 0);
    }
     
    glBindVertexArray(terrainVAO);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    g_pShaderManager->UseShader("MainShader");
}

float cTerrain::GetTerrainHight(float x, float z, glm::vec3& normal) const
{


    int bottomLeftX = static_cast<int>(floor(x));
    int bottomLeftZ = static_cast<int>(floor(z));

    if (bottomLeftX < 0 || bottomLeftZ < 0 || bottomLeftX >= m_width - 1 || bottomLeftZ >= m_height - 1)
    {
        return 0.0f;
    }

    glm::vec3 bottomLeft = vertices[bottomLeftX + bottomLeftZ * m_width].position;
    glm::vec3 topLeft = vertices[bottomLeftX + (bottomLeftZ + 1) * m_width].position;
    glm::vec3 bottomRight = vertices[(bottomLeftX + 1) + bottomLeftZ * m_width].position;
    glm::vec3 topRight = vertices[(bottomLeftX + 1) + (bottomLeftZ + 1) * m_width].position;

    float height = 0;

    if (x <= (1 - z))
    {
        height = g_pMaths->Barrycentric(bottomLeft, bottomRight, topLeft, glm::vec2(x, z));
        normal = glm::normalize(glm::cross(bottomRight - bottomLeft, topLeft - bottomLeft));
    }
    if (x >= (1 - z))
    {
        height = g_pMaths->Barrycentric(bottomRight, topRight, topLeft, glm::vec2(x, z));
        normal = glm::normalize(glm::cross(topRight - bottomRight, topLeft - bottomRight));
    }

    return height;
}
float cTerrain::GetTerrainHight(float x, float z) const
{
    if (x < 0 || z < 0 || x >= m_width - 1 || z >= m_height - 1)
    {
        return 0.0f;
    }

    int bottomLeftX = static_cast<int>(floor(x));
    int bottomLeftZ = static_cast<int>(floor(z));

    glm::vec3 bottomLeft = vertices[bottomLeftX + bottomLeftZ * m_width].position;
    glm::vec3 topLeft = vertices[bottomLeftX + (bottomLeftZ + 1) * m_width].position;
    glm::vec3 bottomRight = vertices[(bottomLeftX + 1) + bottomLeftZ * m_width].position;
    glm::vec3 topRight = vertices[(bottomLeftX + 1) + (bottomLeftZ + 1) * m_width].position;

    float height = 0;

    if (x <= (1 - z))
    {
        height = g_pMaths->Barrycentric(bottomLeft, bottomRight, topLeft, glm::vec2(x, z));
    }
    if (x >= (1 - z))
    {
        height = g_pMaths->Barrycentric(bottomRight, topRight, topLeft, glm::vec2(x, z));
    }

    return height;
}

void cTerrain::SetPhysicsTerrainData(float& invMass, float& restitution)
{
    this->m_invMass = invMass;
    this->m_restitution = restitution;
    return;
}
void cTerrain::GetPhysicsTerrainData(float& invMass, float& rest) const
{
    invMass = this->m_invMass;
    rest = this->m_restitution;
    return;
}

bool cTerrain::m_LoadTexture(const std::string& textureName, std::string modelType, bool flippedUV)
{
    if (flippedUV)
    {
        stbi_set_flip_vertically_on_load(true);
    }
    else
    {
        stbi_set_flip_vertically_on_load(false);
    }

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nChannels;
    std::string fullPath = m_directory + "/" + textureName;

    unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &nChannels, 0);
    if (!data)
    {
        g_pConsole->cError("Texture failed to load at path: " + textureName);
        stbi_image_free(data);

        return false;
    }
    if (modelType == "diffuse")
    {
        GLenum format = 0;
        if (nChannels == 1)
            format = GL_RED;
        else if (nChannels == 3)
            format = GL_RGB;
        else if (nChannels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        m_diffuseID = textureID;
    }
    if (modelType == "height")
    {  
        this->m_CreateTerrain(width, height, nChannels, data);
        m_heightID = textureID;
    }

    g_pConsole->cSuccess("Texture " + textureName + " is loaded");

    stbi_image_free(data);
    return true;
}

void cTerrain::m_CreateTerrain(int width, int height, int nChannels, unsigned char* data)
{
    m_width = width;
    m_height = height;

    int minValue = static_cast<float>(glm::min(width, height));

    float division = 0.0f;
    if (m_heightScale == 0)
    {
        division = 0.0f;
    }
    else
    {
        division = minValue / m_heightScale;
    }
    
    float yScale = division / 255.0f;
    float yShift = -division / 2.0f + m_heightShift;

    unsigned int bytePerPixel = nChannels;

    // Vertex      
    int index = 0;
    vertices.resize(width * height);

    float min = FLT_MAX;
    float max = FLT_MIN;

    for (int z = 0; z < height; z++)
    {
        for (int x = 0; x < width; x++)
        {
            vertices[index].position.x = static_cast<float>(x);
            vertices[index].position.z = static_cast<float>(z);
            vertices[index].position.w = 1.0f;

            unsigned char* pixelOffset = data + (z * width + x) * bytePerPixel;
            unsigned char y = pixelOffset[0];

            vertices[index].position.y = static_cast<float>(y) * yScale + yShift;
            
            vertices[index].texCoords.s = static_cast<float>(x) / static_cast<float>(width);
            vertices[index].texCoords.t = static_cast<float>(z) / static_cast<float>(height);

            vertices[index].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

            if (vertices[index].position.y < min)
            {
                min = vertices[index].position.y;
            }
            if (vertices[index].position.y > max)
            {
                max = vertices[index].position.y;
            }

            index++;
        }
    }

    // Normals
    float heightL = 0.0f;
    float heightR = 0.0f;
    float heightD = 0.0f;
    float heightU = 0.0f;
    
    glm::vec3 normal = glm::vec3(0.0f);

    for (int i = 0; i < vertices.size(); i++)
    {
        heightL = GetTerrainHight(vertices[i].position.x-1, vertices[i].position.z);
        heightR = GetTerrainHight(vertices[i].position.x+1, vertices[i].position.z);
        heightD = GetTerrainHight(vertices[i].position.x, vertices[i].position.z-1);
        heightU = GetTerrainHight(vertices[i].position.x, vertices[i].position.z+1);

        normal = glm::vec3(heightL - heightR, 2.0f, heightD - heightU);
        normal = glm::normalize(normal);

        vertices[i].normal = glm::vec4(normal,1.0f);
    }

    // Indices 
    index = 0;
    indices.resize( (width - 1) * (height - 1) * 6 );

    for (unsigned int z = 0; z < height - 1; z++)
    {
        for (unsigned int x = 0; x < width - 1; x++)
        {
            unsigned int indexBottomLeft = z * width + x;
            unsigned int indexBottomRight = z * width + (x + 1);
            unsigned int indexTopLeft = (z + 1) * width + x;
            unsigned int indexTopRight = (z + 1) * width + (x + 1);

            // Add bottom left triangle
            indices[index++] = indexBottomLeft;
            indices[index++] = indexTopLeft;
            indices[index++] = indexBottomRight;

            // Add top right triangle
            indices[index++] = indexTopLeft;
            indices[index++] = indexTopRight;
            indices[index++] = indexBottomRight;
        }
    }

    g_pConsole->cMessage("Loaded " + std::to_string(vertices.size()) + " vertices and " 
                                  + std::to_string(indices.size()) + " indices");

    glGenVertexArrays(1, &terrainVAO);
    glBindVertexArray(terrainVAO);

    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(sTerVertex), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &terrainIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


    // vertex position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(sTerVertex), (GLvoid*)(0));

    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(sTerVertex), (GLvoid*)offsetof(sTerVertex, normal));

    // vertex texture coords 
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(sTerVertex), (GLvoid*)offsetof(sTerVertex, texCoords));

    // vertex color
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(sTerVertex), (GLvoid*)offsetof(sTerVertex, color));

    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);

    return;
}

