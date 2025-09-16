#pragma once
#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int initialize();
	GLint getBufferWidth() { return bufferWidth; }
	GLint getBufferHeight() { return bufferHeight; }
	bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }
	bool* getKeys() { return keys; }
	float getXChange() { float change = xChange; xChange = 0.0; return change; }
	float getYChange() { float change = yChange; yChange = 0.0; return change; }
	void swapBuffers() { glfwSwapBuffers(mainWindow); }
	GLFWwindow* getWindow() { return mainWindow;  }
	bool leftClickButtonPressed() { return glfwGetMouseButton(mainWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS; }
	bool rightClickButtonPressed() { return glfwGetMouseButton(mainWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS; }
	~Window();
	float getMove() {
		return translation;
	}
	void setMove(float move) {
		translation += move;
	}

private:
	GLFWwindow* mainWindow;
	GLint width, height, bufferWidth, bufferHeight;
	bool keys[1024];
	float lastX, lastY, xChange, yChange;
	bool mouseFirstMoved;
	float translation;

	void createCalbacks();
	static void handleKeys(GLFWwindow *window, int key, int code, int action, int mode);
	static void handleMouse(GLFWwindow* window, double xPos, double yPos);
};

