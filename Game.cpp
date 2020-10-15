#include "Game.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include "BallObject.h"
#include "ParticleGenerator.hpp"
#include  "PostProcessor.hpp"
#include "PowerUp.hpp"
#include <iostream>

SpriteRenderer* Renderer;
GameObject* Player;
ParticleGenerator* Particles;
BallObject* Ball;
PostProcessor* Effects;

// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Radius of the ball object
const float BALL_RADIUS = 12.5f;

float ShakeTime = 0.f;

bool CheckCollision(const GameObject& one, const GameObject& two);
bool CheckCollision(const BallObject& one, const GameObject two);
bool isOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type);

Game::Game(unsigned int width, unsigned int height)
	:State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{

}
Game::~Game()
{
	delete Renderer;
	delete Player;
	delete Ball;
	delete Particles;
	delete Effects;
}

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



//initialize game state (loading all shaders/textures/levels)
void Game::Init()
{
	//load shaders
	ResourceManager::LoadShader("Shaders/SpriteVer.glsl", "Shaders/SpriteFrag.glsl", nullptr, "sprite");
	ResourceManager::LoadShader("Shaders/ParticleVer.glsl", "Shaders/ParticleFrag.glsl", nullptr, "particle");
	ResourceManager::LoadShader("Shaders/PostProcessingVert.glsl", "Shaders/PostProcessingFrag.glsl", nullptr, "postprocessing");
	//configure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
		static_cast<float>(this->Height), 0.0f, -1.f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
	ResourceManager::GetShader("particle").SetMatrix4("projection", projection);
	//load textures
	ResourceManager::LoadTexture("resources/textures/background.jpg", false, "background");
	ResourceManager::LoadTexture("resources/textures/awesomeface.png", true, "face");
	ResourceManager::LoadTexture("resources/textures/block.png", false, "block");
	ResourceManager::LoadTexture("resources/textures/block_solid.png", false, "block_solid");
	ResourceManager::LoadTexture("resources/textures/paddle.png", true, "paddle");
	ResourceManager::LoadTexture("resources/textures/particle.png", true, "particle");
	ResourceManager::LoadTexture("resources/textures/powerup_speed.png", true, "powerup_speed");
	ResourceManager::LoadTexture("resources/textures/powerup_sticky.png", true, "powerup_sticky");
	ResourceManager::LoadTexture("resources/textures/powerup_increase.png", true, "powerup_increase");
	ResourceManager::LoadTexture("resources/textures/powerup_confuse.png", true, "powerup_confuse");
	ResourceManager::LoadTexture("resources/textures/powerup_chaos.png", true, "powerup_chaos");
	ResourceManager::LoadTexture("resources/textures/powerup_passthrough.png", true, "powerup_passthrough");
	// set render-specific controls
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 500);
	Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), this->Width, this->Height);
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
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
}
void ActivatePowerUp(PowerUp& powerUp)
{
	if (powerUp.Type == "speed")
	{
		Ball->Velocity *= 1.2;
	}
	else if (powerUp.Type == "sticky")
	{
		Ball->Sticky = true;
		Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
	}
	else if (powerUp.Type == "pass-through")
	{
		Ball->PassThrough = true;
		Ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
	}
	else if (powerUp.Type == "pad-size-increase")
	{
		Player->Size.x += 50;
	}
	else if (powerUp.Type == "confuse")
	{
		if (!Effects->Chaos)
			Effects->Confuse = true; // only activate if chaos wasn't already active
	}
	else if (powerUp.Type == "chaos")
	{
		if (!Effects->Confuse)
			Effects->Chaos = true;
	}
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
		}
	}
}
void Game::Update(float dt)
{
	Ball->move(dt, this->Width);
	this->DoCollisions();
	Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2.0f));
	this->UpdatePowerUps(dt);
	if (ShakeTime > 0.0f)
	{
		ShakeTime -= dt;
		if (ShakeTime <= 0.0f)
			Effects->Shake = false;
	}
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
		Effects->BeginRender();

		// draw the background
		Renderer->DrawSprite(ResourceManager::GetTexture("background"),
			glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
		// draw the level
		this->Levels[this->Level].Draw(*Renderer);
		// draw the player
		Player->Draw(*Renderer);

		for (PowerUp& powerUp : this->PowerUps)
			if (!powerUp.Destroyed)
				powerUp.Draw(*Renderer);

		Particles->Draw();
		Ball->Draw(*Renderer);

		Effects->EndRender();
		Effects->Render(glfwGetTime());
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
				{
					box.Destroyed = true;
					this->SpawnPowerUps(box);
				}
				else
				{
					ShakeTime = 0.05f;
					Effects->Shake = true;
				}
				Direction dir = std::get<1>(collision);
				glm::vec2 diffVector = std::get<2>(collision);
				if (!(Ball->PassThrough && !box.IsSolid)) // don't do collision resolution on non-solid bricks if pass-through is activated
				{
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
		}

	// also check collisions on PowerUps and if so, activate them
	for (PowerUp& powerUp : this->PowerUps)
	{
		if (!powerUp.Destroyed)
		{
			// first check if powerup passed bottom edge, if so: keep as inactive and destroy
			if (powerUp.Position.y >= this->Height)
				powerUp.Destroyed = true;

			if (CheckCollision(*Player, powerUp))
			{	// collided with player, now activate powerup
				ActivatePowerUp(powerUp);
				powerUp.Destroyed = true;
				powerUp.Activated = true;
			}
		}
	}

	// check collisions for player pad
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
	for (PowerUp& powerUp : this->PowerUps)
		if (!powerUp.Destroyed)
		{
			if (powerUp.Position.y >= this->Height)
				powerUp.Destroyed = true;
			if (CheckCollision(*Player, powerUp))
			{
				// player got the power up
				ActivatePowerUp(powerUp);
				powerUp.Destroyed = true;
				powerUp.Activated = true;
			}
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

	// reset all the power ups also
	Effects->Chaos = Effects->Confuse = false;
	Ball->PassThrough = Ball->Sticky = false;
	Player->Color = glm::vec3(1.0f);
	Ball->Color = glm::vec3(1.0f);
}

bool ShouldSpawn(unsigned int chance)
{
	unsigned int random = rand() % chance;
	return random == 0;
}

void Game::SpawnPowerUps(GameObject& block)
{
	if (ShouldSpawn(75)) // 1 in 75 chance
		this->PowerUps.push_back(PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position, ResourceManager::GetTexture("powerup_speed")));
	if (ShouldSpawn(75))
		this->PowerUps.push_back(PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position, ResourceManager::GetTexture("powerup_sticky")));
	if (ShouldSpawn(75))
		this->PowerUps.push_back(PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position, ResourceManager::GetTexture("powerup_passthrough")));
	if (ShouldSpawn(75))
		this->PowerUps.push_back(PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.Position, ResourceManager::GetTexture("powerup_increase")));
	if (ShouldSpawn(15)) // Negative powerups should spawn more often
		this->PowerUps.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_confuse")));
	if (ShouldSpawn(15))
		this->PowerUps.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_chaos")));
}

