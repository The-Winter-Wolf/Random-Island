#include "cModelManager.h"

#include "../../global/common.h"
#include "../../global/global.h"

#include <iostream>
#include <fstream>

extern cTextureManager* g_pTextureManager;

cModelManager* cModelManager::m_pModelManager = NULL;

cModelManager* cModelManager::GetModelManager()
{
    if (cModelManager::m_pModelManager == NULL)
    {
        cModelManager::m_pModelManager = new cModelManager();
    }
    return cModelManager::m_pModelManager;
}

cModelManager::cModelManager()
{
    this->directory = "../../extern/assets/models"; // default
    m_pTempModel = nullptr;
}

cModelManager::~cModelManager(){}

void cModelManager::SetBasePath(const std::string& basePathWithoutSlash)
{
    this->directory = basePathWithoutSlash;
    return;
}

std::string cModelManager::GetBasePath(void)
{
    return this->directory;
}

cModel* cModelManager::LoadModelIntoVAO(const std::string& fileName,
	                                 unsigned int shaderProgramID,
                                     bool flippedUVs /*false*/,
                                     bool bIsDynamicBuffer /*false*/)
{
    m_pTempModel = FindDrawInfoByModelName(fileName);
    if (m_pTempModel != nullptr)
    {
        return m_pTempModel;
    }

    m_pTempModel = new cModel();
    m_pTempModel->modelPath = directory;
    m_pTempModel->modelName = fileName;

    shaderProgID = shaderProgramID;
    bIsDynamicBuff = bIsDynamicBuffer;
    bIsFlippedUVs = flippedUVs;

    std::string fileAndPath = this->directory + "/" + fileName;

    if ( ! this->m_LoadModelAssimp(fileAndPath) )
    {
        g_pConsole->cError(fileName + " loading failed");
        return nullptr;
    }
    else
    {
        int numberOfModelVertices = 0;
        for (unsigned int i = 0; i != m_pTempModel->vpMeshesOfModel.size(); i++)
        {
            numberOfModelVertices += m_pTempModel->vpMeshesOfModel[i]->numberOfVertices;
        }        
              
        g_pConsole->cSuccess("Loaded model: " + fileName + 
            " (Meshes: " + std::to_string(m_pTempModel->vpMeshesOfModel.size()) + 
            ", Vertices: " + std::to_string(numberOfModelVertices) + ")");
        
        //std::cout << "BoneInfo: " << m_pTempModel->GetBoneInfoMap().size() << std::endl;
        //std::cout << "Count: " << m_pTempModel->GetBoneCount() << std::endl;

        this->m_map_pModelsByName[m_pTempModel->modelName] = m_pTempModel;

        return m_pTempModel;
    }   
}

bool cModelManager::LoadModelIntoVAO(cModel* modelInfo)
{
    if (modelInfo == nullptr)
    {
        g_pConsole->cError("Loading model from model info failed");
        return false;
    }

    m_pTempModel = modelInfo;

    for (cMeshDrawInfo* meshInfo : m_pTempModel->vpMeshesOfModel)
    {
       m_SetupMesh(meshInfo);
    }

    int numberOfModelVertices = 0;
    for (unsigned int i = 0; i != m_pTempModel->vpMeshesOfModel.size(); i++)
    {
        numberOfModelVertices += m_pTempModel->vpMeshesOfModel[i]->numberOfVertices;
    }

    g_pConsole->cSuccess("Loaded model: " + m_pTempModel->modelName +
        " (Meshes: " + std::to_string(m_pTempModel->vpMeshesOfModel.size()) +
        ", Vertices: " + std::to_string(numberOfModelVertices) + ")");

    this->m_map_pModelsByName[m_pTempModel->modelName] = m_pTempModel;

    m_pTempModel = nullptr;

    return true;
}

cModel* cModelManager::FindDrawInfoByModelName(const std::string& filename)
{
	std::map< std::string /*model name*/, cModel* /* info needed to draw*/ >
       ::iterator itModelDrawInfo = this->m_map_pModelsByName.find( filename );

	if (itModelDrawInfo == this->m_map_pModelsByName.end() )
	{
		return nullptr;
	}
 
	return itModelDrawInfo->second;
}

