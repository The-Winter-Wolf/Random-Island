#version 420

out vec4 outputColor;

in GS_OUT
{
	vec4 fColor;
	vec4 fNormal;
	vec4 fPosition;	
	vec2 fTexCoords;
	vec4 fBoneIds;
	vec4 fPosLight;
} fs_in;

uniform vec4 eyeLocation;
uniform sampler2D shadowMap;

uniform bool bUseColor;
uniform vec4 colorRGBA;

uniform float alphaOpacity;

uniform bool bHasLight;
uniform bool bHasTexture;
uniform bool bUseBoneColor;

uniform bool bUseReflection;
uniform bool bUseRefraction;

uniform bool bWhiteOpacity;
uniform bool bBlackOpacity;
uniform bool bUseHeightMap;
uniform bool bUseEmissionMap;
uniform bool bUseSpecularMap;

uniform vec2 UVOffset;
uniform float texScale;
uniform float mixRatio;

// off-screen and 2nd pass effects
uniform vec2 screenSize;
uniform bool bIsStaticScreen;
uniform bool bIsAberration;
uniform bool bIsNightVision;
uniform bool bIsBlurScreenBasic;
uniform bool bIsBlurScreenSlower;
uniform bool bIsBlurScreenFaster;
uniform bool bIsOffScreenTexture;
uniform sampler2D textureOffScreen;

// textures
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;
uniform sampler2D texture_opacity1;
uniform sampler2D texture_emission1;

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

//////////////////////////// FUNCTIONS /////////////////////////////

vec4 CalculateLightContrib( vec3 vertexMaterialColor, vec3 vertexNormal, 
                            vec3 vertexWorldPos, vec4 vertexSpecular );

float CalculateShadow(vec4 fragPosLight);

// 2nd pass effects
vec3 GetFBOColor();
vec3 NightVision();
vec3 BlurScreenBasic();
vec3 BlurScreenSlower(int pixelOffset);
vec3 BlurScreenFaster(int pixelOffset);
vec3 ChromicAberration(float amount);

/////////////////////////////////////////////////////////////////////

vec4 vertexRGBA = fs_in.fColor;
vec2 texCoords = fs_in.fTexCoords;
vec4 boneColor = fs_in.fBoneIds;

void main()
{ 
	if ( bIsOffScreenTexture )
	{	
		int numEffects = 1;	
		vec3 theColor = vec3(0.0f);
		
		theColor = GetFBOColor();
			
		if (bIsNightVision)
		{
			theColor.rgb += NightVision();
			numEffects++;
		}
		if (bIsAberration)
		{
			theColor.rgb += ChromicAberration(0.5f);
			numEffects++;
		}
		if (bIsBlurScreenBasic)
		{
			theColor.rgb += BlurScreenBasic();
			numEffects++;
		}
		if (bIsBlurScreenSlower)
		{
			theColor.rgb += BlurScreenSlower(5);
			numEffects++;
		}
		if (bIsBlurScreenFaster)
		{
			theColor.rgb += BlurScreenFaster(5);
			numEffects++;
		}
						 							 
		outputColor.rgb = theColor / float(numEffects);
		outputColor.a = 1.0f;	
		return;
	}

	if ( bHasTexture )
	{	
		float discardValue = 0.5f;

		if (bWhiteOpacity || bBlackOpacity)
		{
			discardValue = texture( texture_opacity1, texCoords ).r;
		}
		
		if ( bWhiteOpacity )
		{
			// If "black" then discard
			if ( discardValue.r < 0.5f )
			{
				discard;
			}	
		}
			
		if ( bBlackOpacity )
		{
			// If "white" then discard
			if ( discardValue.r > 0.1f )
			{
				discard;
			}	
		}

		vertexRGBA = mix(texture( texture_diffuse1, (texCoords * texScale) + UVOffset), 
						 texture( texture_diffuse2, (texCoords * texScale) + UVOffset), mixRatio);

		//vertexRGBA = texture( texture_diffuse1, newCoords );
	}

	if ( bUseColor && !bHasTexture )
	{	
		if (bUseBoneColor)
		{
			vertexRGBA.rgb = boneColor.rgb;
		}
		else
		{
			vertexRGBA = colorRGBA;
		}	
	}

	if ( bUseColor && bHasTexture )
	{	
		if (bUseBoneColor)
		{
			vertexRGBA.rgb *= boneColor.rgb;
		}
		else
		{
			vertexRGBA *= colorRGBA;
		}
	}

	if ( ! bHasLight )
	{
		//outputColor = vec4(1.0f,0.0f,0.0f,1.0f);
		outputColor = vertexRGBA;
		outputColor.a = alphaOpacity;
		return;
	}

	vec4 ambient = 0.15 * vec4(1.0f);

	float shadow = CalculateShadow(fs_in.fPosLight);

	vec4 vertexSpecular = vec4(0.0f);
	if (bUseSpecularMap)
	{
		vertexSpecular = texture( texture_specular1, texCoords);
	}

	vertexSpecular.w = 4.0f;	// specular power

	vec4 vertexEmission = vec4(0.0f);
	if (bUseEmissionMap)
	{
		vertexEmission = texture( texture_emission1, texCoords);
	}

	vec4 vertexColorLit = CalculateLightContrib( vertexRGBA.rgb, fs_in.fNormal.xyz, 
												  fs_in.fPosition.xyz, vertexSpecular );

	outputColor.rgb = ambient.rgb * vertexRGBA.rgb  + (1.0f - shadow) * vertexColorLit.rgb + vertexEmission.rgb;
	outputColor.a = alphaOpacity;
	return;
}

