#pragma once

#include "../../global/common.h"

#include <glm/glm.hpp>
#include <string>
#include <map>

class cShaderManager
{
public:
	~cShaderManager();
	static cShaderManager* GetShaderManager();

	bool UseShader(unsigned int shaderID);
	bool UseShader(const std::string& friendlyName);

	void SetBasePath(const std::string& basepath);

	bool CreateProgramFromFile(const std::string& friendlyName, 
							   const std::string& vertShader, 
							   const std::string& fragShader,
							   const std::string& geomShader = "");

	unsigned int GetProgramID(const std::string& friendlyName );
	unsigned int GetUniformID(unsigned int shaderID, const std::string& uniformName);

	bool InitActiveUniforms(void);
	bool InitUniform(unsigned int shaderID, const std::string& uniformName);
	bool InitUniformBlock(const std::string& uniformName, unsigned int sizeOfbuffer);
	bool UpdateUniformBlock(const std::string& uniformName, void* updateData);

	std::string GetLastError(void);

	bool SetUniform(int ID, const std::string& name, bool value);
	bool SetUniform(int ID, const std::string& name, int value);
	bool SetUniform(int ID, const std::string& name, float value);
	bool SetUniform(int ID, const std::string& name, const glm::vec2& value);
	bool SetUniform(int ID, const std::string& name, float x, float y);
	bool SetUniform(int ID, const std::string& name, const glm::vec3& value);
	bool SetUniform(int ID, const std::string& name, float x, float y, float z);
	bool SetUniform(int ID, const std::string& name, const glm::vec4& value);
	bool SetUniform(int ID, const std::string& name, float x, float y, float z, float w);
	bool SetUniform(int ID, const std::string& name, const glm::mat2& mat);
	bool SetUniform(int ID, const std::string& name, const glm::mat3& mat);
	bool SetUniform(int ID, const std::string& name, const glm::mat4& mat);

private:
	cShaderManager();
	static cShaderManager* m_pShaderManager;

	struct sShader
	{
		unsigned int id = -2;

		std::string vertex;
		std::string geometry;
		std::string fragment;
		std::string directory;
		std::string friendlyName;

		std::map<std::string /*name*/, unsigned int /*uniformID*/> m_mapUniform_ID;
	};

	struct sUniformBlock
	{
		std::string blockName;

		unsigned int bufferID = -2;
		unsigned int sizeOfbuffer = -2;
		unsigned int bindingPoint = -2;
	};

	std::string m_basepath;
	std::string m_lastError;

	std::string m_ReadShaderFile(const std::string& fileName);
	std::string m_GetStringFromGLenum(unsigned int typeEnum);
	bool m_CheckLinkError(unsigned int progID, std::string& errorText);
	bool m_CheckCompileError(unsigned int shaderID, std::string& errorText);

	std::map<unsigned int /*ID*/, sShader> m_mapID_to_Shader;
	std::map<std::string /*name*/, unsigned int /*ID*/> m_mapShaderName_to_ID;

	unsigned int indexOfUniformBlock;
	std::map<std::string /*name*/, sUniformBlock /*uniform_block*/> m_mapUniformBlocks;
};