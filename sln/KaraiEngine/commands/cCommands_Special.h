#pragma once

#include "iCommands.h"

#include "../objects/object_3D/cObject3D.h"

class cCommands_Special : public iCommands
{
public:
	cCommands_Special();
	~cCommands_Special();

	virtual bool Stop();

	virtual bool Update(float deltaTime);

	virtual std::string GetCommandName();

	virtual void SetCommandName(std::string commandName);

	// Visibility of object
	void Initialize(std::string objectName, bool visibility);

	// Copy existing object
	void Initialize(std::string objectName, std::string newFriendlyName, glm::vec3 posXYZ);

	// Set color
	void Initialize(std::string objectName, bool isColor, glm::vec4 newColor);

private:

	std::string m_commandName;

	std::string m_objectName;
	std::string m_friendlyName;

	bool m_visibility;

	glm::vec3 m_sbjXYZ;

	bool isColor;
	glm::vec4 m_color;

	cObject3D* m_pObject;
};
