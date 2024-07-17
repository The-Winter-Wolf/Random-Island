#pragma once
#include <glm/glm.hpp>

class cRandomUtils
{
public:
    cRandomUtils();
    ~cRandomUtils();

    float GetRandomFloat();
    float GetRandomFloat(float min, float max);

    int GetRandomInt();
    int GetRandomInt(int min, int max);

    glm::vec3 GetRandomVec3();
    glm::vec3 GetRandomVec3(glm::vec3 min, glm::vec3 max);

    glm::vec4 GetRandomVec4();
    glm::vec4 GetRandomVec4(glm::vec4 min, glm::vec4 max);

};

