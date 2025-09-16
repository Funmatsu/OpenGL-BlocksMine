#include "Camera.h"

using namespace std;

Camera::Camera() {
	position = vec3(0.0f, -1.0f, 0.0f);
	worldUp = vec3(0.0f, 1.0f, 0.0f);;
	yaw = 0.1f;
	pitch = 0.1f;
	front = vec3(0.0f, 0.0f, -1.0f);
	movementSpeed = 0.1f;
	turnSpeed = 0.1f;

	update();
}
Camera::Camera(vec3 startPosition, vec3 startUp, float startYaw, float startPitch, float startMoveSpeed, float startTurnSpeed) {
	position = startPosition;
	worldUp = startUp;
	yaw = startYaw;
	pitch = startPitch;
	front = vec3(0.0f, 0.0f, 0.0f);
	movementSpeed = startMoveSpeed;
	turnSpeed = startTurnSpeed;

	update();
}

void Camera::update() {
	front = normalize
	(vec3(
		cos(radians(yaw)) * cos(radians(pitch)),
		sin(radians(pitch)), 
		sin(radians(yaw)) * cos(radians(pitch))
		)
	);
	right = normalize(cross(front, worldUp));
	up = normalize(cross(right, front));
}

void Camera::keyControl(bool* keys, float deltaTime) {
	if (keys[GLFW_KEY_W] && !GLFW_RELEASE) {
		position.x += front.x * movementSpeed * deltaTime;
		position.z += front.z * movementSpeed * deltaTime;
		//keys[GLFW_KEY_W] = false;
	}
	if (keys[GLFW_KEY_S]) {
		position.x -= front.x * movementSpeed * deltaTime;
		position.z -= front.z * movementSpeed * deltaTime;
	}
	if (keys[GLFW_KEY_A]) {
		position -= right * movementSpeed * deltaTime;
	}
	if (keys[GLFW_KEY_D]) {
		position += right * movementSpeed * deltaTime;
	}
	if (keys[GLFW_KEY_SPACE]) {
		position.y += movementSpeed * deltaTime * 2;
	}
	if (keys[GLFW_KEY_LEFT_SHIFT]) {
		position.y -= movementSpeed * deltaTime;
	}
	if (keys[GLFW_KEY_LEFT_CONTROL]) {
		movementSpeed *= 1.4;
		keys[GLFW_KEY_LEFT_CONTROL] = false;
	}
	if (keys[GLFW_KEY_RIGHT_CONTROL]) {
		movementSpeed /= 1.4;
		keys[GLFW_KEY_LEFT_CONTROL] = false;
	}
	if(keys[GLFW_KEY_L]) {
		//cout << "Camera Position: \nx: " << position.x << ", y: " << position.y << ", z: " << position.z << endl;
		position = vec3(0.0f, 1.0f, 0.0f);
	}
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange) {
	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch -= yChange;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	update();
}

mat4 Camera::calcViewMatrix() {
	return lookAt(
		position,  // Camera position
		position + front,  // Look at origin
		up   // Up vector
	);
}

Camera::~Camera() {}