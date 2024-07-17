#include "cDebugRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "../graphics/models/cModelManager.h"
cModelManager* cDebugMeshManager = NULL;

#include "../graphics/shaders/cShaderManager.h"
cShaderManager* cDebugShaderManager = NULL;

#include "../physics/physics_world/cAABBsWorld.h"

GLuint cDebugRenderer_VBO = 0;
GLuint cDebugRenderer_VAO = 0;

cDebugRenderer::cDebugRenderer()
{
    this->m_sizeOfLineVertexBuffer = 0;
    this->m_pLinesVerticesToDraw = NULL;
}

float getRandFloat(float a, float b) {
    float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

bool cDebugRenderer::Initialize()
{
    cDebugShaderManager = cShaderManager::GetShaderManager();

    cDebugShaderManager->SetBasePath("../../extern/assets/shaders");

    if (!cDebugShaderManager->CreateProgramFromFile("DebugObjectShader", "debugShader.vert", "debugShader.frag"))
    {
        this->m_lastError = "ERROR: " + cDebugShaderManager->GetLastError();
        return false;
    }

    GLuint debugShaderProgramID = cDebugShaderManager->GetProgramID("DebugObjectShader");

    this->m_pLinesVerticesToDraw = new sLineVertex_Buffer[LINE_VERTEX_BUFFER_SIZE];

    memset(this->m_pLinesVerticesToDraw, 0, 
           LINE_VERTEX_BUFFER_SIZE * sizeof(sLineVertex_Buffer));
  
    this->m_sizeOfLineVertexBuffer = 0;

    // *********************************************************************************
  
    const float MAX_LINE_POSITION = 1000.0f;

    for ( int count = 0; count != 500; count++ )
    {
        glm::vec3 lineStart = glm::vec3(getRandFloat(-MAX_LINE_POSITION, MAX_LINE_POSITION),
                                        getRandFloat(-MAX_LINE_POSITION, MAX_LINE_POSITION),
                                        getRandFloat(-MAX_LINE_POSITION, MAX_LINE_POSITION));

        glm::vec3 lineEnd = glm::vec3(getRandFloat(-MAX_LINE_POSITION, MAX_LINE_POSITION),
                                      getRandFloat(-MAX_LINE_POSITION, MAX_LINE_POSITION),
                                      getRandFloat(-MAX_LINE_POSITION, MAX_LINE_POSITION));

        glm::vec4 lineColour = glm::vec4(getRandFloat(0.0f, 1.0f),
                                         getRandFloat(0.0f, 1.0f),
                                         getRandFloat(0.0f, 1.0f),
                                         1.0f);

        this->AddLine(lineStart, lineEnd, lineColour);
    }

    // *********************************************************************************

    glGenVertexArrays(1, &cDebugRenderer_VAO);

    glBindVertexArray(cDebugRenderer_VAO);

    glGenBuffers(1, &cDebugRenderer_VBO);

    glBindBuffer(GL_ARRAY_BUFFER, cDebugRenderer_VBO);

    unsigned int numberOfBytesToCopy = sizeof(sLineVertex_Buffer) * LINE_VERTEX_BUFFER_SIZE;

    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(numberOfBytesToCopy),
                 this->m_pLinesVerticesToDraw,  
                 GL_DYNAMIC_DRAW);

    GLint vPosition_location = glGetAttribLocation(debugShaderProgramID, "vPosition");
    GLint vColour_location = glGetAttribLocation(debugShaderProgramID, "vColour");

    glEnableVertexAttribArray(vPosition_location);
    glVertexAttribPointer(vPosition_location, 4,
                          GL_FLOAT, 
                          GL_FALSE,
                          sizeof(sLineVertex_Buffer),                   
                          (void*) offsetof(sLineVertex_Buffer,x) );    

    glEnableVertexAttribArray(vColour_location);
    glVertexAttribPointer(vColour_location, 4,
                          GL_FLOAT, 
                          GL_FALSE,
                          sizeof(sLineVertex_Buffer), 
                          (void*)offsetof(sLineVertex_Buffer, r));   


    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(vPosition_location);
    glDisableVertexAttribArray(vColour_location);

    cDebugMeshManager = cModelManager::GetModelManager();
    cDebugMeshManager->SetBasePath("../../extern/assets/models");

    /*cModel sphereMeshInfo;*/
    cDebugMeshManager->LoadModelIntoVAO("Sphere_r1_xyz_n_rgba.ply", /*sphereMeshInfo,*/ debugShaderProgramID);

   /* cModel cubeMeshInfo;*/
    cDebugMeshManager->LoadModelIntoVAO("Cube_1x1x1_xyz_n_rgba.ply", /*cubeMeshInfo,*/ debugShaderProgramID);

	return true;
}

