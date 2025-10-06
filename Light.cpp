#include "Light.h"
Light::Light() {
	color = vec3(1.0f, 1.0f, 1.0f);
	ambientIntensity = 1.0f;
	direction = vec3(0.0f, -1.0f, 0.0f);
	diffuseIntensity = 0.0f;
}
Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity) {
	color = vec3(red, green, blue);
	ambientIntensity = aIntensity;
}
Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity, GLfloat xDir, GLfloat yDir, GLfloat zDir, GLfloat dIntensity) {
	color = vec3(red, green, blue);
	ambientIntensity = aIntensity;
	direction = vec3(xDir, yDir, zDir);
	diffuseIntensity = dIntensity;
}
void Light::useLight(GLfloat aIntensityLocation, GLfloat aColorLocation) {
	glUniform3f(aColorLocation, color.x, color.y, color.z);
	glUniform1f(aIntensityLocation, ambientIntensity);
}
void Light::useLight(GLfloat aIntensityLocation, GLfloat aColorLocation, GLfloat dIntensityLocation, GLfloat directionLocation) {
	glUniform3f(aColorLocation, color.x, color.y, color.z);
	glUniform1f(aIntensityLocation, ambientIntensity);
	glUniform1f(dIntensityLocation, diffuseIntensity);
	glUniform3f(directionLocation, direction.x, direction.y, direction.z);
}
Light::~Light(){}