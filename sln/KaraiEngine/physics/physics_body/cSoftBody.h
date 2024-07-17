#pragma once

#include "../../graphics/models/cModel.h"

#include <glm/glm.hpp>
#include <string>
#include <vector>

class cSoftBody
{
public:
	cSoftBody();
	~cSoftBody();

	struct sParticle
	{
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 old_position = glm::vec3(0.0f);
		sVertex* pVertex = nullptr;
	};

	struct sConstraint
	{
		sConstraint() {}
		sParticle* pParticleA = nullptr;
		sParticle* pParticleB = nullptr;
		float restLength = 0.0f;
		float tightnessFactor = 1.0f;

		unsigned int numIterations = 1;

		bool bIsActive = true;
		bool bIsLocked = false;
	};

	struct sSoftMesh
	{
		std::vector< sParticle* > vecParticles;
		std::vector< sConstraint* > vecConstraints;
	};

	struct sAttachedParticles
	{
		int particleIndex = 0;
		glm::vec3 oldPosition{ 0.0f };
		glm::vec3 attachPos{ 0.0f };
	};

	cModel* CreateSoftModel(const std::string& modelName, const glm::mat4& matTransform = glm::mat4(1.0f));

	void CreateRandomBracing(int numberOfBraces, float minDistance);

	void UpdateSoftBody(const glm::vec3& velocity, float deltaTime);

	bool AttachParticle(const glm::vec3& oldPos, const glm::vec3& attachPos);
	bool AttachParticle(int index, const glm::vec3& attachPos);
	bool AttachParticle(int index);

	void FastenParticles(void);
	void UnfastenParticles(void);

	void BrokeRandConstr(void);
	glm::vec3 GetCenterPos(void) const;

	bool bIsConvert;
	std::vector< sSoftMesh* > m_vecSoftMeshes;

private:

	bool bAttach;

	glm::vec3 m_center;

	glm::mat4 m_matModel;

	cModel* m_pSoftModel;

	std::vector< sAttachedParticles* > m_vecAttach;

	bool m_UpdateDrawInfo(void);

	void m_UpdateCenterPos(void);

	void m_SatisfyConstraints(void);

	void m_UpdateVertexNormals(void);

	void m_UpdateVertexPositions(void);

	void m_CleanZeros(glm::vec3& value);

	void m_CopyModelInfo(cModel* oldModel);

	float m_Distance(sParticle* pPartA, sParticle* pPartB);
};

