#version 420

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 vNormal;
layout (location = 2) in vec2 vTexCoords;
layout (location = 3) in vec4 vColor;
layout (location = 4) in ivec4 vBoneIds;
layout (location = 5) in vec4 vWeights;
layout (location = 6) in mat4 vMatModel;

out VS_OUT 
{
	vec4 gColor;
	vec4 gNormal;
	vec4 gPosition;	
	vec2 gTexCoords;
	vec4 gBoneIds;
	vec4 gPosLight;
} vs_out;

uniform mat4 matView;
uniform mat4 matModel_IT;
uniform mat4 matProjection;
uniform mat4 lightMatViewProj;

uniform vec2 UVOffset;
uniform float heightScale;
uniform bool bUseHeightMap;
uniform sampler2D texture_height1;

uniform bool bHasAnimation;
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

uniform mat4 finalBonesMatrices[MAX_BONES];

void main()
{
	vec4 vertexModelPosition = vPosition;
	vec4 vertexNormal = vNormal;

	vec4 finalPosition = vec4(0.0f);
	vec4 finalNormal = vec4(0.0f);

	vec2 UVFinal = vTexCoords.st;
	
	if ( bUseHeightMap )
	{
		// Greyscale (black and white) heightmap image
		UVFinal += UVOffset;
		float height = texture( texture_height1, UVFinal.st ).r;
		vertexModelPosition.y += ( height * heightScale );
	}

	if ( bHasAnimation )
	{	
		for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
		{
			if (vBoneIds[i] == -1) break;

			if (vBoneIds[i] >= MAX_BONES) break;

			vec4 localPosition = finalBonesMatrices[vBoneIds[i]] * vPosition;
			finalPosition += localPosition * vWeights[i];

			vec4 localNormal = finalBonesMatrices[vBoneIds[i]] * vNormal;
			finalNormal += localNormal * vWeights[i];		
		}

		vertexModelPosition = finalPosition;
		vertexNormal = finalNormal;
	}

	vs_out.gPosition = vMatModel * vec4( vertexModelPosition.xyz, 1.0f );
	vs_out.gPosLight = lightMatViewProj * vec4( vertexModelPosition.xyz, 1.0f );

	//vs_out.gNormal = matModel_IT * vec4( vertexNormal.xyz, 1.0f );
	vs_out.gNormal = inverse(transpose(vMatModel)) * vec4( vertexNormal.xyz, 1.0f );
	vs_out.gNormal.xyz = normalize( vs_out.gNormal.xyz );
	vs_out.gNormal.w = 1.0f;

	vs_out.gColor = vColor;

    vs_out.gTexCoords = vTexCoords.st; 	
	vs_out.gBoneIds = vBoneIds;

	gl_Position = matProjection * matView * vMatModel * vec4( vertexModelPosition.xyz, 1.0f );
}