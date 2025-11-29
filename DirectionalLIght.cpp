#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() : Light() { lightProj = ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);		}
DirectionalLight::DirectionalLight(GLuint shadowWidth, GLuint shadowHeight,
	GLfloat red, GLfloat green, GLfloat blue,
	GLfloat aIntensity, GLfloat dIntensity,
	GLfloat xDir, GLfloat yDir, GLfloat zDir) : Light(shadowWidth, shadowHeight, red, green, blue, aIntensity, dIntensity){
	direction = vec3(xDir, yDir, zDir);
	float off = 100.0f;
	lightProj = ortho(-25.0f - off - shadowPos.x, 25.0f + off + shadowPos.x, -25.0f - off - shadowPos.y, 25.0f + off + shadowPos.y, 0.5f, 500.0f);
	//lightProj = ortho(-shadowWidth, shadowWidth, -shadowHeight, shadowHeight);
}
void DirectionalLight::useLight(GLfloat aIntensityLocation, GLfloat aColorLocation, GLfloat dIntensityLocation, GLfloat directionLocation) {
	glUniform3f(aColorLocation, color.x, color.y, color.z);
	glUniform1f(aIntensityLocation, ambientIntensity);
	glUniform1f(dIntensityLocation, diffuseIntensity);
	glUniform3f(directionLocation, direction.x, direction.y, direction.z);
}

mat4 DirectionalLight::calcLightTransform() {
	directionalLightTransform = lightProj * /*kinda like the view abit like proj * view * model*/
		lookAt(-direction, /*front*/vec3(0.0f), /*up*/vec3(0.0f, 1.0f, 0.0f));
	return directionalLightTransform;
}

DirectionalLight::~DirectionalLight() {}