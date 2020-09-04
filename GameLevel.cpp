#include "GameLevel.h"

#include <iostream>
#include <fstream>
#include <sstream>

void GameLevel::Load(const char* file, unsigned int levelWidth, unsigned int levelHeight)
{
	// clear the old data
	this->Bricks.clear();

	unsigned int tileCode;
	//GameLevel level;

	std::string line;
	std::ifstream fstream(file);
	std::vector <std::vector<unsigned int>> tileData;
	if (fstream)
	{
		while (std::getline(fstream, line))
		{
			std::istringstream sstream(line);
			std::vector<unsigned int> row;
			while (sstream >> tileCode)
				row.push_back(tileCode);
			tileData.push_back(row);
		}
		if (tileData.size() > 0)
			this->init(tileData, levelWidth, levelHeight);
	}

}
void GameLevel::Draw(SpriteRenderer& renderer)
{
	for (GameObject& tile : this->Bricks)
		if (!tile.Destroyed)
			tile.Draw(renderer);
}
// true if no more bricks can be destroyed
bool GameLevel::IsCompleted()
{
	for (GameObject& tile : this->Bricks)
		if (!tile.IsSolid && !tile.Destroyed)
			return false;
	return true;
}
void GameLevel::init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight)
{
	unsigned int height = tileData.size();
	unsigned int width = tileData[0].size();
	float UnitWidth = levelWidth / static_cast<float>(width), unitHeight = levelHeight / height;

	//initialize the level with the tiles
	for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			if (tileData.size() < y || tileData[y].size() < x)
			{
				std::cout << "Problem !!!\n";
				exit(1);
			}
			if (tileData[y][x] == 1) // solid
			{
				glm::vec2 pos(UnitWidth * x, unitHeight * y);
				glm::vec2 size(UnitWidth, unitHeight);
				GameObject obj(pos, size, ResourceManager::GetTexture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
				obj.IsSolid = true;
				this->Bricks.push_back(obj);
			}
			else if (tileData[y][x] > 1) // non solid so determine its color
			{
				glm::vec3 color = glm::vec3(1.0f);
				switch (tileData[y][x])
				{
				case 2: color = glm::vec3(0.2f, 0.6f, 1.0f); break;
				case 3: color = glm::vec3(0.0f, 0.7f, 0.0f); break;
				case 4: color = glm::vec3(0.8f, 0.8f, 0.4f); break;
				case 5: color = glm::vec3(1.0f, 0.5f, 0.0f); break;
				}
				glm::vec2 pos(UnitWidth * x, unitHeight * y);
				glm::vec2 size(UnitWidth, unitHeight);
				this->Bricks.push_back(GameObject(pos, size, ResourceManager::GetTexture("block"), color));
			}

		}
	}

}