#include "cShaderManager.h"

#include "../../global/global.h"

#include <fstream>
#include <sstream>	
#include <iostream>

#include <algorithm>		
#include <iterator>	

cShaderManager* cShaderManager::m_pShaderManager = NULL;
cShaderManager* cShaderManager::GetShaderManager()
{
	if (cShaderManager::m_pShaderManager == NULL)
	{
		cShaderManager::m_pShaderManager = new cShaderManager();
	}
	return cShaderManager::m_pShaderManager;
}

cShaderManager::cShaderManager()
{
	this->indexOfUniformBlock = 0;
}
cShaderManager::~cShaderManager(){}

std::string cShaderManager::GetLastError(void)
{
	std::string lastErrorTemp = this->m_lastError;
	this->m_lastError = "";
	return lastErrorTemp;
}	

void cShaderManager::SetBasePath(const std::string& basepath)
{
	this->m_basepath = basepath;
	return;
}

bool cShaderManager::CreateProgramFromFile(const std::string& friendlyName, 
										   const std::string& vertShader, 
										   const std::string& fragShader,
										   const std::string& geomShader)
{
	std::string errorText = "";

	// Vertex shader
	std::string vertexCode = m_ReadShaderFile(vertShader);
	const char* vShaderCode = vertexCode.c_str();

	unsigned int vertex;	
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	if (! m_CheckCompileError(vertex, errorText))
	{
		std::stringstream ssError;
		ssError << "Vertex shader [" << vertShader << "] compile error:\n";
		ssError << errorText;
		this->m_lastError = ssError.str();
		return false;
	}

	// Fragment Shader
	std::string fragmentCode = m_ReadShaderFile(fragShader);
	const char* fShaderCode = fragmentCode.c_str();

	unsigned int fragment;
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	if (! m_CheckCompileError(fragment, errorText))
	{
		std::stringstream ssError;
		ssError << "Fragment shader [" << fragShader << "] compile error:\n";
		ssError << errorText;
		this->m_lastError = ssError.str();
		return false;
	}

	// Geometry Shader
	unsigned int geometry;
	if (geomShader != "")
	{
		std::string geometryCode = m_ReadShaderFile(geomShader);
		const char* gShaderCode = geometryCode.c_str();

		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		if (!m_CheckCompileError(geometry, errorText))
		{
			std::stringstream ssError;
			ssError << "Geometry shader [" << geomShader << "] compile error:\n";
			ssError << errorText;
			this->m_lastError = ssError.str();
			return false;
		}
	}

	// Shader Program
	unsigned int id = glCreateProgram();
	glAttachShader(id, vertex);
	glAttachShader(id, fragment);

	if (geomShader != "")
	{
		glAttachShader(id, geometry);
	}

	glLinkProgram(id);
	if (! m_CheckLinkError(id, errorText))
	{
		std::stringstream ssError;
		ssError << "Shader program link error: ";
		ssError << errorText;
		this->m_lastError = ssError.str();
		return false;
	}

	// Delete the shaders
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	if (geomShader != "")
	{
		glDeleteShader(geometry);
	}

	sShader shader;

	shader.id = id;
	shader.vertex = vertShader;
	shader.fragment = fragShader;
	shader.geometry = geomShader;
	shader.directory = m_basepath;
	shader.friendlyName = friendlyName;

	m_mapID_to_Shader[id] = shader;
	m_mapShaderName_to_ID[friendlyName] = id;

	return true;
}

bool cShaderManager::UseShader(unsigned int ID)
{
	if (glIsProgram(ID))
	{
		g_ShaderID = ID;
		glUseProgram(ID);
		return true;
	}

	return false;
}
bool cShaderManager::UseShader(const std::string& friendlyName)
{
	std::map< std::string, unsigned int >
	   ::iterator itShader = this->m_mapShaderName_to_ID.find(friendlyName);

	if (itShader == this->m_mapShaderName_to_ID.end() )
	{	
		return false;
	}

	g_ShaderID = itShader->second;
	glUseProgram(itShader->second);

	return true;
}

