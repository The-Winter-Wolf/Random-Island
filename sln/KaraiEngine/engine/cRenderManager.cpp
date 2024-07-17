#include "cRenderManager.h"

#include "../global/common.h"
#include "../global/global.h"

#include <glm/gtc/type_ptr.hpp> 

#include "../objects/light/cLightHelper.h"
#include "../objects/light/cLightManager.h"
extern cLightManager* g_pLightManager;

#include "../graphics/shaders/cShaderManager.h"
extern cShaderManager* g_pShaderManager;

#include "../objects/object_3D/cObject3D.h"
extern cObject3D* g_pFindMeshByFriendlyName(std::string friendlyNameToFind);

#include "../graphics/models/cModelManager.h"
extern cModelManager* g_pModelManager;

#include "../system/time/cTime.h"
extern cTime* g_pTime;

cRenderManager* cRenderManager::m_pRenderManager = NULL;

cRenderManager* cRenderManager::GetRenderManager()
{
    if (cRenderManager::m_pRenderManager == NULL)
    {
        cRenderManager::m_pRenderManager = new cRenderManager();
    }
    return cRenderManager::m_pRenderManager;
}

void cRenderManager::InitObjectNUB(){}

cRenderManager::cRenderManager(){}

cRenderManager::~cRenderManager(){}

void cRenderManager::DrawObject(std::string friendlyName, glm::mat4 matModelParent)
{
    cObject3D* pCurrentObject = g_pFindMeshByFriendlyName(friendlyName);
    this->DrawObject(pCurrentObject, matModelParent);
}

