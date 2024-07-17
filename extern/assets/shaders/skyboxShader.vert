#version 420
layout (location = 0) in vec3 aPos;

out vec3 fTexCoords;

uniform mat4 matView;
uniform mat4 matProjection;

void main()
{
    fTexCoords = aPos;
    vec4 pos = matProjection * matView * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
} 