unsigned int cShaderManager::GetProgramID(const std::string& friendlyName)
{
	std::map< std::string, unsigned int >
	   ::iterator itShader = this->m_mapShaderName_to_ID.find(friendlyName);

	if (itShader == m_mapShaderName_to_ID.end() )
	{	
		return 0;
	}

	return itShader->second;
}
unsigned int cShaderManager::GetUniformID(unsigned int shaderID, const std::string& uniformName)
{
	std::map< unsigned int, sShader >::iterator itShader = this->m_mapID_to_Shader.find(shaderID);
	if (itShader == this->m_mapID_to_Shader.end())
	{
		return -2;
	}

	std::map< std::string, unsigned int >::iterator itUniform = itShader->second.m_mapUniform_ID.find(uniformName);
	if (itUniform == itShader->second.m_mapUniform_ID.end())
	{
		return -2;
	}

	return itUniform->second;
}

bool cShaderManager::InitActiveUniforms(void)
{
	std::map<std::string, unsigned int>::iterator itShader = m_mapShaderName_to_ID.begin();

	for (itShader; itShader != m_mapShaderName_to_ID.end(); itShader++)
	{
		int shaderID = itShader->second;

		GLint numOfActiveUniforms = 0;
		GLint uniformBufferLength = 0;

		glGetProgramiv(shaderID, GL_ACTIVE_UNIFORMS, &numOfActiveUniforms);
		glGetProgramiv(shaderID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformBufferLength);

		g_pConsole->cMessage("Active uniforms in shader " + itShader->first);

		// String buffer to hold the uniform name
		char* uniformNameBuffer = new char[uniformBufferLength];

		for (unsigned int uniformIndex = 0; uniformIndex != numOfActiveUniforms; uniformIndex++)
		{
			GLenum uniformTypeEnum = 0;
			GLsizei uniformNameStringLength = 0;
			GLint uniformVariableNumberBytes = 0;

			glGetActiveUniform(shaderID,						// shader ID ('name')
								uniformIndex,					// index from 0 to GL_ACTIVE_UNIFORMS returned
								uniformBufferLength,			// size of string buffer
								&uniformNameStringLength,		// actual uniform name string length
								&uniformVariableNumberBytes,	// size (an array or not)
								&uniformTypeEnum,				// uniform type (float, vec2, etc.)
								uniformNameBuffer);

			std::string uniformName(uniformNameBuffer);

			// Save the uniform location
			this->InitUniform(shaderID, uniformName);

			std::string uniformType = this->m_GetStringFromGLenum(uniformTypeEnum);

			g_pConsole->cMessage("location: " + std::to_string(uniformIndex) + "  " + uniformName +
								" size: " + std::to_string(uniformVariableNumberBytes) +
								" type: " + uniformType );
		}		
	}

	return true;
}
bool cShaderManager::InitUniform(unsigned int shaderID, const std::string& uniformName)
{
	GLint uniLocation = glGetUniformLocation(shaderID, uniformName.c_str());
	if (uniLocation == GL_INVALID_INDEX)
	{
		return false;
	}

	std::map< unsigned int, sShader >::iterator itShader = this->m_mapID_to_Shader.find(shaderID);
	if (itShader == this->m_mapID_to_Shader.end())
	{
		return false;
	}

	std::map< std::string, unsigned int >::iterator itUniform = itShader->second.m_mapUniform_ID.find(uniformName);
	if (itUniform == itShader->second.m_mapUniform_ID.end())
	{
		itShader->second.m_mapUniform_ID[uniformName] = uniLocation;
		return true;
	}

	return false;
}
bool cShaderManager::InitUniformBlock(const std::string& uniformName, unsigned int sizeOfbuffer)
{
	sUniformBlock uBlock;
	uBlock.sizeOfbuffer = sizeOfbuffer;
	uBlock.bindingPoint = indexOfUniformBlock;

	glGenBuffers(1, &(uBlock.bufferID));
	glBindBuffer(GL_UNIFORM_BUFFER, uBlock.bufferID);

	glBufferData(GL_UNIFORM_BUFFER,		// type of buffer
				sizeOfbuffer,			// number of bytes in size
				NULL,					// if NOT null, then copy the data
				GL_DYNAMIC_DRAW);		// hint for how this will be used

	glBindBuffer(GL_UNIFORM_BUFFER, 0);	// 0 releases any binding

	std::map<std::string, unsigned int>::iterator itShader = m_mapShaderName_to_ID.begin();

	for (itShader; itShader != m_mapShaderName_to_ID.end(); itShader++)
	{
		int shaderID = itShader->second;
		GLint blockIndex = glGetUniformBlockIndex(shaderID, uniformName.c_str());
		if (blockIndex != GL_INVALID_INDEX)
		{
			// Bind the buffer to the binding point
			glUniformBlockBinding(shaderID, blockIndex, uBlock.bindingPoint);
		}
	}

	// Tie the buffer to the same binding point
	glBindBufferRange(GL_UNIFORM_BUFFER,	// Type of buffer
					uBlock.bindingPoint,	// Binding point
					uBlock.bufferID,		// Buffer ID ('name')
					0,						// Offset (in bytes)
					uBlock.sizeOfbuffer);

	m_mapUniformBlocks[uniformName] = uBlock;

	indexOfUniformBlock++;

	g_pConsole->cMessage("Init " + uniformName + " named uniform block was successfull");

	return true;
}