//////////////////////////////////////////////////////////////////////////////////////

float CalculateShadow(vec4 fragPosLight)
{
	vec3  projCoords = fragPosLight.xyz / fragPosLight.w; 
	projCoords = projCoords * 0.5 + 0.5; // transform to [0,1] range
	float closestDepth = texture(shadowMap, projCoords.xy).r; 
	float currentDepth = projCoords.z;
	float shadow = currentDepth > closestDepth ? 1.0f : 0.0f;
	return shadow;
}

vec4 CalculateLightContrib( vec3 vertexMaterialColor, vec3 vertexNormal, 
                            vec3 vertexWorldPos, vec4 vertexSpecular )
{
	vec3 norm = normalize(vertexNormal);
	
	vec4 finalObjectColor = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	
	for ( int index = 0; index < NUMBER_OF_LIGHTS; index++ )
	{	
		// ********************************************************
		// is light "on" it's off
		if ( lightNUB.theLights[index].param2.x == 0.0f )
		{	
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

			vec3 lightDir = normalize(lightNUB.theLights[index].direction.xyz);
			// Get the dot product of the light and normalize
			float dotProduct = dot( -lightDir, norm.xyz );	// -1 to 1

			dotProduct = max( 0.0f, dotProduct );	// 0 to 1	

			vec3 lightDiffContrib = dotProduct * lightNUB.theLights[index].diffuse.rgb;
				
			//lightDiffuseContrib *= dotProduct;
			
			// Specular
			vec3 eyeVector = normalize(eyeLocation.xyz - vertexWorldPos.xyz);
			vec3 reflectVector = reflect( lightDir, norm.xyz );
			float objectSpecularPower = vertexSpecular.w; 
		
			vec3 lightSpecularContrib = pow( max(0.0f, dot( eyeVector, reflectVector) ), objectSpecularPower )
										* lightNUB.theLights[index].specular.rgb;
 
			finalObjectColor.rgb += ( vertexMaterialColor.rgb * lightDiffContrib )
								  + ( vertexSpecular.rgb * lightSpecularContrib.rgb );

			// NOTE: There isn't any attenuation, like with sunlight.
			// (This is part of the reason directional lights are fast to calculate)
			finalObjectColor.a = 1.0f;

			return finalObjectColor;		
		}
		 
		// Point light = 0
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
		
		// Spot light = 1 (spot light from point light)
		if ( intLightType == SPOT_LIGHT_TYPE )
		{	
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
		}
							
		finalObjectColor.rgb += (vertexMaterialColor.rgb * lightDiffuseContrib.rgb)
								 + (vertexSpecular.rgb  * lightSpecularContrib.rgb );
	}
	
	finalObjectColor.a = 1.0f;
	
	return finalObjectColor;
}

