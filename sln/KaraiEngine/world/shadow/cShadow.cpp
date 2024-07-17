#include "cShadow.h"

#include "../../global/global.h"

#include "../../engine/cRenderManager.h"
extern cRenderManager* g_pRenderManager;

#include "../../graphics/shaders/cShaderManager.h"
extern cShaderManager* g_pShaderManager;

#include "../../graphics/models/cModelManager.h"
extern cModelManager* g_pModelManager;

#include "../terrain/cTerrain.h"
extern cTerrain* g_pTerrain;

#include "../../graphics/FBO/cFBO.h"
cFBO depthFBO;

#include "../../objects/camera/cCamera.h"
extern cCamera* g_pCamera;

cShadow::cShadow()
{
    this->shadow_width = 1024;
    this->shadow_height = 1024;
}

cShadow::~cShadow()
{
}

void cShadow::InitShadows()
{
    std::string FBOError;
    if (!depthFBO.InitFBO(shadow_width, shadow_height, true, true, FBOError))
    {
        g_pConsole->cError("Error creating depthFBO: " + FBOError);
        return;
    }
    else g_pConsole->cSuccess("depthFBO created.");
}

void cShadow::RenderDepthMap()
{
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO.ID);
    glViewport(0, 0, depthFBO.width, depthFBO.height);
    depthFBO.ClearBuffers();

    g_pTerrain->RenderTerrain();

    g_pShaderManager->UseShader("ShadowShader");

    float near_plane = 1.0f, far_plane = 10.5f;

    //glm::mat4 lightModel = glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 100.0f, 100.0f));

    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

    glm::mat4 lightView = glm::lookAt(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(200.0f, 0.0f, 200.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 lightMatrix = lightProjection * lightView /** lightModel*/;

    g_pShaderManager->SetUniform(g_ShaderID, "lightMatViewProj", lightMatrix);

    for (int index = 0; index < g_vpObjectToDraw.size(); index++)
    {
        if (g_vpObjectToDraw[index] == nullptr)
        {
           continue;
        }

        glm::mat4 matModel = glm::mat4(1.0f);

         matModel = matModel * g_vpObjectToDraw[index]->GetMatModel();

        cModel* pTempModel = g_vpObjectToDraw[index]->GetModel();

        if (pTempModel != nullptr)
        {
            int numOfInstances = pTempModel->matrices.size();

            if (numOfInstances == 1)
            {
                pTempModel->matrices[0] = matModel;
                g_pModelManager->UpdateMatrixBuffers(pTempModel);
            }

            for (unsigned int i = 0; i != pTempModel->vpMeshesOfModel.size(); i++)
            {
                glBindVertexArray(pTempModel->vpMeshesOfModel[i]->VAO_ID);
                glDrawElementsInstanced(GL_TRIANGLES, pTempModel->vpMeshesOfModel[i]->numberOfIndices, GL_UNSIGNED_INT, 0, numOfInstances);

                glBindVertexArray(0);
            }
        }
    }
 
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    DebugDepthRender();
    
    g_pShaderManager->UseShader("TerrainShader");

    int textureNumber = 10;
    glActiveTexture(GL_TEXTURE0 + textureNumber);
    glBindTexture(GL_TEXTURE_2D, depthFBO.depthTexture_ID);
    glUniform1i(glGetUniformLocation(g_ShaderID, "shadowMap"), textureNumber);
    g_pShaderManager->SetUniform(g_ShaderID, "lightMatViewProj", lightMatrix);

    g_pShaderManager->UseShader("MainShader");

    textureNumber = 10;
    glActiveTexture(GL_TEXTURE0 + textureNumber);
    glBindTexture(GL_TEXTURE_2D, depthFBO.depthTexture_ID);
    glUniform1i(glGetUniformLocation(g_ShaderID, "shadowMap"), textureNumber);
    g_pShaderManager->SetUniform(g_ShaderID, "lightMatViewProj", lightMatrix);
}

void cShadow::DebugDepthRender()
{
    g_pShaderManager->UseShader("DebugShader");

    glViewport(0, 0, 1200, 1200);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glCullFace(GL_BACK);

    glDisable(GL_BLEND);

    ////uniform bool bIsStaticScreen;
    //g_pShaderManager->SetUniform(g_ShaderID, "bIsStaticScreen", true);

    ////uniform bool bIsOffScreenTexture;
    //g_pShaderManager->SetUniform(g_ShaderID, "bIsOffScreenTexture", true);

    // uniform vec2 screenSize;
    glm::vec2 size = glm::vec2(depthFBO.width, depthFBO.height);
    g_pShaderManager->SetUniform(g_ShaderID, "screenSize", size);

    GLint textureUnitNumber = 0;
    GLint texture_off_screen_UL = 0;
    glm::mat4 matModel = glm::mat4(1.0);

    // FBO_01 to texture Screen01
    textureUnitNumber = 98;
    glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
    glBindTexture(GL_TEXTURE_2D, depthFBO.colourTexture_0_ID);

    //texture_off_screen_UL = glGetUniformLocation(g_ShaderID, "textureOffScreen");
    //glUniform1i(texture_off_screen_UL, textureUnitNumber);

    unsigned int quadVAO = 0;
    unsigned int quadVBO = 0;

    if (quadVAO == 0)
    {
        float quadVertices[] = 
        {
            // positions        // texture Coords
            -10.0f,  10.0f, 0.0f, 0.0f, 1.0f,
            -10.0f, -10.0f, 0.0f, 0.0f, 0.0f,
             10.0f,  10.0f, 0.0f, 1.0f, 1.0f,
             10.0f, -10.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