bool cShaderManager::UpdateUniformBlock(const std::string& uniformName, void* updateData)
{
	std::map<std::string, sUniformBlock>::iterator itUniBlock = m_mapUniformBlocks.find(uniformName);
	if (itUniBlock == m_mapUniformBlocks.end())
	{
		return false;
	}

	unsigned int bufferID = itUniBlock->second.bufferID;
	unsigned int bufferSize = itUniBlock->second.sizeOfbuffer;

	glBindBuffer(GL_UNIFORM_BUFFER, bufferID);

	glBufferSubData(GL_UNIFORM_BUFFER, 0, bufferSize, updateData);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string cShaderManager::m_ReadShaderFile(const std::string& fileName)
{
	// Retrieve the shader source code from filePath
	std::string shaderCode;
	std::ifstream shaderFile;

	// Ensure ifstream object can throw exceptions:
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		// Open file
		shaderFile.open(m_basepath + "/" + fileName);

		// Read file's buffer content into stream
		std::stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();

		// Close file handler
		shaderFile.close();

		// Convert stream into string
		shaderCode = shaderStream.str();
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
	}
	
	return shaderCode;
}

bool cShaderManager::m_CheckCompileError(unsigned int shaderID, std::string& errorText)
{
	errorText = "";	

	GLint isCompiled = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

		char* pLogText = new char[maxLength];
	
		glGetShaderInfoLog(shaderID, maxLength, &maxLength, pLogText);
	
		errorText.append(pLogText);

		this->m_lastError.append("\n");
		this->m_lastError.append( errorText );

		delete [] pLogText;

		return false;	
	}
	return true;
}
bool cShaderManager::m_CheckLinkError(unsigned int programID, std::string& errorText)
{
	errorText = "";	

	GLint wasError = 0;
	glGetProgramiv(programID, GL_LINK_STATUS, &wasError);
	if(wasError == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);

		char* pLogText = new char[maxLength];
		
		glGetProgramInfoLog(programID, maxLength, &maxLength, pLogText);
	
		errorText.append(pLogText);

		this->m_lastError.append("\n");
		this->m_lastError.append( errorText );

		delete [] pLogText;	

		return false;	
	}
	
	return true; 
}

