#include "cAssimpHelper.h"

glm::vec2 cAssimpHelper::AssimpToGLM(const aiVector2D& input)
{
    glm::vec2 output;

    output.x = input.x;
    output.y = input.y;

    return output;
}

glm::vec3 cAssimpHelper::AssimpToGLM(const aiVector3D& input)
{
    glm::vec3 output;

    output.x = input.x;
    output.y = input.y;
    output.z = input.z;

    return output;
}

glm::mat4 cAssimpHelper::AssimpToGLM(const aiMatrix4x4& input)
{
    glm::mat4 output;

    output[0][0] = input.a1; output[1][0] = input.a2; output[2][0] = input.a3; output[3][0] = input.a4;
    output[0][1] = input.b1; output[1][1] = input.b2; output[2][1] = input.b3; output[3][1] = input.b4;
    output[0][2] = input.c1; output[1][2] = input.c2; output[2][2] = input.c3; output[3][2] = input.c4;
    output[0][3] = input.d1; output[1][3] = input.d2; output[2][3] = input.d3; output[3][3] = input.d4;

    return output;
}

glm::quat cAssimpHelper::AssimpToGLM(const aiQuaternion& input)
{
    glm::quat output;

    output.w = input.w;
    output.x = input.x;
    output.y = input.y;
    output.z = input.z;

    return output;
}
