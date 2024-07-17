#include "cAnimation.h"

#include "../utils/cAssimpHelper.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>

cAssimpHelper AssimpHelper;

cAnimation::cAnimation()
{
    this->m_NumChannels = 0;
    this->m_Duration = 0.0f;
    this->m_TicksPerSecond = 0;
    this->m_AnimPath = "../../extern/assets/animations"; //default
}

cAnimation::~cAnimation() {}

void cAnimation::LoadAnimationFromFile(const std::string& animationName, cModel* model)
{
    Assimp::Importer importer;

    m_AnimName = animationName;
    std::string fullPath = m_AnimPath + "/" + m_AnimName;

    const aiScene* scene = importer.ReadFile(fullPath, aiProcess_Triangulate);

    if (!scene || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    //m_NumAnimations = scene->mNumAnimations;

    aiAnimation* animation = scene->mAnimations[0];
    m_Duration = animation->mDuration;
    m_TicksPerSecond = animation->mTicksPerSecond;

    //this->m_FBXPatch(m_TicksPerSecond, m_Duration);

    this->m_ReadHeirarchyData(m_sRootNode, scene->mRootNode);
    this->m_ReadMissingBones(animation, model);

    m_sRootNode.transformation = glm::mat4(1.0f);
}

cBone* cAnimation::FindBone(const std::string& name)
{
    for (unsigned int index = 0; index != m_vecBones.size(); index++)
    {
        if (m_vecBones[index].GetBoneName() == name)
        {
            return &m_vecBones[index];
        }
    }

    return nullptr;
}

void cAnimation::SetBasePath(const std::string& animationPath)
{
    this->m_AnimPath = animationPath;
}

std::string cAnimation::GetBasePath()
{
    return this->m_AnimPath;
}

std::string cAnimation::GetAnimationName()
{
    return this->m_AnimName;
}

float cAnimation::GetTicksPerSecond()
{
    return this->m_TicksPerSecond;
}

float cAnimation::GetDuration()
{
    return this->m_Duration;
}

const sAssimpNodeData& cAnimation::GetRootNode()
{
    return this->m_sRootNode;
}

const std::map<std::string, sBoneInfo>& cAnimation::GetBoneIDMap()
{
    return this->m_BoneInfoMap;
}

void cAnimation::m_ReadMissingBones(const aiAnimation* animation, cModel* model)
{
    this->m_NumChannels = animation->mNumChannels;

    auto& boneInfoMap = model->GetBoneInfoMap();
    int& boneCount = model->GetBoneCount();

    for (int i = 0; i < m_NumChannels; i++)
    {
        aiNodeAnim* channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;

        //this->m_FBXPatch(boneName);

        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            boneInfoMap[boneName].id = boneCount;
            boneCount++;
        }

        m_vecBones.push_back( cBone(boneName, boneInfoMap[boneName].id, channel));
    }

    this->m_BoneInfoMap = boneInfoMap;
}

void cAnimation::m_ReadHeirarchyData(sAssimpNodeData& nodeData, aiNode* node)
{
    if (node == nullptr)
    {
        return;
    }

    this->m_FBXPatch(nodeData, node);

    for (int i = 0; i < node->mNumChildren; i++)
    {
        sAssimpNodeData childNodeData;
        m_ReadHeirarchyData(childNodeData, node->mChildren[i]);
        nodeData.children.push_back(childNodeData);
    }
}

void cAnimation::m_FBXPatch(int& ticks, float& duration)
{

}
void cAnimation::m_FBXPatch(std::string& name)
{
    for (int i = 0; i < name.size(); i++)
    {
        if (name[i] == ':')
        {
            name[i] = '_';
        }
    }
}
void cAnimation::m_FBXPatch(sAssimpNodeData& nodeData, aiNode* &node)
{
    bool bFBXnode = false;

    glm::mat4 matModel = glm::mat4(1.0f);

    aiNode* firstNode = node;
    std::string tempName = firstNode->mName.data;

    for (int i = 0; i < tempName.size(); i++)
    {
        if (tempName[i] == ':')
        {
            tempName[i] = '_';
        }
        if (tempName[i] == '$')
        {
            bFBXnode = true;
        }
    }

    nodeData.transformation *= AssimpHelper.AssimpToGLM(firstNode->mTransformation);

    if (!bFBXnode || firstNode->mNumChildren == 0)
    {
        nodeData.name = tempName;
        nodeData.childrenCount = firstNode->mNumChildren;
        node = firstNode;
        return;
    }

    m_FBXPatch(nodeData, node->mChildren[0]);
    node = node->mChildren[0];
    return;
}

void cAnimation::m_PrintAnimData(aiNode* node)
{
    std::cout << node->mName.data << " " << node->mNumChildren << std::endl;
}