std::string cShaderManager::m_GetStringFromGLenum(unsigned int typeEnum)
{
	switch (typeEnum) 
	{
		case GL_FLOAT: return "float"; break;
		case GL_FLOAT_VEC2: return "vec2"; break;
		case GL_FLOAT_VEC3: return "vec3"; break;
		case GL_FLOAT_VEC4: return "vec4"; break;
		case GL_DOUBLE: return "double"; break;
		case GL_DOUBLE_VEC2: return "dvec2"; break;
		case GL_DOUBLE_VEC3: return "dvec3"; break;
		case GL_DOUBLE_VEC4: return "dvec4"; break;
		case GL_INT: return "int"; break;
		case GL_INT_VEC2: return "ivec2"; break;
		case GL_INT_VEC3: return "ivec3"; break;
		case GL_INT_VEC4: return "ivec4"; break;
		case GL_UNSIGNED_INT: return "unsigned"; break;
		case GL_UNSIGNED_INT_VEC2: return "uvec2"; break;
		case GL_UNSIGNED_INT_VEC3: return "uvec3"; break;
		case GL_UNSIGNED_INT_VEC4: return "uvec4"; break;
		case GL_BOOL: return "bool"; break;
		case GL_BOOL_VEC2: return "bvec2"; break;
		case GL_BOOL_VEC3: return "bvec3"; break;
		case GL_BOOL_VEC4: return "bvec4"; break;
		case GL_FLOAT_MAT2: return "mat2"; break;
		case GL_FLOAT_MAT3: return "mat3"; break;
		case GL_FLOAT_MAT4: return "mat4"; break;
		case GL_FLOAT_MAT2x3: return "mat2x3"; break;
		case GL_FLOAT_MAT2x4: return "mat2x4"; break;
		case GL_FLOAT_MAT3x2: return "mat3x2"; break;
		case GL_FLOAT_MAT3x4: return "mat3x4"; break;
		case GL_FLOAT_MAT4x2: return "mat4x2"; break;
		case GL_FLOAT_MAT4x3: return "mat4x3"; break;
		case GL_DOUBLE_MAT2: return "dmat2"; break;
		case GL_DOUBLE_MAT3: return "dmat3"; break;
		case GL_DOUBLE_MAT4: return "dmat4"; break;
		case GL_DOUBLE_MAT2x3: return "dmat2x3"; break;
		case GL_DOUBLE_MAT2x4: return "dmat2x4"; break;
		case GL_DOUBLE_MAT3x2: return "dmat3x2"; break;
		case GL_DOUBLE_MAT3x4: return "dmat3x4"; break;
		case GL_DOUBLE_MAT4x2: return "dmat4x2"; break;
		case GL_DOUBLE_MAT4x3: return "dmat4x3"; break;
		case GL_SAMPLER_1D: return "sampler1D"; break;
		case GL_SAMPLER_2D: return "sampler2D"; break;
		case GL_SAMPLER_3D: return "sampler3D"; break;
		case GL_SAMPLER_CUBE: return "samplerCube"; break;
		case GL_SAMPLER_1D_SHADOW: return "sampler1DShadow"; break;
		case GL_SAMPLER_2D_SHADOW: return "sampler2DShadow"; break;
		case GL_SAMPLER_1D_ARRAY: return "sampler1DArray"; break;
		case GL_SAMPLER_2D_ARRAY: return "sampler2DArray"; break;
		case GL_SAMPLER_1D_ARRAY_SHADOW: return "sampler1DArrayShadow"; break;
		case GL_SAMPLER_2D_ARRAY_SHADOW: return "sampler2DArrayShadow"; break;
		case GL_SAMPLER_2D_MULTISAMPLE: return "sampler2DMS"; break;
		case GL_SAMPLER_2D_MULTISAMPLE_ARRAY: return "sampler2DMSArray"; break;
		case GL_SAMPLER_CUBE_SHADOW: return "samplerCubeShadow"; break;
		case GL_SAMPLER_BUFFER: return "samplerBuffer"; break;
		case GL_SAMPLER_2D_RECT: return "sampler2DRect"; break;
		case GL_SAMPLER_2D_RECT_SHADOW: return "sampler2DRectShadow"; break;
		case GL_INT_SAMPLER_1D: return "isampler1D"; break;
		case GL_INT_SAMPLER_2D: return "isampler2D"; break;
		case GL_INT_SAMPLER_3D: return "isampler3D"; break;
		case GL_INT_SAMPLER_CUBE: return "isamplerCube"; break;
		case GL_INT_SAMPLER_1D_ARRAY: return "isampler1DArray"; break;
		case GL_INT_SAMPLER_2D_ARRAY: return "isampler2DArray"; break;
		case GL_INT_SAMPLER_2D_MULTISAMPLE: return "isampler2DMS"; break;
		case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return "isampler2DMSArray"; break;
		case GL_INT_SAMPLER_BUFFER: return "isamplerBuffer"; break;
		case GL_INT_SAMPLER_2D_RECT: return "isampler2DRect"; break;
		case GL_UNSIGNED_INT_SAMPLER_1D: return "usampler1D"; break;
		case GL_UNSIGNED_INT_SAMPLER_2D: return "usampler2D"; break;
		case GL_UNSIGNED_INT_SAMPLER_3D: return "usampler3D"; break;
		case GL_UNSIGNED_INT_SAMPLER_CUBE: return "usamplerCube"; break;
		case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY: return "usampler2DArray"; break;
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY: return "usampler2DArray"; break;
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE: return "usampler2DMS"; break;
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return "usampler2DMSArray"; break;
		case GL_UNSIGNED_INT_SAMPLER_BUFFER: return "usamplerBuffer"; break;
		case GL_UNSIGNED_INT_SAMPLER_2D_RECT: return "usampler2DRect"; break;
		case GL_IMAGE_1D: return "image1D"; break;
		case GL_IMAGE_2D: return "image2D"; break;
		case GL_IMAGE_3D: return "image3D"; break;
		case GL_IMAGE_2D_RECT: return "image2DRect"; break;
		case GL_IMAGE_CUBE: return "imageCube"; break;
		case GL_IMAGE_BUFFER: return "imageBuffer"; break;
		case GL_IMAGE_1D_ARRAY: return "image1DArray"; break;
		case GL_IMAGE_2D_ARRAY: return "image2DArray"; break;
		case GL_IMAGE_2D_MULTISAMPLE: return "image2DMS"; break;
		case GL_IMAGE_2D_MULTISAMPLE_ARRAY: return "image2DMSArray"; break;
		case GL_INT_IMAGE_1D: return "iimage1D"; break;
		case GL_INT_IMAGE_2D: return "iimage2D"; break;
		case GL_INT_IMAGE_3D: return "iimage3D"; break;
		case GL_INT_IMAGE_2D_RECT: return "iimage2DRect"; break;
		case GL_INT_IMAGE_CUBE: return "iimageCube"; break;
		case GL_INT_IMAGE_BUFFER: return "iimageBuffer"; break;
		case GL_INT_IMAGE_1D_ARRAY: return "iimage1DArray"; break;
		case GL_INT_IMAGE_2D_ARRAY: return "iimage2DArray"; break;
		case GL_INT_IMAGE_2D_MULTISAMPLE: return "iimage2DMS"; break;
		case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY: return "iimage2DMSArray"; break;
		case GL_UNSIGNED_INT_IMAGE_1D: return "uimage1D"; break;
		case GL_UNSIGNED_INT_IMAGE_2D: return "uimage2D"; break;
		case GL_UNSIGNED_INT_IMAGE_3D: return "uimage3D"; break;
		case GL_UNSIGNED_INT_IMAGE_2D_RECT: return "uimage2DRect"; break;
		case GL_UNSIGNED_INT_IMAGE_CUBE: return "uimageCube"; break;
		case GL_UNSIGNED_INT_IMAGE_BUFFER: return "uimageBuffer"; break;
		case GL_UNSIGNED_INT_IMAGE_1D_ARRAY: return "uimage1DArray"; break;
		case GL_UNSIGNED_INT_IMAGE_2D_ARRAY: return "uimage2DArray"; break;
		case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE: return "uimage2DMS"; break;
		case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY: return "uimage2DMSArray"; break;
		case GL_UNSIGNED_INT_ATOMIC_COUNTER: return "atomic_uint"; break;
	}

	return "UNKNOWN";
}