bool isOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type)
{
	for (const PowerUp& powerUp : powerUps)
	{
		if (powerUp.Activated)
			if (powerUp.Type == type)
				return true;
	}
	return false;
}

void Game::UpdatePowerUps(float dt)
{
	for (PowerUp& powerUp : this->PowerUps)
	{
		powerUp.Position += powerUp.Velocity * dt;
		if (powerUp.Activated)
		{
			powerUp.Duration -= dt;

			if (powerUp.Duration <= 0.0f)
			{
				// remove powerup from list (will later be removed)
				powerUp.Activated = false;
				// deactivate effects
				if (powerUp.Type == "sticky")
				{
					if (!isOtherPowerUpActive(this->PowerUps, "sticky"))
					{	// only reset if no other PowerUp of type sticky is active
						Ball->Sticky = false;
						Player->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "pass-through")
				{
					if (!isOtherPowerUpActive(this->PowerUps, "pass-through"))
					{	// only reset if no other PowerUp of type pass-through is active
						Ball->PassThrough = false;
						Ball->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "confuse")
				{
					if (!isOtherPowerUpActive(this->PowerUps, "confuse"))
					{	// only reset if no other PowerUp of type confuse is active
						Effects->Confuse = false;
					}
				}
				else if (powerUp.Type == "chaos")
				{
					if (!isOtherPowerUpActive(this->PowerUps, "chaos"))
					{	// only reset if no other PowerUp of type chaos is active
						Effects->Chaos = false;
					}
				}
			}
		}
	}
	// "remove if" puts all the things we wanna remove at the end and returns an iterator to the begining of that group

	this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
		[](const PowerUp& powerUp) { return powerUp.Destroyed && !powerUp.Activated; }
	), this->PowerUps.end());
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
