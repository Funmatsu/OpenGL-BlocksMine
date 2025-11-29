#pragma once
#include "Light.h"

class DirectionalLight :
    public Light
{
public:
    DirectionalLight();
    DirectionalLight(GLuint shadowWidth, GLuint shadowHeight, GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity, float xDir, float yDir, float zDir, GLfloat dIntensity);
    void useLight(GLfloat aIntensityLocation, GLfloat aColorLocation, GLfloat dIntensityLocation, GLfloat directionLocation);

    mat4 calcLightTransform();
    void setShadowPos(vec2 shadPos) { shadowPos = shadPos; }
    ~DirectionalLight();
private:
    vec3 direction;
    vec2 shadowPos = vec2(0.0f);
};

