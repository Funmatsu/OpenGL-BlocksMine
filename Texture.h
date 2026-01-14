#pragma once
#include <GL/glew.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm\gtc/type_ptr.hpp>
#include <vector>

using namespace std;

class GL_Texture
{
public:
	GL_Texture();
	GL_Texture(const char* fileLocation);
	void loadTexture();
	void useTexture();
	bool pixelOpaque(int x, int y);
	void useTexture(GLenum tex);
	void useNextTexture();
	void unbindNextTexture();
	void clearTexture();
	int getWidth() { return width; }
	int getHeight() { return height; }
	~GL_Texture();
	vector<unsigned char> pixels;
private:
	const char* fileLocation;
	unsigned int textureId;
	int width, height, bitDepth;	
};