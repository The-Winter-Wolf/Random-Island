#pragma once
#include <string>
#include <vector>

class cShader
{
public:
	cShader();
	~cShader();

	enum eShaderType
	{
		VERTEX_SHADER,
		GEOMETRY_SHADER,
		FRAGMENT_SHADER,
		UNKNOWN
	};

	eShaderType shaderType;
	std::string getShaderTypeString(void);

	unsigned int ID;
	std::vector<std::string> vecSource;
	bool bSourceIsMultiLine;
	std::string fileName;
};

