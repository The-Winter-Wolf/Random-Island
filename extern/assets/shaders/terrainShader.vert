#version 420 core
layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 vNormal;
layout (location = 2) in vec2 vTexCoords;
layout (location = 3) in vec4 vColor;

out VS_OUT 
{
	vec4 fColor;
	vec4 fNormal;
	vec4 fPosition;	
	vec2 fTexCoords;
	vec4 fPosLight;
} vs_out;
	
uniform mat4 matView;
uniform mat4 matModel;
uniform mat4 matModel_IT;
uniform mat4 matProjection;
uniform mat4 lightMatViewProj;

void main()
{
    vs_out.fColor = vColor;
    vs_out.fTexCoords = vTexCoords.st;

    vs_out.fPosition = matModel * vec4( vPosition.xyz, 1.0f );
	vs_out.fPosLight = lightMatViewProj * vec4( vPosition.xyz, 1.0f );

    vs_out.fNormal = matModel_IT * vec4( vNormal.xyz, 1.0f );
	vs_out.fNormal.xyz = normalize( vs_out.fNormal.xyz );
	vs_out.fNormal.w = 1.0f;

    gl_Position = matProjection * matView * matModel * vPosition;
}