bool cModelManager::CheckNameInMap(const std::string& filename)
{
    if (m_map_pModelsByName.find(filename) == m_map_pModelsByName.end())
    {
        return false;
    }
    return true;
}

bool cModelManager::UpdateVAOBuffers(const cModel* updatedModelDrawInfo)
{
    m_pTempModel = FindDrawInfoByModelName(updatedModelDrawInfo->modelName);

    if (m_pTempModel == nullptr) { return false; }

    for (unsigned int i = 0; i != m_pTempModel->vpMeshesOfModel.size(); i++)
    {
         glBindBuffer(GL_ARRAY_BUFFER, m_pTempModel->vpMeshesOfModel[i]->VertexBufferID);

         glBufferSubData( GL_ARRAY_BUFFER, 0,
                          sizeof(sVertex) * m_pTempModel->vpMeshesOfModel[i]->numberOfVertices,
                          static_cast<GLvoid*> (& m_pTempModel->vpMeshesOfModel[i]->vertices[0]) );

         glBindBuffer(GL_ARRAY_BUFFER, 0);

         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pTempModel->vpMeshesOfModel[i]->IndexBufferID);

         glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, 0,
                          sizeof(unsigned int) * m_pTempModel->vpMeshesOfModel[i]->numberOfIndices,
                          static_cast<GLvoid*> (& m_pTempModel->vpMeshesOfModel[i]->indices) );

         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
 
    return true;
}

bool cModelManager::UpdateVertexBuffers(const cModel* updatedDrawInfo)
{
    m_pTempModel = FindDrawInfoByModelName(updatedDrawInfo->modelName);

    if (m_pTempModel == nullptr) { return false; }

    for (unsigned int i = 0; i != m_pTempModel->vpMeshesOfModel.size(); i++)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_pTempModel->vpMeshesOfModel[i]->VertexBufferID);

        glBufferSubData(GL_ARRAY_BUFFER, 0,
            sizeof(sVertex) * updatedDrawInfo->vpMeshesOfModel[i]->numberOfVertices,
            static_cast<GLvoid*> (&updatedDrawInfo->vpMeshesOfModel[i]->vertices[0]));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    return true;
}

