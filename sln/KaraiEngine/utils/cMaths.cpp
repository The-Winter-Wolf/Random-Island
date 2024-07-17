#include "cMaths.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

cMaths::cMaths()
{
}

cMaths::~cMaths()
{
}

float cMaths::Barrycentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos)
{
    float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
    float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
    float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
    float l3 = 1.0f - l1 - l2;
    return l1 * p1.y + l2 * p2.y + l3 * p3.y;
}

void cMaths::DisassembleMatrix4(glm::mat4 matrix, glm::vec3& position, glm::vec3& scale, glm::quat& rotation)
{
    position = matrix[3];

    glm::mat3 mat3 = matrix;

    this->DisassembleMatrix3(mat3, scale, rotation);
}

void cMaths::DisassembleMatrix3(glm::mat3 matrix, glm::vec3& scale, glm::quat& rotation)
{
    scale.x = glm::length(glm::vec3(matrix[0]));
    scale.y = glm::length(glm::vec3(matrix[1]));
    scale.z = glm::length(glm::vec3(matrix[2]));

    float T = 1.0f + matrix[0][0] + matrix[1][1] + matrix[2][2];

    float S, X, Y, Z, W;

    if (T > 0.00000001f)
    {
        S = sqrt(T) * 2.0f;
        X = (matrix[1][2] - matrix[2][1]) / S;
        Y = (matrix[2][0] - matrix[0][2]) / S;
        Z = (matrix[0][1] - matrix[1][0]) / S;
        W = 0.25f * S;
    }
    else
    {
        if (matrix[0][0] > matrix[1][1] && matrix[0][0] > matrix[2][2]) // Column 0: 
        {	
            S = sqrt(1.0f + matrix[0][0] - matrix[1][1] - matrix[2][2]) * 2.0f;
            X = 0.25f * S;
            Y = (matrix[0][1] + matrix[1][0]) / S;
            Z = (matrix[2][0] + matrix[0][2]) / S;
            W = (matrix[1][2] - matrix[2][1]) / S;
        }
        else if (matrix[1][1] > matrix[2][2]) // Column 1:
        {			 
            S = sqrt(1.0f + matrix[1][1] - matrix[0][0] - matrix[2][2]) * 2.0f;
            X = (matrix[0][1] + matrix[1][0]) / S;
            Y = 0.25f * S;
            Z = (matrix[1][2] + matrix[2][1]) / S;
            W = (matrix[2][0] - matrix[0][2]) / S;
        }
        else // Column 2:
        {						
            S = sqrt(1.0f + matrix[2][2] - matrix[0][0] - matrix[1][1]) * 2.0f;
            X = (matrix[2][0] + matrix[0][2]) / S;
            Y = (matrix[1][2] + matrix[2][1]) / S;
            Z = 0.25f * S;
            W = (matrix[0][1] - matrix[1][0]) / S;          
        }
    }

    rotation = glm::quat(W, X, Y, Z);
}