void cRenderManager::DrawObject(cObject3D* pCurrentObject, glm::mat4 matModelParent)
{
    if (pCurrentObject == nullptr)
    {
        return;
    }

    int shaderProgramID = g_ShaderID;

    glm::mat4 matModel = matModelParent;

    matModel = matModel * pCurrentObject->GetMatModel();

    glm::mat4 matModel_InverseTranspose = glm::inverse(glm::transpose(matModel));

    g_pShaderManager->SetUniform(shaderProgramID, "matModel_IT", matModel_InverseTranspose);

    if (pCurrentObject->GetAnimator() != nullptr)
    {
        g_pShaderManager->SetUniform(shaderProgramID, "bHasAnimation", true);
        std::vector<glm::mat4> vecBoneMatrices = pCurrentObject->GetAnimator()->GetFinalBoneMatrices();
        glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "finalBonesMatrices"), static_cast<GLsizei>(vecBoneMatrices.size()), GL_FALSE, glm::value_ptr(vecBoneMatrices[0]));
    }
    else
    {
        g_pShaderManager->SetUniform(shaderProgramID, "bHasAnimation", false);
    }

    if (bIsBoneColor)
    {
        g_pShaderManager->SetUniform(shaderProgramID, "bUseBoneColor", true);
    }
    else
    {
        g_pShaderManager->SetUniform(shaderProgramID, "bUseBoneColor", false);
    }
    
    if (pCurrentObject->bHasLight)
    {
        g_pShaderManager->SetUniform(shaderProgramID, "bHasLight", true);
    }
    else
    {
        g_pShaderManager->SetUniform(shaderProgramID, "bHasLight", false);
    }

    if (pCurrentObject->GetOpacity() != 1.0f)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        g_pShaderManager->SetUniform(shaderProgramID, "alphaOpacity", pCurrentObject->GetOpacity());
    }
    else
    {
        glDisable(GL_BLEND);
    }

    if (pCurrentObject->bHasColor)
    {
        g_pShaderManager->SetUniform(shaderProgramID, "bUseColor", true);
        g_pShaderManager->SetUniform(shaderProgramID, "colorRGBA", pCurrentObject->GetColor());
    }
    else
    {
        g_pShaderManager->SetUniform(shaderProgramID, "bUseColor", false);
    }

    if (pCurrentObject->bIsWireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (pCurrentObject->bIsTwoSided)
    {
        glDisable(GL_CULL_FACE);
    }
    else
    {
        glEnable(GL_CULL_FACE);
    }

    bool bGenNewNormals = false;
    if ( bGenNewNormals )
    {
        g_pShaderManager->SetUniform(shaderProgramID, "bGenNewNormals", true);
    }
    else
    {
        g_pShaderManager->SetUniform(shaderProgramID, "bGenNewNormals", false);
    }

    bool bExplodeObject = false;
    if ( bExplodeObject )
    {
        g_pShaderManager->SetUniform(shaderProgramID, "time", g_pTime->GetGLFWTime());
        g_pShaderManager->SetUniform(shaderProgramID, "bExplodeObject", true);
    }
    else
    {
        g_pShaderManager->SetUniform(shaderProgramID, "bExplodeObject", false);
    }

    cModel* pTempModel = pCurrentObject->GetModel();
    
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
            if (pTempModel->vpMeshesOfModel[i]->textures.size() == 0)
            {
                g_pShaderManager->SetUniform(shaderProgramID, "bHasTexture", false);
                g_pShaderManager->SetUniform(shaderProgramID, "bWhiteOpacity", false);
                g_pShaderManager->SetUniform(shaderProgramID, "bBlackOpacity", false);
                g_pShaderManager->SetUniform(shaderProgramID, "bUseHeightMap", false);
                g_pShaderManager->SetUniform(shaderProgramID, "bUseEmissionMap", false);
                g_pShaderManager->SetUniform(shaderProgramID, "bUseSpecularMap", false);
            }
            else
            {
                g_pShaderManager->SetUniform(shaderProgramID, "bHasTexture", true);

                unsigned int diffuseNr = 1;
                unsigned int specularNr = 1;
                unsigned int normalNr = 1;
                unsigned int heightNr = 1;
                unsigned int opacityNr = 1;
                unsigned int emissionNr = 1;

                for (unsigned int j = 0; j < pTempModel->vpMeshesOfModel[i]->textures.size(); j++)
                {
                    glActiveTexture(GL_TEXTURE0 + j);

                    g_pShaderManager->SetUniform(shaderProgramID, "bWhiteOpacity", false);
                    g_pShaderManager->SetUniform(shaderProgramID, "bBlackOpacity", false);
                    g_pShaderManager->SetUniform(shaderProgramID, "bUseHeightMap", false);
                    g_pShaderManager->SetUniform(shaderProgramID, "bUseEmissionMap", false);
                    g_pShaderManager->SetUniform(shaderProgramID, "bUseSpecularMap", false);

                    std::string name;
                    eTextureType type = pTempModel->vpMeshesOfModel[i]->textures[j]->textureType;

                    g_pShaderManager->SetUniform(shaderProgramID, "UVOffset", 
                        pTempModel->vpMeshesOfModel[i]->textures[j]->UVOffset);
                    g_pShaderManager->SetUniform(shaderProgramID, "mixRatio", 
                        pTempModel->vpMeshesOfModel[i]->textures[j]->mixRatio);
                    g_pShaderManager->SetUniform(shaderProgramID, "texScale",
                        pTempModel->vpMeshesOfModel[i]->textures[j]->texScale);

                    if (type == texture_diffuse)
                        name = "texture_diffuse" + std::to_string(diffuseNr++);
                    else if (type == texture_specular)
                    {
                        g_pShaderManager->SetUniform(shaderProgramID, "bUseSpecularMap", true);
                        name = "texture_specular" + std::to_string(specularNr++);
                    }
                    else if (type == texture_normal)
                        name = "texture_normal" + std::to_string(normalNr++);
                    else if (type == texture_emission)
                    {
                        g_pShaderManager->SetUniform(shaderProgramID, "bUseEmissionMap", true);
                        name = "texture_emission" + std::to_string(emissionNr++);
                    }
                    else if (type == texture_height)
                    {
                        g_pShaderManager->SetUniform(shaderProgramID, "bUseHeightMap", true);
                        name = "texture_height" + std::to_string(heightNr++);

                        g_pShaderManager->SetUniform(shaderProgramID, "heightScale", 
                        pTempModel->vpMeshesOfModel[i]->textures[j]->heightMapScale);
                    }
                    else if (type == texture_opacity_black)// If "white" then discard
                    {
                        g_pShaderManager->SetUniform(shaderProgramID, "bBlackOpacity", true);
                        name = "texture_opacity" + std::to_string(opacityNr++);
                    }
                    else if (type == texture_opacity_white)// If "black" then discard
                    {
                        g_pShaderManager->SetUniform(shaderProgramID, "bWhiteOpacity", true);
                        name = "texture_opacity" + std::to_string(opacityNr++);
                    }

                    glUniform1i(glGetUniformLocation(shaderProgramID, (name).c_str()), j);
                    glBindTexture(GL_TEXTURE_2D, pTempModel->vpMeshesOfModel[i]->textures[j]->textureID);
                }
            }

            glBindVertexArray(pTempModel->vpMeshesOfModel[i]->VAO_ID);
            glDrawElementsInstanced(GL_TRIANGLES, pTempModel->vpMeshesOfModel[i]->numberOfIndices, GL_UNSIGNED_INT, 0, numOfInstances);

            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    return;
}