bool cModelManager::UpdateMatrixBuffers(const cModel* updatedDrawInfo)
{
    m_pTempModel = FindDrawInfoByModelName(updatedDrawInfo->modelName);

    if (m_pTempModel == nullptr) { return false; }

    int sizeOfMatrices = m_pTempModel->matrices.size();

    for (unsigned int i = 0; i != m_pTempModel->vpMeshesOfModel.size(); i++)
    {
        int numOfMatrices = updatedDrawInfo->vpMeshesOfModel[i]->numberOfMatrices;

        glBindBuffer(GL_ARRAY_BUFFER, m_pTempModel->vpMeshesOfModel[i]->MatModelBufferID);

        if (numOfMatrices == sizeOfMatrices)
        {
            glBufferSubData(GL_ARRAY_BUFFER, 0,
                            sizeof(glm::mat4) * numOfMatrices,
                            static_cast<GLvoid*> (&m_pTempModel->matrices[0]));
        }
        else
        {
            glBufferData(GL_ARRAY_BUFFER,
                         sizeof(glm::mat4) * sizeOfMatrices,
                         static_cast<GLvoid*> (&m_pTempModel->matrices[0]), GL_DYNAMIC_DRAW);

            m_pTempModel->vpMeshesOfModel[i]->numberOfMatrices = sizeOfMatrices;
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void cModelManager::SetTextureToModel(const std::string& textureName, const std::string& modelName, int meshIndex)
{
    sTexture* texture;
    
    texture = g_pTextureManager->GetTextureByFileName(textureName);
    if (texture == nullptr) { return; }

    m_pTempModel = FindDrawInfoByModelName(modelName);
    if (m_pTempModel == nullptr) { return; }

    if (meshIndex != -1)
    {
        bool addTexture = true;

        for (sTexture* t : m_pTempModel->vpMeshesOfModel[meshIndex]->textures)
        {
            if (t->textureFile == textureName)
            {
                addTexture = false;
                break;
            }
        }
        if (addTexture)
        {
            m_pTempModel->vpMeshesOfModel[meshIndex]->textures.push_back(texture);
        }

        return;
    }

    for (int index = 0; index < m_pTempModel->vpMeshesOfModel.size(); index++)
    {
        bool addTexture = true;

        for (sTexture* t : m_pTempModel->vpMeshesOfModel[index]->textures)
        {
            if (t->textureFile == textureName)
            {
                addTexture = false;
                break;
            }
        }
        if (addTexture)
        {
            m_pTempModel->vpMeshesOfModel[index]->textures.push_back(texture);
        }
    }
}

void cModelManager::SetTextureUVOffset(const std::string& textureName, const std::string& modelName, const glm::vec2& UVOffset)
{
    m_pTempModel = FindDrawInfoByModelName(modelName);
    if (m_pTempModel == nullptr) { return; }

    for (cMeshDrawInfo* mesh : m_pTempModel->vpMeshesOfModel)
    {
        for (unsigned int i = 0; i != mesh->textures.size(); i++)
        {
            if (mesh->textures[i]->textureFile == textureName)
            {
                mesh->textures[i]->UVOffset = UVOffset;
                break;
            }
        }
    }
}
glm::vec2 cModelManager::GetTextureUVOffset(const std::string& textureName, const std::string& modelName)
{
    m_pTempModel = FindDrawInfoByModelName(modelName);
    if (m_pTempModel == nullptr) { return glm::vec2(0.0f, 0.0f); }

    for (cMeshDrawInfo* mesh : m_pTempModel->vpMeshesOfModel)
    {
        for (sTexture* t : mesh->textures)
        {
            if (t->textureFile == textureName)
            {
                return t->UVOffset;
            }
        }
    }

    return glm::vec2(0.0f, 0.0f);
}

void cModelManager::SetTextureScale(const std::string& textureName, const std::string& modelName, float texScale)
{
    m_pTempModel = FindDrawInfoByModelName(modelName);
    if (m_pTempModel == nullptr) { return; }

    for (cMeshDrawInfo* mesh : m_pTempModel->vpMeshesOfModel)
    {
        for (unsigned int i = 0; i != mesh->textures.size(); i++)
        {
            if (mesh->textures[i]->textureFile == textureName)
            {
                mesh->textures[i]->texScale = texScale;
                break;
            }
        }
    }
}
float cModelManager::GetTextureScale(const std::string& textureName, const std::string& modelName)
{
    m_pTempModel = FindDrawInfoByModelName(modelName);
    if (m_pTempModel == nullptr) { return 1.0f; }

    for (cMeshDrawInfo* mesh : m_pTempModel->vpMeshesOfModel)
    {
        for (sTexture* t : mesh->textures)
        {
            if (t->textureFile == textureName)
            {
                return t->texScale;
            }
        }
    }

    return 1.0f;
}

void cModelManager::SetHeightMapScale(const std::string& textureName, const std::string& modelName, const float& heightScale)
{
    m_pTempModel = FindDrawInfoByModelName(modelName);
    if (m_pTempModel == nullptr) { return; }

    for (cMeshDrawInfo* mesh : m_pTempModel->vpMeshesOfModel)
    {
        for (unsigned int i = 0; i != mesh->textures.size(); i++)
        {
            if (mesh->textures[i]->textureFile == textureName && mesh->textures[i]->textureType == texture_height)
            {
                mesh->textures[i]->heightMapScale = heightScale;
                break;
            }
        }
    }
}
float cModelManager::GetHeightMapScale(const std::string& textureName, const std::string& modelName)
{
    m_pTempModel = FindDrawInfoByModelName(modelName);
    if (m_pTempModel == nullptr) { return 0.0f; }

    for (cMeshDrawInfo* mesh : m_pTempModel->vpMeshesOfModel)
    {
        for (sTexture* t : mesh->textures)
        {
            if (t->textureFile == textureName && t->textureType == texture_height)
            {
                return t->heightMapScale;
            }
        }
    }

    return 0.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void cModelManager::m_SetupMesh(cMeshDrawInfo* meshDrawInfo)
{
    glGenVertexArrays( 1, &(meshDrawInfo->VAO_ID) );
	glBindVertexArray(meshDrawInfo->VAO_ID);

    // vertices
    size_t vertexSize = sizeof(sVertex);

	glGenBuffers( 1, &(meshDrawInfo->VertexBufferID) );
	glBindBuffer(GL_ARRAY_BUFFER, meshDrawInfo->VertexBufferID);

	glBufferData( GL_ARRAY_BUFFER, 
                  vertexSize * meshDrawInfo->numberOfVertices,
                  static_cast<GLvoid*> (& meshDrawInfo->vertices[0]),
				  (bIsDynamicBuff ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW) );

    glEnableVertexAttribArray(0);
    glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, vertexSize, (GLvoid*)(0) );

    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, vertexSize, (GLvoid*)offsetof(sVertex, normal) );

    // vertex texture coords 
    glEnableVertexAttribArray(2);
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, vertexSize, (GLvoid*)offsetof(sVertex, texCoords) );

    // vertex color
    glEnableVertexAttribArray(3);
    glVertexAttribPointer( 3, 4, GL_FLOAT, GL_FALSE, vertexSize, (GLvoid*)offsetof(sVertex, color) );

    // vertex boneIDs
    glEnableVertexAttribArray(4);
    glVertexAttribIPointer(4, 4, GL_INT, vertexSize, (GLvoid*)offsetof(sVertex, boneIDs));

    // vertex bone weights
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, vertexSize, (GLvoid*)offsetof(sVertex, weights));

    // matrices
    glGenBuffers( 1, &(meshDrawInfo->MatModelBufferID) );
	glBindBuffer(GL_ARRAY_BUFFER, meshDrawInfo->MatModelBufferID);

    size_t vec4Size = sizeof(glm::vec4);

    glm::mat4 matModel = glm::mat4(1.0f);

	glBufferData( GL_ARRAY_BUFFER, 
                  4 * vec4Size * meshDrawInfo->numberOfMatrices,
                  static_cast<GLvoid*> (&matModel), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)0);

    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(vec4Size));

    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(2 * vec4Size));

    glEnableVertexAttribArray(9);
    glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(3 * vec4Size));

    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);
    glVertexAttribDivisor(8, 1);
    glVertexAttribDivisor(9, 1);
   
    // indices
	glGenBuffers( 1, &(meshDrawInfo->IndexBufferID) );
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshDrawInfo->IndexBufferID);

	glBufferData( GL_ELEMENT_ARRAY_BUFFER,		
	              sizeof(unsigned int) * meshDrawInfo->numberOfIndices,
                  static_cast<GLvoid*> (& meshDrawInfo->indices[0]),
                  GL_STATIC_DRAW );

	glBindVertexArray(0);

    return;
}

