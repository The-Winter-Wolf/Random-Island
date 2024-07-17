#version 420 core

out vec4 outputColor;

in VS_OUT 
{
	vec4 fColor;
	vec4 fNormal;
	vec4 fPosition;	
	vec2 fTexCoords;
	vec4 fPosLight;
} fs_in;

uniform vec4 eyeLocation;
uniform sampler2D shadowMap;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;
uniform sampler2D texture_opacity1;

////////////////////////////////// Light Init ///////////////////////////////////////

struct sLight
{
	vec4 position;			
	vec4 diffuse;	// color of the light (used for diffuse)
	vec4 specular;	// rgb = highlight color, w = power
	vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	vec4 direction;	// spot, directional lights
	vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
	                // 0 = pointlight
					// 1 = spot light
					// 2 = directional light
	vec4 param2;	// x = 0 for off, 1 for on
};

const int POINT_LIGHT_TYPE = 0;
const int SPOT_LIGHT_TYPE = 1;
const int DIRECTIONAL_LIGHT_TYPE = 2;

const int NUMBER_OF_LIGHTS = 20;

layout (std140) uniform LightDataNUB
{
	sLight theLights[NUMBER_OF_LIGHTS];
} lightNUB;

vec4 calculateLightContrib( vec3 vertexMaterialColor, vec3 vertexNormal, 
                            vec3 vertexWorldPos, vec4 vertexSpecular );

float CalculateShadow(vec4 fragPosLight);

////////////////////////////////////// Main //////////////////////////////////////////

void main()
{
	vec4 vertexRGBA = fs_in.fColor;

	vertexRGBA *= texture( texture_diffuse1, fs_in.fTexCoords * 100.0f );

	vec4 vertexSpecular = vec4(1.0f, 1.0f, 1.0f, 4.0f);

	vec4 ambient = 0.15 * vec4(1.0f);

	float shadow = CalculateShadow(fs_in.fPosLight);

	vec4 vertexColorLit = calculateLightContrib( vertexRGBA.rgb, fs_in.fNormal.xyz, 
												 fs_in.fPosition.xyz, vertexSpecular );

	outputColor.rgb = ambient.rgb * vertexRGBA.rgb  + (1.0f - shadow) * vertexColorLit.rgb;
//  outputColor.rgb = (1.0f - shadow) * vec3(1.0f, 0.0f, 0.0f);
//	outputColor.rgb = vertexColorLit.rgb;
	outputColor.a = 1.0f;
	return;
}

///////////////////////////// Light calculate function ////////////////////////////////

float CalculateShadow(vec4 fragPosLight)
{
	vec3  projCoords = fragPosLight.xyz / fragPosLight.w; 
	projCoords = projCoords * 0.5 + 0.5; // transform to [0,1] range
	float closestDepth = texture(shadowMap, projCoords.xy).r; 
	float currentDepth = projCoords.z;
	float shadow = currentDepth > closestDepth ? 1.0f : 0.0f;
	return shadow;
}

