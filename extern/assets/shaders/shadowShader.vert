#version 420 core
layout (location = 0) in vec4 vPosition;
layout (location = 6) in mat4 vMatModel;
	
uniform mat4 lightMatrix;

void main()
{
    gl_Position = lightMatrix * vMatModel * vPosition;
}