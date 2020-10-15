#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "SpriteRenderer.h"
#include "shader.h"

class PostProcessor
{
public:

	Shader PostProcessingShader;
	Texture2D Texture;
	unsigned int Width, Height;
	bool Confuse, Chaos, Shake;
	PostProcessor(Shader shader, unsigned int width, unsigned int height);
	// prepares the postprocessor's framebuffer operations before rendering the game
	void BeginRender();
	// should be called after rendering the game, so it stores all the rendered data into a texture object
	void EndRender();
	// renders the PostProcessor texture quad (as a screen-encompassing large sprite)
	void Render(float time);
private:

	void initRenderData();

	// render state
	unsigned int MSFBO, FBO; // MSFBO = Multisampled FBO. FBO is regular, used for blitting MS color-buffer to texture
	unsigned int RBO; // RBO is used for multisampled color buffer
	unsigned int VAO;

};