bool cModelManager::m_LoadModelAssimp(const std::string& fileAndPath)
{
    Assimp::Importer importer;

    unsigned int flags = 0;
    //flags |= aiProcess_SortByPType;
    //flags |= aiProcess_LimitBoneWeights;
    //flags |= aiProcess_CalcTangentSpace;
    //flags |= aiProcess_JoinIdenticalVertices;
    flags |= aiProcess_GenNormals;
    //flags |= aiProcess_GenUVCoords;
    //flags |= aiProcess_TransformUVCoords;
    //flags |= aiProcess_GenSmoothNormals;
    flags |= aiProcess_Triangulate;
    flags |= aiProcess_FlipUVs;
    //flags |= aiProcess_MakeLeftHanded;
    //flags |= aiProcess_FixInfacingNormals;
    //flags |= aiProcess_FindDegenerates;
    //flags |= aiProcess_FindInvalidData;

    const aiScene* scene = importer.ReadFile(fileAndPath, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "Error loading scene from file '" << fileAndPath << "'";
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return false;
    }
    
    m_ProcessNode(scene->mRootNode, scene);

    return true;
}

void cModelManager::m_ProcessNode(aiNode* node, const aiScene* scene)
{
    // process all the node's meshes
    int numOfMeshes = 0;
    std::string name = node->mName.C_Str();

    numOfMeshes = node->mNumMeshes;

    for (unsigned int i = 0; i < numOfMeshes; i++)
    {   
        cMeshDrawInfo* meshDrawInfo = new cMeshDrawInfo();

        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        m_ProcessMesh(mesh, scene, meshDrawInfo);
        m_SetupMesh(meshDrawInfo);

        m_pTempModel->vpMeshesOfModel.push_back(meshDrawInfo);
    }

    // do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        m_ProcessNode(node->mChildren[i], scene);
    }
}

