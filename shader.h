#pragma once

#include <GL/glew.h>
#include<string>
#include <sstream>
#include <fstream>
#include <iostream>
#include "DirectionalLight.h"
#include "PointLIght.h"

using namespace std;
class Shadergl
{
public:
	int pointLightCount;
	Shadergl();
	~Shadergl();
	void createShaderFromString(const char* vertexCode, const char* fragmentCode);
	void createShaderFromFiles(const char* vertexFilePath, const char* fragmentFilePath);
	unsigned int getProjectionLocation();
	unsigned int getModelLocation();
	unsigned int getViewLocation();
	unsigned int getAmbientIntensityLocation();
	unsigned int getAmbientColorLocation();
	unsigned int getDiffuseIntensityLocation();
	unsigned int getDirectionLocation();

	struct {
		int uniformColor;
		int uniformAmbientIntensity, uniformDiffuseIntensity;
		int uniformDirection;
	} uniformDirectionalLight;

	int uniformPointLightCount;

	struct {
		int uniformColor;
		int uniformAmbientIntensity, uniformDiffuseIntensity;
		int uniformPosition;

		int uniformConstant, uniformLinear, uniformExponent;
	} uniformPointLight[MAX_POINT_LIGHTS];

	void setDirectionalLight(DirectionalLight* dLight);
	void setPointLights(PointLight* pLight, unsigned int lightCount);
	void setTexture(GLenum texture_unit);
	void setDirectionalShadowMap(GLenum texture_unit);
	void setDirectionalLightTransform(mat4 lTransform);

	unsigned int getShaderId() { return shaderId; }

	string readShaderFiles(const char* fileLocation);

	void useShader();
	void clearShader();

private:
	unsigned int shaderId, uniformModel, uniformProjection, uniformView, 
		         uniformDirectionalLightTransform, uniformDirectionalShadowMap,
				 uniformTexture;
	unsigned int compileShader(unsigned int type, const char* source);
	void addShader(const char* vertexCode, const char* fragmentCode);
};

