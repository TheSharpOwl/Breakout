#include "Game.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"

SpriteRenderer* Renderer;
GameObject* Player;

Game::Game(unsigned int width, unsigned int height)
	:State(GAME_ACTIVE), Keys(),Width(width), Height(height)
{

}
Game::~Game()
{
	delete Renderer;
	delete Player;
}

//initialize game state (loading all shaders/textures/levels)
void Game::Init()
{
	//load shaders
	ResourceManager::LoadShader("Shaders/SpriteVer.glsl", "Shaders/SpriteFrag.glsl", nullptr, "sprite");
	//configure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
		static_cast<float>(this->Height), 0.0f, -1.f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	// set render-specific controls
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	//load textures
	ResourceManager::LoadTexture("resources/textures/background.jpg", false, "background");
	ResourceManager::LoadTexture("resources/textures/awesomeface.png", true, "face");
	ResourceManager::LoadTexture("resources/textures/block.png", false, "block");
	ResourceManager::LoadTexture("resources/textures/block_solid.png", false, "block_solid");
	ResourceManager::LoadTexture("resources/textures/paddle.png", true, "paddle");
	// load levels
	GameLevel one;
	one.Load("levels/one.lvl", this->Width, this->Height / 2);
	GameLevel two;
	two.Load("levels/two.lvl", this->Width, this->Height / 2);
	GameLevel three;
	three.Load("levels/three.lvl", this->Width, this->Height / 2);
	GameLevel four;
	four.Load("levels/four.lvl", this->Width, this->Height / 2);
	this->Levels.push_back(one);
	this->Levels.push_back(two);
	this->Levels.push_back(three);
	this->Levels.push_back(four);
	this->Level = 0;

	// configure the game objects
	glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
}

//game loop
void Game::ProcessInput(float dt)
{
	if (this->State == GAME_ACTIVE)
	{
		float velocity = PLAYER_VELOCITY * dt;
		// move the player's board
		if (this->Keys[GLFW_KEY_A])
		{
			if (Player->Position.x >= 0.0f)
				Player->Position.x -= velocity;
		}
		if (this->Keys[GLFW_KEY_D])
		{
			if (Player->Position.x <= this->Width - Player->Size.x)
				Player->Position.x += velocity;
		}
	}
}
void Game::Update(float dt)
{

}
void Game::Render()
{
	if (this->State == GAME_ACTIVE)
	{
		// draw the background
		Renderer->DrawSprite(ResourceManager::GetTexture("background"),
			glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
		// draw the level
		this->Levels[this->Level].Draw(*Renderer);
		// draw the player
		Player->Draw(*Renderer);
	}
}