void cModelManager::m_ProcessMesh(aiMesh* mesh, const aiScene* scene, cMeshDrawInfo* meshDrawInfo)
{
    if (this->m_CheckPlyFileFormat(m_pTempModel->modelName))
    {
        this->m_LoadPly_XYZ_N_RGBA_UV(m_pTempModel->modelName, meshDrawInfo);
        return;
    }

    meshDrawInfo->numberOfVertices = mesh->mNumVertices;
    meshDrawInfo->numberOfTriangles = mesh->mNumFaces;
    meshDrawInfo->numberOfIndices = mesh->mNumFaces * 3;
    meshDrawInfo->numberOfBones = mesh->mNumBones;

    // vertex
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        sVertex vertex;

        this->m_SetDefaultBoneData(vertex);

        glm::vec4 tempVector = glm::vec4(0.0);

        // vertex positions
        tempVector.x = mesh->mVertices[i].x;
        tempVector.y = mesh->mVertices[i].y;
        tempVector.z = mesh->mVertices[i].z;
        tempVector.w = 1.0f;

        vertex.position = tempVector;

        // vertex normals
        tempVector.x = mesh->mNormals[i].x;
        tempVector.y = mesh->mNormals[i].y;
        tempVector.z = mesh->mNormals[i].z;
        tempVector.w = 0.0f;

        vertex.normal = tempVector;

        // vertex colors
        tempVector.r = 0.5f;
        tempVector.g = 0.5f;
        tempVector.b = 0.5f;
        tempVector.a = 1.0f;

        if (mesh->mColors[0] != NULL)
        {
            tempVector.r = mesh->mColors[0][i].r;
            tempVector.g = mesh->mColors[0][i].g;
            tempVector.b = mesh->mColors[0][i].b;
        }

        vertex.color = tempVector;

        // vertex texture coordinates
        glm::vec2 tempVec = glm::vec2(0.0f);

        if (mesh->mTextureCoords[0])
        {
            tempVec.s = mesh->mTextureCoords[0][i].x;
            tempVec.t = mesh->mTextureCoords[0][i].y; 
        }

        vertex.texCoords = tempVec;

        meshDrawInfo->vertices.push_back(vertex);
    }

    // indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            meshDrawInfo->indices.push_back(face.mIndices[j]);
        }
    }

    // materials
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        m_FindNumberOfTypes(material);
        // 1. diffuse maps

        std::vector<sTexture*> diffuseMaps = m_LoadMaterialTextures(material, aiTextureType_DIFFUSE, texture_diffuse);
        meshDrawInfo->textures.insert(meshDrawInfo->textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        std::vector<sTexture*> specularMaps = m_LoadMaterialTextures(material, aiTextureType_SPECULAR, texture_specular);
        meshDrawInfo->textures.insert(meshDrawInfo->textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<sTexture*> normalMaps = m_LoadMaterialTextures(material, aiTextureType_NORMALS, texture_normal);
        meshDrawInfo->textures.insert(meshDrawInfo->textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<sTexture*> heightMaps = m_LoadMaterialTextures(material, aiTextureType_HEIGHT, texture_height);
        meshDrawInfo->textures.insert(meshDrawInfo->textures.end(), heightMaps.begin(), heightMaps.end());
        // 4. opacity maps (black is opaque)
        std::vector<sTexture*> opacityMaps = m_LoadMaterialTextures(material, aiTextureType_OPACITY, texture_opacity_black);
        meshDrawInfo->textures.insert(meshDrawInfo->textures.end(), heightMaps.begin(), heightMaps.end());
        // 5. emission maps
        std::vector<sTexture*> emissionMaps = m_LoadMaterialTextures(material, aiTextureType_EMISSIVE, texture_emission);
        meshDrawInfo->textures.insert(meshDrawInfo->textures.end(), emissionMaps.begin(), emissionMaps.end());
    }

    // bones
    this->m_ExtractBoneWeights(meshDrawInfo, mesh);

    return;
}

bool cModelManager::m_CheckPlyFileFormat(const std::string& fileName)
{
    size_t nameSize = fileName.size();
    std::string format;
    std::string::const_iterator it;
    int index = 1;
    while (index != 5)
    {
        if (fileName[nameSize - index] == ' ')
        {
            continue;
        }
        else
        {
            it = format.begin();
            format.insert(it, fileName[nameSize - index]);
            index++;
        }
    }

    if (format == ".ply")
    {
        return true;
    }

    return false;
}

void cModelManager::m_LoadPly_XYZ_N_RGBA_UV(const std::string& fileName, cMeshDrawInfo* meshDrawInfo)
{
    // Input filestream 
    std::ifstream thePlyFile(this->directory + "/" + fileName);
    if (!thePlyFile.is_open())
    {
        return;
    }

    bool bHasNormals = false;
    bool bHasColors = false;
    bool bHasUVCoords = false;

    std::string temp;

    while (thePlyFile >> temp)
    {
        if (temp == "vertex")
        {
            break;
        }
    }

    thePlyFile >> meshDrawInfo->numberOfVertices;

    while (thePlyFile >> temp)
    {
        if (temp == "nx")
        {
            bHasNormals = true;
        }
        if (temp == "red")
        {
            bHasColors = true;
        }
        if (temp == "texture_u")
        {
            bHasUVCoords = true;
        }
        if (temp == "face")
        {
            break;
        }
    }

    thePlyFile >> meshDrawInfo->numberOfTriangles;

    meshDrawInfo->numberOfIndices = meshDrawInfo->numberOfTriangles * 3;

    while (thePlyFile >> temp)
    {
        if (temp == "end_header")
        {
            break;
        }
    }

    for (unsigned int index = 0; index != meshDrawInfo->numberOfVertices; index++)
    {
        sVertex vertex;

        glm::vec4 tempVector = glm::vec4(0.0f);

    // vertex positions
        thePlyFile >> tempVector.x;                
        thePlyFile >> tempVector.y;                
        thePlyFile >> tempVector.z;            
        tempVector.w = 1.0f;

        vertex.position = tempVector;

    // vertex normals
        tempVector.x = 0.0f;
        tempVector.y = 0.0f;
        tempVector.z = 0.0f;
        tempVector.w = 0.0f;

        if (bHasNormals)
        {
            thePlyFile >> tempVector.x;
            thePlyFile >> tempVector.y;
            thePlyFile >> tempVector.z;
        }

        vertex.normal = tempVector;

    // vertex colors
        tempVector.r = 0.5f;
        tempVector.g = 0.5f;
        tempVector.b = 0.5f;
        tempVector.a = 1.0f;

        if (bHasColors)
        {
            thePlyFile >> tempVector.r;       tempVector.r /= 255.0f;
            thePlyFile >> tempVector.g;       tempVector.g /= 255.0f;
            thePlyFile >> tempVector.b;       tempVector.b /= 255.0f;
            thePlyFile >> tempVector.a;       tempVector.a = 1.0f;
        }

        vertex.color = tempVector;

    // vertex texture coordinates
        glm::vec2 tempVec = glm::vec2(0.0f);

        if (bHasUVCoords)
        {
            thePlyFile >> tempVec.s;
            thePlyFile >> tempVec.t;
        }

        vertex.texCoords = tempVec;

        meshDrawInfo->vertices.push_back(vertex);
    }

    // indices
    for (unsigned int i = 0; i < meshDrawInfo->numberOfTriangles; i++)
    {
        unsigned int tempInt;
        thePlyFile >> tempInt; // discard

        thePlyFile >> tempInt; // 1st indice
        meshDrawInfo->indices.push_back(tempInt);

        thePlyFile >> tempInt; // 2nd indice
        meshDrawInfo->indices.push_back(tempInt);

        thePlyFile >> tempInt; // 3rd indice
        meshDrawInfo->indices.push_back(tempInt);
    }
    
    return;
}

void cModelManager::m_SetDefaultBoneData(sVertex& vertex)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        vertex.boneIDs[i] = -1;
        vertex.weights[i] = 0.0f;
    }
}