std::string cDebugRenderer::GetLastError(void)
{
    std::string theLastError = this->m_lastError;
    this->m_lastError = "";
    return theLastError;
}

void cDebugRenderer::ShutDown(void)
{
    delete cDebugShaderManager;
}

void cDebugRenderer::AddLine(glm::vec3 startXYZ, glm::vec3 endXYZ, glm::vec4 RGBA)
{
    this->m_pLinesVerticesToDraw[this->m_sizeOfLineVertexBuffer + 0].x = startXYZ.x;
    this->m_pLinesVerticesToDraw[this->m_sizeOfLineVertexBuffer + 0].y = startXYZ.y;
    this->m_pLinesVerticesToDraw[this->m_sizeOfLineVertexBuffer + 0].z = startXYZ.z;

    this->m_pLinesVerticesToDraw[this->m_sizeOfLineVertexBuffer + 0].r = RGBA.r;
    this->m_pLinesVerticesToDraw[this->m_sizeOfLineVertexBuffer + 0].g = RGBA.g;
    this->m_pLinesVerticesToDraw[this->m_sizeOfLineVertexBuffer + 0].b = RGBA.b;
    this->m_pLinesVerticesToDraw[this->m_sizeOfLineVertexBuffer + 0].a = RGBA.a;

    this->m_pLinesVerticesToDraw[this->m_sizeOfLineVertexBuffer + 1].x = endXYZ.x;
    this->m_pLinesVerticesToDraw[this->m_sizeOfLineVertexBuffer + 1].y = endXYZ.y;
    this->m_pLinesVerticesToDraw[this->m_sizeOfLineVertexBuffer + 1].z = endXYZ.z;

    this->m_pLinesVerticesToDraw[this->m_sizeOfLineVertexBuffer + 1].r = RGBA.r;
    this->m_pLinesVerticesToDraw[this->m_sizeOfLineVertexBuffer + 1].g = RGBA.g;
    this->m_pLinesVerticesToDraw[this->m_sizeOfLineVertexBuffer + 1].b = RGBA.b;
    this->m_pLinesVerticesToDraw[this->m_sizeOfLineVertexBuffer + 1].a = RGBA.a;

    this->m_sizeOfLineVertexBuffer += 2;

    return;
}

void cDebugRenderer::AddSphere(glm::vec3 location, float radius, glm::vec4 RGBA){}

void cDebugRenderer::AddCube(glm::vec3 centreLocation, float size, glm::vec4 RGBA){}

void cDebugRenderer::RenderDebugObjects(double deltaTime, glm::mat4 matView, glm::mat4 matProjection)
{
    GLuint shaderID = cDebugShaderManager->GetProgramID("debugObjectShader");


    glUseProgram(shaderID);

    unsigned int numberOfBytesToCopy = sizeof(sLineVertex_Buffer) * this->m_sizeOfLineVertexBuffer;


    GLint mvp_location = glGetUniformLocation(shaderID, "MVP");

    glm::mat4 matModel = glm::mat4(1.0f);

    glm::mat4 matMVP = matProjection * matView * matModel;

    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(matMVP));


    glBindVertexArray(cDebugRenderer_VAO);

    //Draw however many lines are in the buffer currently.
    glDrawArrays(GL_LINES, 0, this->m_sizeOfLineVertexBuffer);

    glBindVertexArray(0);

    return;
}