vec4 calculateLightContrib( vec3 vertexMaterialColor, vec3 vertexNormal, 
                            vec3 vertexWorldPos, vec4 vertexSpecular )
{
	vec3 norm = normalize(vertexNormal);
	
	vec4 finalObjectColor = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	
	for ( int index = 0; index < NUMBER_OF_LIGHTS; index++ )
	{	
		// ********************************************************
		// is light "on"
		if ( lightNUB.theLights[index].param2.x == 0.0f )
		{	// it's off
			continue;
		}
		
		// Cast to an int (note with c'tor)
		int intLightType = int(lightNUB.theLights[index].param1.x);
		
		// We will do the directional light here... 
		// (BEFORE the attenuation, since sunlight has no attenuation, really)
		if ( intLightType == DIRECTIONAL_LIGHT_TYPE )		// = 2
		{
			// This is supposed to simulate sunlight. 
			// SO: 
			// -- There's ONLY direction, no position
			// -- Almost always, there's only 1 of these in a scene
			// Cheapest light to calculate. 

			vec3 lightContrib = lightNUB.theLights[index].diffuse.rgb;
			
			// Get the dot product of the light and normalize
			float dotProduct = dot( -lightNUB.theLights[index].direction.xyz,  
									   normalize(norm.xyz) );	// -1 to 1

			dotProduct = max( 0.0f, dotProduct );		// 0 to 1
		
			lightContrib *= dotProduct;		
			
			finalObjectColor.rgb += ( vertexMaterialColor.rgb * lightNUB.theLights[index].diffuse.rgb * lightContrib ); 
									 //+ (materialSpecular.rgb * lightSpecularContrib.rgb );
			// NOTE: There isn't any attenuation, like with sunlight.
			// (This is part of the reason directional lights are fast to calculate)


			return finalObjectColor;		
		}
		
		// Assume it's a point light 
		// intLightType = 0
		
		// Contribution for this light
		vec3 vLightToVertex = lightNUB.theLights[index].position.xyz - vertexWorldPos.xyz;
		float distanceToLight = length(vLightToVertex);	
		vec3 lightVector = normalize(vLightToVertex);
		float dotProduct = dot(lightVector, vertexNormal.xyz);	 
		
		dotProduct = max( 0.0f, dotProduct );	
		
		vec3 lightDiffuseContrib = dotProduct * lightNUB.theLights[index].diffuse.rgb;
			

		// Specular 
		vec3 lightSpecularContrib = vec3(0.0f);
			
		vec3 reflectVector = reflect( -lightVector, normalize(norm.xyz) );

		// Get eye or view vector
		// The location of the vertex in the world to your eye
		vec3 eyeVector = normalize(eyeLocation.xyz - vertexWorldPos.xyz);

		// To simplify, we are NOT using the light specular value, just the object’s.
		float objectSpecularPower = vertexSpecular.w; 
		
//		lightSpecularContrib = pow( max(0.0f, dot( eyeVector, reflectVector) ), objectSpecularPower )
//			                   * vertexSpecular.rgb;	//* lightNUB.theLights[lightIndex].Specular.rgb
		lightSpecularContrib = pow( max(0.0f, dot( eyeVector, reflectVector) ), objectSpecularPower )
			                   * lightNUB.theLights[index].specular.rgb;
							   
		// Attenuation
		float attenuation = 1.0f / 
				( lightNUB.theLights[index].atten.x + 										
				  lightNUB.theLights[index].atten.y * distanceToLight +						
				  lightNUB.theLights[index].atten.z * distanceToLight*distanceToLight );  	
				  
		// total light contribution is Diffuse + Specular
		lightDiffuseContrib *= attenuation;
		lightSpecularContrib *= attenuation;
		
		
		// But is it a spot light
		if ( intLightType == SPOT_LIGHT_TYPE )		// = 1
		{	
		

			// Yes, it's a spotlight
			// Calcualate light vector (light to vertex, in world)
			vec3 vertexToLight = vertexWorldPos.xyz - lightNUB.theLights[index].position.xyz;

			vertexToLight = normalize(vertexToLight);

			float currentLightRayAngle
					= dot( vertexToLight.xyz, lightNUB.theLights[index].direction.xyz );
					
			currentLightRayAngle = max(0.0f, currentLightRayAngle);

			//vec4 param1;	
			// x = lightType, y = inner angle, z = outer angle, w = TBD

			// Is this inside the cone? 
			float outerConeAngleCos = cos(radians(lightNUB.theLights[index].param1.z));
			float innerConeAngleCos = cos(radians(lightNUB.theLights[index].param1.y));
							
			// Is it completely outside of the spot?
			if ( currentLightRayAngle < outerConeAngleCos )
			{
				// Nope. so it's in the dark
				lightDiffuseContrib = vec3(0.0f, 0.0f, 0.0f);
				lightSpecularContrib = vec3(0.0f, 0.0f, 0.0f);
			}
			else if ( currentLightRayAngle < innerConeAngleCos )
			{
				// Angle is between the inner and outer cone
				// (this is called the penumbra of the spot light, by the way)
				// 
				// This blends the brightness from full brightness, near the inner cone
				//	to black, near the outter cone
				float penumbraRatio = (currentLightRayAngle - outerConeAngleCos) / 
									  (innerConeAngleCos - outerConeAngleCos);
									  
				lightDiffuseContrib *= penumbraRatio;
				lightSpecularContrib *= penumbraRatio;
			}
						
		}// if ( intLightType == 1 )
		
		
					
		finalObjectColor.rgb += (vertexMaterialColor.rgb * lightDiffuseContrib.rgb)
								 + (vertexSpecular.rgb  * lightSpecularContrib.rgb );

	}//for(intindex=0...
	
	finalObjectColor.a = 1.0f;
	
	return finalObjectColor;
}