void cModelManager::m_ExtractBoneWeights(cMeshDrawInfo* meshInfo, aiMesh* mesh)
{
    cAssimpHelper AssimpHelper;

    for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
    {
        int boneID = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();

        if (m_pTempModel->GetBoneInfoMap().find(boneName) == m_pTempModel->GetBoneInfoMap().end())
        {
            sBoneInfo newBoneInfo;
            newBoneInfo.id = m_pTempModel->GetBoneCount();
            newBoneInfo.offset = AssimpHelper.AssimpToGLM(mesh->mBones[boneIndex]->mOffsetMatrix);
            m_pTempModel->GetBoneInfoMap()[boneName] = newBoneInfo;
            boneID = m_pTempModel->GetBoneCount();
            m_pTempModel->GetBoneCount()++;
        }
        else
        {
            boneID = m_pTempModel->GetBoneInfoMap()[boneName].id;
        }

        if (boneID == -1)
        {
            continue;
        }
       
        aiVertexWeight* weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
        {
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            assert(vertexId <= meshInfo->vertices.size());

            float finalWeight = 0.0f;
            for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
            {
                if (meshInfo->vertices[vertexId].boneIDs[i] < 0)
                {
                    meshInfo->vertices[vertexId].weights[i] = weight;
                    meshInfo->vertices[vertexId].boneIDs[i] = boneID;
                    break;
                }
            }
        }
    }

    return;
}

