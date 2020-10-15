#pragma once

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include "Gamelevel.h"

// forward decleration
class BallObject;

enum GameState
{
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

enum Direction
{
	UP,
	RIGHT,
	DOWN,
	LEFT
};

using Collision = std::tuple<bool, Direction, glm::vec2>;


// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
// Initial velocity of the player paddle
const float PLAYER_VELOCITY(500.0f);

// To be defined in the cpp
bool CheckCollision(const GameObject& one, const GameObject& two);
bool CheckCollision(const BallObject& one, const GameObject& two);
Direction VectorDirection(glm::vec2 target);
Collision checkCollision(const BallObject& one, const GameObject& two);
// Holds all game state and functionality related things

class Game
{
public:
	GameState State;
	bool Keys[1024];
	unsigned int Width, Height;

	std::vector<GameLevel> Levels;
	unsigned int           Level;

	Game(unsigned int width, unsigned int height);
	~Game();

	//initialize game state (loading all shaders/textures/levels)
	void Init();

	//game loop
	void ProcessInput(float dt);
	void Update(float dt);
	void Render();

	void DoCollisions();

	void ResetLevel();
	void ResetPlayer();
};

