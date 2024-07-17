#include "cCommands_Special.h"

#include <vector>

extern cObject3D* g_pFindMeshByFriendlyName(std::string friendlyNameToFind);
extern std::vector< cObject3D* > g_vpObjectToDraw;

cCommands_Special::cCommands_Special()
{
	this->m_color = glm::vec4(-1.0f);
	this->isColor = false;
	this->m_visibility = true;
}

cCommands_Special::~cCommands_Special()
{
}

bool cCommands_Special::Stop()
{
	return false;
}

bool cCommands_Special::Update(float deltaTime)
{
	if (m_friendlyName != "")
	{
		cObject3D* newObject = new cObject3D();
		newObject->bHasColor = m_pObject->bHasColor;
		newObject->bHasLight = m_pObject->bHasLight;
		newObject->bIsVisible = m_pObject->bIsVisible;
		newObject->bIsWireframe = m_pObject->bIsWireframe;
		newObject->friendlyName = m_pObject->friendlyName;
		newObject->SetColor(m_pObject->GetColor());
		newObject->SetQuatOrientation(m_pObject->GetQuatOrientation());
		newObject->SetPosition(m_sbjXYZ);
		newObject->SetScale(m_pObject->GetScale());
		g_vpObjectToDraw.push_back(newObject);

		return true;
	}
	if (m_color.r >= 0.0f)
	{
		m_pObject->bHasColor = isColor;
		m_pObject->SetColor(m_color);

		return true;
	}
	else
	{
		m_pObject->bIsVisible = m_visibility;
		return true;
	}
}

std::string cCommands_Special::GetCommandName()
{
	return this->m_commandName;
}

void cCommands_Special::SetCommandName(std::string commandName)
{
	this->m_commandName = commandName;
}

void cCommands_Special::Initialize(std::string objectName, bool visibility)
{
	this->m_objectName = objectName;
	this->m_pObject = g_pFindMeshByFriendlyName(m_objectName);

	this->m_visibility = visibility;
}

void cCommands_Special::Initialize(std::string copyObjectName, std::string newFriendlyName, glm::vec3 posXYZ)
{
	this->m_objectName = copyObjectName;
	this->m_pObject = g_pFindMeshByFriendlyName(m_objectName);

	this->m_friendlyName = newFriendlyName;
	this->m_sbjXYZ = posXYZ;
}

void cCommands_Special::Initialize(std::string objectName, bool isColor, glm::vec4 newColor)
{
	this->m_objectName = objectName;
	this->m_pObject = g_pFindMeshByFriendlyName(m_objectName);

	this->isColor = isColor;

	if (newColor.r < 0.0f && newColor.g < 0.0f && newColor.b < 0.0f)
	{
		return;
	}

	this->m_color = newColor;
}
