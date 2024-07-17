#pragma once

#include <string>

class iCommands
{
public:
	virtual ~iCommands() {};

	virtual bool Update(float deltaTime) = 0;

	virtual bool Stop() = 0;

	virtual std::string GetCommandName() = 0;
	virtual void SetCommandName(std::string commandName) = 0;
};