/////////////////////////////////////////////////////////////////////////////////////////

vec3 GetFBOColor()
{
	vec2 tCoords = vec2(0.0f);

	if (bIsStaticScreen)
	{
		tCoords = texCoords;
	}
	else
	{
		tCoords = vec2( gl_FragCoord.x / screenSize.x, gl_FragCoord.y / screenSize.y );
	}
	
	vec3 theColor = texture( textureOffScreen, tCoords ).rgb;		
	
	return theColor;
}

//////////////////////////////////////////////////////////////////////////////////////////

vec3 NightVision()
{
	vec3 theColor = GetFBOColor();
	float brightness = ( theColor.r + theColor.g + theColor.b ) / 3.0f;
	theColor = vec3(0.0f, brightness, 0.0f);
	
	return theColor;
}

//////////////////////////////////////////////////////////////////////////////////////////

vec3 ChromicAberration( float amount )
{
	vec3 theColor = vec3(0.0f);
	vec2 textCoords = vec2(0.0f);

	if (bIsStaticScreen)
	{
		textCoords = texCoords;
	}
	else
	{
		textCoords = vec2( gl_FragCoord.x / screenSize.x, gl_FragCoord.y / screenSize.y );
	}
					
	// Red coordinate is off up (-0.025 in y) and to the left (-0.01f in x)
	vec2 redYV = vec2( textCoords.x - (amount * 0.01f), textCoords.y - (amount * 0.025f) );					   
	theColor.r = texture( textureOffScreen, redYV ).r;	
	
	// Green coordinate is off up (+0.002 in y) and to the right (+0.01f in x)
	vec2 greenYV = vec2( textCoords.x + (amount * 0.01f), textCoords.y + (amount * 0.002f) );					   
	theColor.g = texture( textureOffScreen, greenYV ).g;	
	
	// Green coordinate is off the left (-0.015f in x)
	vec2 blueYV = vec2( textCoords.x - (amount * 0.015f), textCoords.y );					   
	theColor.b = texture( textureOffScreen, blueYV ).b;	
	
	return theColor;
}

//////////////////////////////////////////////////////////////////////////////////////////