void cRenderManager::DrawDebugSphere(glm::vec3 position, float scale, glm::vec4 colourRGBA, bool isTwoSided)
{
    if (! g_bDrawDebugObjects)
    {
        return;
    }

    bool OLD_isVisible = g_pDebugSphereObject->bIsVisible;
    bool OLD_isTwoSided = g_pDebugSphereObject->bIsTwoSided;
    glm::vec3 OLD_position = g_pDebugSphereObject->GetPosition();
    glm::vec3 OLD_scale = g_pDebugSphereObject->GetScale();
    glm::vec4 OLD_colours = g_pDebugSphereObject->GetColor();

    g_pDebugSphereObject->bIsVisible = true;
    g_pDebugSphereObject->bIsTwoSided = isTwoSided;
    g_pDebugSphereObject->SetPosition(position);
    g_pDebugSphereObject->SetScale(glm::vec3(scale));
    g_pDebugSphereObject->SetColor(colourRGBA);

    DrawObject(g_pDebugSphereObject, glm::mat4(1.0f));

    g_pDebugSphereObject->bIsVisible = OLD_isVisible;
    g_pDebugSphereObject->bIsTwoSided = OLD_isTwoSided;
    g_pDebugSphereObject->SetPosition(OLD_position);
    g_pDebugSphereObject->SetScale(OLD_scale);
    g_pDebugSphereObject->SetColor(OLD_colours);

    return;
}

void cRenderManager::DrawLightDebugSpheres()
{
    if (!g_bDrawDebugLightSpheres)
    {
        return;
    }

    DrawDebugSphere(g_pLightManager->GetLight(g_SelectedLight)->position,
                    0.5f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    cLightHelper lightHelper;

    float constantAtten = g_pLightManager->GetLight(g_SelectedLight)->atten.x;
    float linearAtten = g_pLightManager->GetLight(g_SelectedLight)->atten.y;
    float quadAtten = g_pLightManager->GetLight(g_SelectedLight)->atten.z;

    // Draw a red sphere at 75% brightness
    {
        float distAt75Percent = lightHelper.calcApproxDistFromAtten(0.75f, 0.01f, 100000.0f,
            constantAtten, linearAtten, quadAtten, 50);

        DrawDebugSphere(g_pLightManager->GetLight(g_SelectedLight)->position,
                        distAt75Percent,
                        glm::vec4(0.5f, 0.0f, 0.0f, 1.0f));
    }

    // Draw a green sphere at 50% brightness
    {
        float distAt50Percent = lightHelper.calcApproxDistFromAtten(0.50f, 0.01f, 100000.0f,
                                                                    constantAtten, linearAtten, quadAtten, 50);

        DrawDebugSphere(g_pLightManager->GetLight(g_SelectedLight)->position,
                        distAt50Percent,
                        glm::vec4(0.0f, 0.5f, 0.0f, 1.0f));
    }

    // Draw a yellow sphere at 25% brightness
    {
        float distAt25Percent = lightHelper.calcApproxDistFromAtten(0.25f, 0.01f, 100000.0f,
                                                                    constantAtten, linearAtten, quadAtten, 50);

        DrawDebugSphere(g_pLightManager->GetLight(g_SelectedLight)->position,
                        distAt25Percent,
                        glm::vec4(0.5f, 0.5f, 0.0f, 1.0f));
    }

    // Draw a blue sphere at 1% brightness
    {
        float distAt_5Percent = lightHelper.calcApproxDistFromAtten(0.01f, 0.01f, 100000.0f,
                                                                    constantAtten, linearAtten, quadAtten, 50);

        DrawDebugSphere(g_pLightManager->GetLight(g_SelectedLight)->position,
                        distAt_5Percent,
                        glm::vec4(0.0f, 0.0f, 0.5f, 1.0f));
    }

    return;
}