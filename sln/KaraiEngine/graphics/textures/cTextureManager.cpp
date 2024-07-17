#include "cTextureManager.h"

#include "../../global/global.h"

#include <iostream>

cTextureManager* cTextureManager::m_pTextureManager = NULL;

cTextureManager* cTextureManager::GetTextureManager()
{
    if (cTextureManager::m_pTextureManager == NULL)
    {
        cTextureManager::m_pTextureManager = new cTextureManager();
    }
    return cTextureManager::m_pTextureManager;
}

cTextureManager::cTextureManager()
{
    this->m_directory = "../../extern/assets/textures";
}

cTextureManager::~cTextureManager(){}

sTexture* cTextureManager::LoadTextureFromFile(const eTextureType& textureType, const std::string& fileName,
                                          const std::string& friendName, bool flippedUVs)
{
    sTexture* texture = new sTexture();

    texture->directory = m_directory;
    texture->textureFile = fileName;
    texture->textureType = textureType;
    texture->friendlyName = friendName;
    texture->bIsFlippedUVs = flippedUVs;
    
    sTexture* tempTexture = this->GetTextureByFileName(fileName);
    if (tempTexture)
    {
        return tempTexture;
    }

    if (flippedUVs)
    {
        stbi_set_flip_vertically_on_load(true);
    }
    else
    {
        stbi_set_flip_vertically_on_load(false);
    }

    unsigned int textureID;
    int width, height, nrComponents;
    std::string fullName = m_directory + '/' + fileName;

    unsigned char* data = stbi_load(fullName.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        g_pConsole->cError("Texture failed to load at path: " + fullName);
        stbi_image_free(data);
        return nullptr;
    }

    texture->textureID = textureID;
    m_mapTextures[fileName] = texture;

    g_pConsole->cSuccess("Texture " + fileName + " is loaded as " + GetStringFromType(textureType));
    return texture;
}

sTexture* cTextureManager::GetTextureByFriendlyName(const std::string& friendlyTextureName)
{
    mapIterator = m_mapTextures.begin();
    for (mapIterator; mapIterator != m_mapTextures.end(); mapIterator++)
    {
        if (mapIterator->second->friendlyName == friendlyTextureName)
        {
            return mapIterator->second;
        }
    }

    return nullptr;
}

sTexture* cTextureManager::GetTextureByFileName(const std::string& textureFileName)
{
    mapIterator = m_mapTextures.find(textureFileName);
    if (mapIterator == m_mapTextures.end())
    {
        return nullptr;
    }
    else
    {
        return mapIterator->second;
    }
}

void cTextureManager::SetBasePath(const std::string& basePath)
{
    this->m_directory = basePath;
    return;
}

std::string cTextureManager::GetBasePath(void)
{
    return this->m_directory;
}

std::string cTextureManager::GetStringFromType(eTextureType type)
{
    switch (type)
    {
    case 0:
        return "texture_diffuse";
    case 1:
        return "texture_specular";     
    case 2:
        return "texture_normal";
    case 3:
        return "texture_height";
    case 4:
        return "texture_opacity_black";
    case 5:
        return "texture_opacity_white";
    case 6:
        return "texture_emission";
    default:
        return "unknown";
    }
}

int cTextureManager::LoadTextureCubemap(std::vector<std::string> faces, bool flippedUV)
{
    int size = faces.size();

    if (size < 6)
    {
        g_pConsole->cError("Skybox contains " + std::to_string(size) + " textures out of 6 textures");
        return -1;
    }

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
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nChannels;

    for (unsigned int i = 0; i < size; i++)
    {
        std::string fullPath = m_directory + "/" + faces[i];
        unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &nChannels, 0);
        if (!data)
        {
            g_pConsole->cError("Texture failed to load at path: " + fullPath);
            stbi_image_free(data);

            return -1;
        }
        else
        {
            GLenum format = 0;
            if (nChannels == 1)
                format = GL_RED;
            else if (nChannels == 3)
                format = GL_RGB;
            else if (nChannels == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