////////////////////////////////////////// UNIFORM FUNCTIONS ///////////////////////////////////////////////////
bool cShaderManager::SetUniform(int ID, const std::string& name, bool value)
{
	unsigned int uniformID = this->GetUniformID(ID, name);
	if (uniformID != -2)
	{
		glUniform1f(uniformID, static_cast<float>(value));
		return true;
	}

	if ( ! this->InitUniform(ID, name))
	{
		return false;
	}

	glUniform1f(glGetUniformLocation(ID, name.c_str()), static_cast<float>(value));
	return true;
}
// ------------------------------------------------------------------------
bool cShaderManager::SetUniform(int ID, const std::string& name, int value)
{
	unsigned int uniformID = this->GetUniformID(ID, name);
	if (uniformID != -2)
	{
		glUniform1f(uniformID, static_cast<float>(value));
		return true;
	}

	if (!this->InitUniform(ID, name))
	{
		return false;
	}

	glUniform1f(glGetUniformLocation(ID, name.c_str()), static_cast<float>(value));
	return true;
}
// ------------------------------------------------------------------------
bool cShaderManager::SetUniform(int ID, const std::string& name, float value)
{
	unsigned int uniformID = this->GetUniformID(ID, name);
	if (uniformID != -2)
	{
		glUniform1f(uniformID, value);
		return true;
	}

	if (!this->InitUniform(ID, name))
	{
		return false;
	}

	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	return true;
}
// ------------------------------------------------------------------------
bool cShaderManager::SetUniform(int ID, const std::string& name, const glm::vec2& value)
{
	unsigned int uniformID = this->GetUniformID(ID, name);
	if (uniformID != -2)
	{
		glUniform2fv(uniformID, 1, &value[0]);
		return true;
	}

	if (!this->InitUniform(ID, name))
	{
		return false;
	}

	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	return true;
}
bool cShaderManager::SetUniform(int ID, const std::string& name, float x, float y)
{
	unsigned int uniformID = this->GetUniformID(ID, name);
	if (uniformID != -2)
	{
		glUniform2f(uniformID, x, y);
		return true;
	}

	if (!this->InitUniform(ID, name))
	{
		return false;
	}

	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	return true;
}
// ------------------------------------------------------------------------
bool cShaderManager::SetUniform(int ID, const std::string& name, const glm::vec3& value)
{
	unsigned int uniformID = this->GetUniformID(ID, name);
	if (uniformID != -2)
	{
		glUniform3fv(uniformID, 1, &value[0]);
		return true;
	}

	if (!this->InitUniform(ID, name))
	{
		return false;
	}

	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	return true;
}
bool cShaderManager::SetUniform(int ID, const std::string& name, float x, float y, float z)
{
	unsigned int uniformID = this->GetUniformID(ID, name);
	if (uniformID != -2)
	{
		glUniform3f(uniformID, x, y, z);
		return true;
	}

	if (!this->InitUniform(ID, name))
	{
		return false;
	}

	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	return true;
}
// ------------------------------------------------------------------------
bool cShaderManager::SetUniform(int ID, const std::string& name, const glm::vec4& value)
{
	unsigned int uniformID = this->GetUniformID(ID, name);
	if (uniformID != -2)
	{
		glUniform4fv(uniformID, 1, &value[0]);
		return true;
	}

	if (!this->InitUniform(ID, name))
	{
		return false;
	}

	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	return true;
}
bool cShaderManager::SetUniform(int ID, const std::string& name, float x, float y, float z, float w)
{
	unsigned int uniformID = this->GetUniformID(ID, name);
	if (uniformID != -2)
	{
		glUniform4f(uniformID, x, y, z, w);
		return true;
	}

	if (!this->InitUniform(ID, name))
	{
		return false;
	}

	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	return true;
}
// ------------------------------------------------------------------------
bool cShaderManager::SetUniform(int ID, const std::string& name, const glm::mat2& mat)
{
	unsigned int uniformID = this->GetUniformID(ID, name);
	if (uniformID != -2)
	{
		glUniformMatrix2fv(uniformID, 1, GL_FALSE, &mat[0][0]);
		return true;
	}

	if (!this->InitUniform(ID, name))
	{
		return false;
	}

	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	return true;
}
// ------------------------------------------------------------------------
bool cShaderManager::SetUniform(int ID, const std::string& name, const glm::mat3& mat)
{
	unsigned int uniformID = this->GetUniformID(ID, name);
	if (uniformID != -2)
	{
		glUniformMatrix3fv(uniformID, 1, GL_FALSE, &mat[0][0]);
		return true;
	}

	if (!this->InitUniform(ID, name))
	{
		return false;
	}

	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	return true;
}
// ------------------------------------------------------------------------
bool cShaderManager::SetUniform(int ID, const std::string& name, const glm::mat4& mat)
{
	unsigned int uniformID = this->GetUniformID(ID, name);
	if (uniformID != -2)
	{
		glUniformMatrix4fv(uniformID, 1, GL_FALSE, &mat[0][0]);
		return true;
	}

	if (!this->InitUniform(ID, name))
	{
		return false;
	}

	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

