#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() : Light() {}
DirectionalLight::DirectionalLight(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity, GLfloat xDir, GLfloat yDir, GLfloat zDir, GLfloat dIntensity) : Light(red, green, blue, aIntensity, dIntensity){
	direction = vec3(xDir, yDir, zDir);
}
void DirectionalLight::useLight(GLfloat aIntensityLocation, GLfloat aColorLocation, GLfloat dIntensityLocation, GLfloat directionLocation) {
	glUniform3f(aColorLocation, color.x, color.y, color.z);
	glUniform1f(aIntensityLocation, ambientIntensity);
	glUniform1f(dIntensityLocation, diffuseIntensity);
	glUniform3f(directionLocation, direction.x, direction.y, direction.z);
}
DirectionalLight::~DirectionalLight() {}