#pragma once

#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm\gtc/type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <cmath>
#include <vector>

using namespace glm;

class Camera
{
public:
	Camera();
	Camera(vec3 startPosition, vec3 startUp, float startYaw, float startPitch, float startMoveSpeed, float startTurnSpeed);
	~Camera();
	void keyControl(bool* keys, float deltaTime);
	void mouseControl(GLfloat xChange, GLfloat yChange);
	vec3 getCameraFront() { return front; }
	vec3 getCameraPos() { return position; }
	void setCameraPos(vec3 pos) { position = pos; }
	mat4 calcViewMatrix();
private:
	vec3 position, front, up, right, worldUp;
	float yaw, pitch, movementSpeed, turnSpeed;
	void update();
};

