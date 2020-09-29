#include "Game.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include "BallObject.h"
// to debug TODO delete
#include <iostream>
#include <cmath>

SpriteRenderer* Renderer;
GameObject* Player;

// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Radius of the ball object
const float BALL_RADIUS = 12.5f;

BallObject* Ball;

bool CheckCollision(const GameObject& one, const GameObject& two) //AABB - AABB collision 
{
	bool collisonX = one.Position.x + one.Size.x >= two.Position.x && two.Position.x + two.Size.x >= one.Position.x;
	bool collisonY = one.Position.y + one.Size.y >= two.Position.y && two.Position.y + two.Size.y >= one.Position.y;

	return collisonX && collisonY;
}

bool CheckCollision(const BallObject& one, const GameObject two)
{
	glm::vec2 center(one.Position + one.Radius);
	glm::vec2 aabbHalfExtents(two.Size.x / 2.0f, two.Size.y / 2.0f);
	glm::vec2 aabbCenter(
		two.Position.x + aabbHalfExtents.x,
		two.Position.y + aabbHalfExtents.y
	);
	glm::vec2 difference = center - aabbCenter;
	glm::vec2 clamped = glm::clamp(difference, -aabbHalfExtents, aabbHalfExtents);

	glm::vec2 closest = aabbCenter + clamped;
	difference = closest - center;

	return glm::length(difference) < one.Radius;
}

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

	// initialize the ball variables
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS,
		-BALL_RADIUS * 2.0f);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
		ResourceManager::GetTexture("face"));
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
			if(Ball->Stuck)
				Ball->Position.x -= velocity;
		}
		if (this->Keys[GLFW_KEY_D])
		{
			if (Player->Position.x <= this->Width - Player->Size.x)
				Player->Position.x += velocity;
			if (Ball->Stuck)
				Ball->Position.x += velocity;
		}
		if (this->Keys[GLFW_KEY_SPACE])
		{
			Ball->Stuck = false;
			// debug
			//std::cout << Ball->Velocity.x << " " << Ball->Velocity.y << "\n";
		}
	}
}
void Game::Update(float dt)
{
	Ball->move(dt, this->Width);
	this->DoCollisions();

	if (Ball->Position.y >= this->Height) // if the bottom reached the bottom edge or got out
	{
		this->ResetLevel();
		this->ResetPlayer();

	}
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
		Ball->Draw(*Renderer);
	}
}

void Game::DoCollisions()
{
	for (GameObject& box : this->Levels[this->Level].Bricks)
		if (!box.Destroyed)
		{
			Collision collision = checkCollision(*Ball, box);
			if (std::get<0>(collision)) // if there's a collision
			{
				// destroy the block if it wasn't solid
				if (!box.IsSolid)
					box.Destroyed = true;
				Direction dir = std::get<1>(collision);
				glm::vec2 diffVector = std::get<2>(collision);

				if (dir == LEFT || dir == RIGHT) // horizontal collision
				{
					Ball->Velocity.x *= -1;
					float penetration = Ball->Radius - std::abs(diffVector.x);
					if (dir == LEFT)
						Ball->Position.x += penetration; // move the ball to the right
					else
						Ball->Position.x -= penetration; // or move it to the left
				}
				else // vertical collision
				{
					Ball->Velocity.y *= -1;
					float penetration = Ball->Radius - std::abs(diffVector.y);
					if (dir == LEFT)
						Ball->Position.y -= penetration; // move the ball up
					else
						Ball->Position.y += penetration; // or move it down
				}
			}
		}
		
	Collision ballPlayerCollision = checkCollision(*Ball, *Player);
	if (!Ball->Stuck && std::get<0>(ballPlayerCollision))
	{

		float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
		float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
		float percentage = distance / (Player->Size.x / 2.0f);

		// move according to calculations
		float strength = 2.0f;
		glm::vec2 oldVelocity = Ball->Velocity;
		Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);
		Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
	}
		
		
}

void Game::ResetLevel()
{
	if (this->Level == 0)
		this->Levels[0].Load("levels/one.lvl", this->Width, this->Height / 2);
	else if (this->Level == 1)
		this->Levels[1].Load("levels/two.lvl", this->Width, this->Height / 2);
	else if (this->Level == 2)
		this->Levels[2].Load("levels/three.lvl", this->Width, this->Height / 2);
	else if (this->Level == 3)
		this->Levels[3].Load("levels/four.lvl", this->Width, this->Height / 2);
}

void Game::ResetPlayer()
{
	// reset player/ball stats
	Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
	Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
}

Direction VectorDirection(glm::vec2 target)
{
	glm::vec2 compass[4];
	compass[0] = glm::vec2(0.f, 1.f);
	compass[1] = glm::vec2(1.f, 0.f);
	compass[2] = glm::vec2(0.f, -1.f);
	compass[3] = glm::vec2(-1.f, 0.f);

	// mx = max 
	float mx = 0.f;
	int bestMatch = -1;

	for (int i = 0; i < 4; i++)
	{
		float dotPro = glm::dot(glm::normalize(target), compass[i]);
		if (dotPro > mx)
		{
			mx = dotPro;
			bestMatch = i;
		}
	}

	return Direction(bestMatch);
}

Collision checkCollision(const BallObject& one, const GameObject& two)
{
	glm::vec2 center(one.Position + one.Radius);
	glm::vec2 aabbHalfExtents(two.Size.x / 2.0f, two.Size.y / 2.0f);
	glm::vec2 aabbCenter(
		two.Position.x + aabbHalfExtents.x,
		two.Position.y + aabbHalfExtents.y
	);
	glm::vec2 difference = center - aabbCenter;
	glm::vec2 clamped = glm::clamp(difference, -aabbHalfExtents, aabbHalfExtents);

	glm::vec2 closest = aabbCenter + clamped;
	difference = closest - center;

	if (glm::length(difference) <= one.Radius)
		return std::make_tuple(true, VectorDirection(difference), difference);
	else
		return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}
