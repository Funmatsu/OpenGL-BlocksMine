#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm\gtc/type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <cmath>
#include <vector>

class GL_Texture
{
public:
	GL_Texture();
	GL_Texture(const char* fileLocation);
	void loadTexture();
	void useTexture();
	void useNextTexture();
	void clearTexture();
	~GL_Texture();
private:
	unsigned int textureId;
	int width, height, bitDepth;
	const char* fileLocation;
};