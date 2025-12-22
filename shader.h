#pragma once

#include <GL/glew.h>
#include<string>
#include <sstream>
#include <fstream>
#include <iostream>
#include "DirectionalLight.h"
#include "PointLIght.h"

using namespace std;
class glShader
{
public:
	int pointLightCount;
	glShader();
	~glShader();
	void createShaderFromString(const char* vertexCode, const char* fragmentCode);
	void createShaderFromFiles(const char* vertexFilePath, const char* fragmentFilePath);
	unsigned int getProjectionLocation();
	unsigned int getModelLocation();
	unsigned int getViewLocation();
	unsigned int getAmbientIntensityLocation();
	unsigned int getAmbientColorLocation();
	unsigned int getDiffuseIntensityLocation();
	unsigned int getDirectionLocation();
	unsigned int getColorMaskLocation();
	unsigned int getOrthoLocation();

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
				 uniformTexture, uniformColorMask, uniformOrtho;
	unsigned int compileShader(unsigned int type, const char* source);
	void addShader(const char* vertexCode, const char* fragmentCode);
};

