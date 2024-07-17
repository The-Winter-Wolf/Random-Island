// Fragment shader
#version 420

out vec4 outputColour;

in vec2 TexCoords;

uniform sampler2D depthMap;

void main()
{
    float depthValue = texture(depthMap, TexCoords).r;
	outputColour = vec4(vec3(depthValue), 1.0);
}