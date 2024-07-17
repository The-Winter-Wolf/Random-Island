#version 420

out vec4 FragColor;

in vec3 fTexCoords;

uniform bool bHasTexture01;
uniform vec4 skyboxColor01;
uniform samplerCube skybox01;

uniform bool bHasTexture02;
uniform vec4 skyboxColor02;
uniform samplerCube skybox02;

uniform bool bHasTexture03;
uniform vec4 skyboxColor03;
uniform samplerCube skybox03;

uniform bool bHasTexture04;
uniform vec4 skyboxColor04;
uniform samplerCube skybox04;

uniform float time;

uniform mat4 cloudMatModel;
uniform mat4 planetMatModel;

vec4 middayColor;
vec4 sunriseColor;
vec4 sunsetColor;
vec4 midnightColor;

void InitSkyColors(vec3 cloudRot, vec3 planetRot);

void main()
{   
    vec3 cloudRotation = vec3(cloudMatModel * vec4(fTexCoords, 1.0f));
    vec3 planetRotation = vec3(planetMatModel * vec4(fTexCoords, 1.0f));

    InitSkyColors(cloudRotation, planetRotation);

    FragColor = mix(middayColor, sunriseColor, clamp(time, 1.0f, 1.5f) - 1.0f);
    FragColor = mix(FragColor, sunsetColor, clamp(time, 1.5f, 2.0f) - 1.5f);
    FragColor = mix(FragColor, midnightColor, clamp(time, 2.0f, 3.0f) - 2.0f);
}

void InitSkyColors(vec3 cloudRot, vec3 planetRot)
{
    if (bHasTexture01)
    {
        middayColor = texture( skybox01, cloudRot);
    }
    else
    {
        middayColor = skyboxColor01;
    }

    if (bHasTexture02)
    {
        midnightColor = texture( skybox02, planetRot);
    }
    else
    {
        midnightColor = skyboxColor02;
    }

    if (bHasTexture03)
    {
        sunriseColor = texture( skybox03, cloudRot);
    }
    else
    {
        sunriseColor = skyboxColor03;
    }

    if (bHasTexture04)
    {
        sunsetColor = texture( skybox04, cloudRot);
    }
    else
    {
        sunsetColor = skyboxColor04;
    }
}


