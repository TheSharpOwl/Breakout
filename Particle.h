#pragma once

#include<glad/glad.h>
#include<glm/glm.hpp>

struct Particle
{
	glm::vec2 Position, Velocity;
	glm::vec4 Color;
	float Life;

	Particle() :
		Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) {}
};

