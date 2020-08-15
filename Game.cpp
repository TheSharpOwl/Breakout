#include "Game.h"

Game::Game(unsigned int width, unsigned int height)
	:State(GAME_ACTIVE), Keys(),Width(width), Height(height)
{

}
Game::~Game()
{

}

//initialize game state (loading all shaders/textures/levels)
void Game::Init()
{

}

//game loop
void Game::ProcessInput(float dt)
{

}
void Game::Update(float dt)
{

}
void Game::Render()
{

}