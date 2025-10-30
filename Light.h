#pragma once

#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <glm/glm.hpp>
#include <glm\gtc/type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

using namespace glm;

class Light
{
public:
	Light();
	Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity, GLfloat dIntensity);
	void useLight(GLfloat aIntensityLocation, GLfloat aColorLocation, GLfloat dIntensityLocation);
	~Light();

protected:
	vec3 color;
	GLfloat ambientIntensity;
	GLfloat diffuseIntensity;
};

