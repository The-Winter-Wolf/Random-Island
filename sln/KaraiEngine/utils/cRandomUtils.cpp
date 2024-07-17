#include "cRandomUtils.h"

#include <time.h>

cRandomUtils::cRandomUtils()
{
	srand(time(0));
}

cRandomUtils::~cRandomUtils(){}

float cRandomUtils::GetRandomFloat()
{
	float random = static_cast<float>( rand() );
	return random;
}

float cRandomUtils::GetRandomFloat(float min, float max)
{
	if (max < min)
	{
		float temp = max;
		max = min;
		min = temp;
	}

	float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	float diff = max - min;
	float r = random * diff;
	return min + r;
}

int cRandomUtils::GetRandomInt()
{
	int random = rand();
	return random;
}

int cRandomUtils::GetRandomInt(int min, int max)
{
	if (max < min)
	{
		int temp = max;
		max = min;
		min = temp;
	}
	float rands = rand();
	float random = rands / RAND_MAX;
	int diff = max - min;
	int r = static_cast<int>(random * diff);
	return min + r;
}

glm::vec3 cRandomUtils::GetRandomVec3()
{
	glm::vec3 vector = glm::vec3(0.0f);
	vector.x = GetRandomFloat();
	vector.y = GetRandomFloat();
	vector.z = GetRandomFloat();
	return vector;
}

glm::vec3 cRandomUtils::GetRandomVec3(glm::vec3 min, glm::vec3 max)
{
	glm::vec3 vector = glm::vec3(0.0f);
	vector.x = GetRandomFloat(min.x, max.x);
	vector.y = GetRandomFloat(min.y, max.y);
	vector.z = GetRandomFloat(min.z, max.z);
	return vector;
}

glm::vec4 cRandomUtils::GetRandomVec4()
{
	glm::vec4 vector = glm::vec4(0.0f);
	vector.r = GetRandomFloat();
	vector.g = GetRandomFloat();
	vector.b = GetRandomFloat();
	vector.a = GetRandomFloat();
	return vector;
}

glm::vec4 cRandomUtils::GetRandomVec4(glm::vec4 min, glm::vec4 max)
{
	glm::vec4 vector = glm::vec4(0.0f);
	vector.r = GetRandomFloat(min.r, max.r);
	vector.g = GetRandomFloat(min.g, max.g);
	vector.b = GetRandomFloat(min.b, max.b);
	vector.a = GetRandomFloat(min.a, max.a);
	return vector;
}