void cModelManager::m_FindNumberOfTypes(aiMaterial* material)
{
    int numOfTextures = material->GetTextureCount(aiTextureType_AMBIENT);
    numOfTextures = material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION);
    numOfTextures = material->GetTextureCount(aiTextureType_BASE_COLOR);
    numOfTextures = material->GetTextureCount(aiTextureType_CLEARCOAT);
    numOfTextures = material->GetTextureCount(aiTextureType_DIFFUSE);
    numOfTextures = material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS);
    numOfTextures = material->GetTextureCount(aiTextureType_DISPLACEMENT);
    numOfTextures = material->GetTextureCount(aiTextureType_EMISSION_COLOR);
    numOfTextures = material->GetTextureCount(aiTextureType_EMISSIVE);
    numOfTextures = material->GetTextureCount(aiTextureType_HEIGHT);
}

std::vector<sTexture*> cModelManager::m_LoadMaterialTextures(aiMaterial* mat, aiTextureType type, eTextureType typeName) const
{
    std::vector<sTexture*> textures;
    int numOfTextures = mat->GetTextureCount(type);

    for (unsigned int i = 0; i < numOfTextures; i++)
    {
        aiString path;
        mat->GetTexture(type, i, &path);
        std::string fileName;
        int j = 0;
        for (unsigned int k = 0; k != path.length + j; k++)
        {
            int data = path.data[k];
            if (data == 0)
            {
                j++;
                continue;
            }
            else
            {
                fileName += char(data);
            }
        }

        sTexture* texture = nullptr;
        g_pTextureManager->SetBasePath(directory);
        texture = g_pTextureManager->LoadTextureFromFile(typeName, fileName, fileName, bIsFlippedUVs);

        if (texture != nullptr)
        {       
            textures.push_back(texture);
        }
    }

    return textures;
}
