#pragma once
#include<map>
#include<string>

#include<glad/glad.h>

#include "Texture.h"
#include "Shader.h"

class ResourceManager
{
public:
	//resource storage
	static std::map<std::string, Shader> Shaders;
	static std::map<std::string, Texture2D> Textures;
	//load (and generate) a shader program from a file (and skips Geometry shader if it was nullptr)
	static Shader LoadShader(const char* cShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name);
	//retrieves a stored texture
	static Shader GetShader(std::string name);
	//loads (and generates a texture file)
	static Texture2D LoadTexture(const char* file, bool alpha, std::string name);
	// retrieves a stored texture
	static Texture2D GetTexture(std::string name);
	// deallocate all the loaded resources
	static void Clear();

private:
	ResourceManager() {};
	static Shader    loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile = nullptr);
	// loads a single texture from file
	static Texture2D loadTextureFromFile(const char* file, bool alpha);
};

