#version 420
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT 
{
	vec4 gColor;
	vec4 gNormal;
	vec4 gPosition;	
	vec2 gTexCoords;
	vec4 gBoneIds;
	vec4 gPosLight;
} gs_in[];

out GS_OUT
{
	vec4 fColor;
	vec4 fNormal;
	vec4 fPosition;	
	vec2 fTexCoords;
	vec4 fBoneIds;
	vec4 fPosLight;
} gs_out;	

uniform float time;
uniform bool bGenNewNormals;
uniform bool bExplodeObject;

int maxVertices = 3;

vec3 GetNormal();
vec4 ExplodeObject(vec4 position, vec3 normal);

void main() 
{	
	vec3 newNormal = GetNormal();

	for (int i = 0; i < maxVertices; i++)
	{	
		if ( bExplodeObject )
		{
			gl_Position = ExplodeObject(gs_in[i].gPosition, newNormal);
		}
		else
		{
			gl_Position = gl_in[i].gl_Position;
		}

		if ( bGenNewNormals )
		{
			gs_out.fNormal = vec4(newNormal, 1.0f);
		}
		else
		{
			gs_out.fNormal = gs_in[i].gNormal;
		}

		gs_out.fColor = gs_in[i].gColor;
		gs_out.fPosition = gs_in[i].gPosition;	
		gs_out.fTexCoords = gs_in[i].gTexCoords;
		gs_out.fBoneIds = gs_in[i].gBoneIds;
		gs_out.fPosLight = gs_in[i].gPosLight;
		EmitVertex();
	}

	EndPrimitive();
}

vec3 GetNormal()
{
   vec3 triEdgeA = gs_in[1].gPosition.xyz - gs_in[0].gPosition.xyz;
   vec3 triEdgeB = gs_in[2].gPosition.xyz - gs_in[0].gPosition.xyz;
   return normalize(cross(triEdgeB, triEdgeA));
}

vec4 ExplodeObject(vec4 position, vec3 normal)
{
	float length = 1.0f;
	vec3 direction = normal * ((sin(time) + 1.0f) / 2.0f) * length;
	return position + vec4(direction, 0.0f);
}