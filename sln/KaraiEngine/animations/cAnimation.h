#pragma once
#include "cBone.h"
#include "../graphics/models/cModel.h"

struct sAssimpNodeData
{
    glm::mat4 transformation = glm::mat4(1.0f);
    std::string name;
    int childrenCount = 0;
    std::vector<sAssimpNodeData> children;
};

class cAnimation
{
public:
    cAnimation();
    ~cAnimation();

    void LoadAnimationFromFile(const std::string& animationName, cModel* model);

    cBone* FindBone(const std::string& name);

    void SetBasePath(const std::string& animationPath);
    std::string GetBasePath();

    std::string GetAnimationName();

    float GetTicksPerSecond();

    float GetDuration();

    const sAssimpNodeData& GetRootNode();

    const std::map<std::string, sBoneInfo>& GetBoneIDMap();

private:

    void m_ReadMissingBones(const aiAnimation* animation, cModel* mesh);

    void m_ReadHeirarchyData(sAssimpNodeData& nodeData, aiNode* node);

    void m_FBXPatch(std::string& name);
    void m_FBXPatch(int& ticks, float& duration);
    void m_FBXPatch(sAssimpNodeData& nodeData, aiNode* &node);
    void m_PrintAnimData(aiNode* node);

    int m_NumChannels;
    float m_Duration;
    int m_TicksPerSecond;
    std::vector<cBone> m_vecBones;
    sAssimpNodeData m_sRootNode;
    std::map<std::string, sBoneInfo> m_BoneInfoMap;

    std::string m_AnimPath;
    std::string m_AnimName;
};

