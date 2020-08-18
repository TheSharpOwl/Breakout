#include "Game.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"

SpriteRenderer* Renderer;

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
	//load shaders
	ResourceManager::LoadShader("Shaders/SpriteFrag.glsl", "Shaders/SpriteVertex.glsl", nullptr, "sprite");
	//configure 
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
		static_cast<float>(this->Height), 0.0f, -1.f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	// set render-specific controls
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	//load textures
	ResourceManager::LoadTexture("textures/awsomeface.png", true, "face");
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
	Renderer->DrawSprite(ResourceManager::GetTexture("face"),
		glm::vec2(200.0f, 200.0f), glm::vec2(300.f, 400.f), 45.f, glm::vec3(0.0f, 1.0f, 0.0f));
}