#pragma once

#include<glad/glad.h>
#include<GLFW/glfw3.h>


enum GameState
{
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

// Holds all game state and functionality realted things

class Game
{
public:
	GameState State;
	bool Keys[1024];
	unsigned int Width, Height;

	Game(unsigned int width, unsigned int height);
	~Game();
	//initialize game state (loading all shaders/textures/levels)
	void Init();

	//game loop
	void ProcessInput(float dt);
	void Update(float dt);
	void Render();
};