vec3 BlurScreenBasic()
{
	//   *
	// * O *
	//   *
	
	vec2 textCoords = vec2(0.0f);
	if (bIsStaticScreen)
	{
		textCoords = texCoords;
	}
	else
	{
		textCoords = vec2( gl_FragCoord.x / screenSize.x, gl_FragCoord.y / screenSize.y );
	}					
	vec3 theColorPixel = texture( textureOffScreen, textCoords ).rgb;	
		
		
	// To the left (-ve in X)
	vec2 pixel0 = vec2(0.0f);
	if (bIsStaticScreen)
	{
		pixel0 = vec2( (texCoords.x - 0.01), texCoords.y);
	}
	else
	{
		pixel0 = vec2( (gl_FragCoord.x - 1) / screenSize.x, gl_FragCoord.y / screenSize.y );	
	}				
	vec3 pixel0Colour =  texture( textureOffScreen, pixel0 ).rgb;	


	// To the right (+ve in X)
	vec2 pixel1 = vec2(0.0f);
	if (bIsStaticScreen)
	{
		pixel1 = vec2( (texCoords.x + 0.01), texCoords.y);
	}
	else
	{
		pixel1 = vec2( (gl_FragCoord.x + 1) / screenSize.x, gl_FragCoord.y / screenSize.y );	
	}				
	vec3 pixel1Colour =  texture( textureOffScreen, pixel1 ).rgb;	
	

	// Above (-ve in y)
	vec2 pixel2 = vec2(0.0f);
	if (bIsStaticScreen)
	{
		pixel2 = vec2( texCoords.x, (texCoords.y - 0.01) );
	}
	else
	{
		pixel2 = vec2( gl_FragCoord.x / screenSize.x, (gl_FragCoord.y - 1) / screenSize.y );	
	}				
	vec3 pixel2Colour =  texture( textureOffScreen, pixel2 ).rgb;					
	

	// Below (+ve in y)
	vec2 pixel3 = vec2(0.0f);
	if (bIsStaticScreen)
	{
		pixel3 = vec2( texCoords.x, (texCoords.y + 0.01) );
	}
	else
	{
		pixel3 = vec2( gl_FragCoord.x / screenSize.x, (gl_FragCoord.y + 1) / screenSize.y );
	}
	vec3 pixel3Colour =  texture( textureOffScreen, pixel3 ).rgb;	
	

	// There are five (5) pixels so average by dividing by 5
	vec3 theColor = vec3(0.0f);
	theColor += ( theColorPixel + pixel0Colour + pixel1Colour + pixel2Colour + pixel3Colour ) / 5.0f;

	return theColor;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

vec3 BlurScreenSlower(int pixelOffset)
{
	// pixelOffset = 1   pixelOffset = 2
	// 3x3               5x5
	// * * *             * * * * * 
	// * O *             * * * * * 
	// * * *             * * O * * 
	//                   * * * * * 
	//                   * * * * * 
	
	int totalSamples = 0;	
	vec3 theColor = vec3(0.0f);	
	
	for ( int xOffset = -pixelOffset; xOffset <= pixelOffset; xOffset++ )
	{
		for ( int yOffset = -pixelOffset; yOffset <= pixelOffset; yOffset++ )
		{
			totalSamples++;	
			vec2 pixelUV = vec2(0.0f);

			if (bIsStaticScreen)
			{
				pixelUV = vec2( texCoords.x + (xOffset/100.0f), texCoords.y + (yOffset/100.0f) );
			}
			else
			{
				pixelUV = vec2( (gl_FragCoord.x + xOffset) / screenSize.x, (gl_FragCoord.y + yOffset) / screenSize.y );	
			}
			
			theColor += texture( textureOffScreen, pixelUV ).rgb;	
		}	
	}

	// Average this colour by the number of samples
	theColor.rgb /= float(totalSamples);

	return theColor;
}

////////////////////////////////////////////////////////////////////////////////////////////////

vec3 BlurScreenFaster(int pixelOffset)
{
	// pixelOffset = 1   pixelOffset = 2
	// 3x3               5x5
	//   *                   * 
	// * O *                 * 
	//   *               * * O * * 
	//                       *  
	//                       * 
	
	int totalSamples = 0;	
	vec3 theColor = vec3(0.0f);	
	
	// Add up a horizontal sample
	for ( int xOffset = -pixelOffset; xOffset <= pixelOffset; xOffset++ )
	{
		totalSamples++;	
		vec2 pixelUV = vec2(0.0f);

		if (bIsStaticScreen)
		{
			pixelUV = vec2( texCoords.x + (xOffset/100.0f), texCoords.y );
		}
		else
		{
			pixelUV = vec2( (gl_FragCoord.x + xOffset) / screenSize.x, gl_FragCoord.y / screenSize.y );	
		}
					
		theColor += texture( textureOffScreen, pixelUV ).rgb;	
	}
	
	// Add up a vertical sample
	for ( int yOffset = -pixelOffset; yOffset <= pixelOffset; yOffset++ )
	{
		totalSamples++;
		vec2 pixelUV = vec2(0.0f);

		if (bIsStaticScreen)
		{
			pixelUV = vec2( texCoords.x, texCoords.y + (yOffset/100.0f) );
		}
		else
		{
			pixelUV = vec2( gl_FragCoord.x / screenSize.x, (gl_FragCoord.y + yOffset) / screenSize.y );	
		}
			
		theColor += texture( textureOffScreen, pixelUV ).rgb;	
	}	

	// Average this colour by the number of samples
	theColor.rgb /= float(totalSamples);

	return theColor;
}