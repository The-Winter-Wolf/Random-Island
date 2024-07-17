#pragma once

#include <assimp/scene.h>

#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <string>
#include <vector>

struct KeyPosition
{
    glm::vec3 position;
    double timeStamp;
};

struct KeyRotation
{
    glm::quat orientation;
    double timeStamp;
};

struct KeyScale
{
    glm::vec3 scale;
    double timeStamp;
};

class cBone
{
public: 
    cBone(const std::string& name, int ID, const aiNodeAnim* channel);
    ~cBone();

    void Update(double animationTime);

    glm::mat4 GetLocalTransform();
    std::string GetBoneName();
    int GetBoneID();

private:

    int m_ID;
    std::string m_Name;
    glm::mat4 m_LocalTransform;

    int m_NumPositions;
    int m_NumRotations;
    int m_NumScalings;

    std::vector<KeyPosition> m_Positions;
    std::vector<KeyRotation> m_Rotations;
    std::vector<KeyScale> m_Scales;

    int m_GetPositionIndex(double animationTime);

    int m_GetRotationIndex(double animationTime);

    int m_GetScaleIndex(double animationTime);

    float m_GetScaleFactor(double lastTimeStamp, double nextTimeStamp, double animationTime);

    glm::mat4 m_InterpolatePosition(double animationTime);

    glm::mat4 m_InterpolateRotation(double animationTime);

    glm::mat4 m_InterpolateScaling(float animationTime);
};

