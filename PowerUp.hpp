/* 
*	Possible PowerUps :
*
*	1.Speed: increases the velocity of the ball by 20%.
*	2.Sticky: when the ball collides with the paddle, the ball remains stuck to the paddle unless the spacebar is pressed again.
		This allows the player to better position the ball before releasing it.
*	3.Pass-Through: collision resolution is disabled for non-solid blocks, allowing the ball to pass through multiple blocks.
*	4.Pad-Size-Increase: increases the width of the paddle by 50 pixels.
*	5.Confuse: activates the confuse postprocessing effect for a short period of time, confusing the user.
*	6.Chaos: activates the chaos postprocessing effect for a short period of time, heavily disorienting the user.
*
*/
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "GameObject.h"

// The size of a PowerUp block
const glm::vec2 POWERUP_SIZE(60.0f, 20.0f);
// Velocity a PowerUp block has when spawned
const glm::vec2 VELOCITY(0.0f, 150.0f);

class PowerUp : public GameObject
{
public:

	std::string Type;
	float Duration;
	bool Activated;

	PowerUp(std::string type, glm::vec3 color, float duration,
		glm::vec2 position, Texture2D texture)
		: GameObject(position, POWERUP_SIZE, texture, color, VELOCITY),
		Type(type), Duration(duration), Activated()
	{ }
};

