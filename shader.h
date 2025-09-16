#pragma once

#include <GL/glew.h>
#include<string>
#include <sstream>
#include <fstream>
#include <iostream>

using namespace std;
class Shadergl
{
public:
	Shadergl();
	~Shadergl();
	void createShaderFromString(const char* vertexCode, const char* fragmentCode);
	void createShaderFromFiles(const char* vertexFilePath, const char* fragmentFilePath);
	unsigned int getProjectionLocation();
	unsigned int getModelLocation();
	unsigned int getViewLocation();
	unsigned int getShaderId() { return shaderId; }

	string readShaderFiles(const char* fileLocation);

	void useShader();
	void clearShader();

private:
	unsigned int shaderId, uniformModel, uniformProjection, uniformView;
	unsigned int compileShader(unsigned int type, const char* source);
	void addShader(const char* vertexCode, const char* fragmentCode);
};

