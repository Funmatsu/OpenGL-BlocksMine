#pragma once
#include "Light.h"
class DirectionalLight :
    public Light
{
public:
    DirectionalLight();
    ~DirectionalLight();
    DirectionalLight(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity);
    DirectionalLight(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity, float xDir, float yDir, float zDir, GLfloat dIntensity);
    void useLight(GLfloat aIntensityLocation, GLfloat aColorLocation);
    void useLight(GLfloat aIntensityLocation, GLfloat aColorLocation, GLfloat dIntensityLocation, GLfloat directionLocation);
private:
    vec3 direction;
};

