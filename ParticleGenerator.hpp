#pragma once
#include "Shader.h"
#include "Texture.h"
#include "GameObject.h"
#include<vector>
#include "Particle.h"

class ParticleGenerator
{
public:
	ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);

	void Update(float dt, GameObject& object, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));

	void Draw();

private:
	std::vector<Particle> particles;
	unsigned int amount;

	Shader shader;
	Texture2D texture;
	unsigned int VAO;

	void init();
	unsigned int firstUnusedParticle();
	void respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};

