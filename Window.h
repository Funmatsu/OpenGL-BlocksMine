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
	int initializeFullScreen();
	GLint getBufferWidth() { return bufferWidth; }
	GLint getBufferHeight() { return bufferHeight; }
	int getFramebufferId() { return fbo; }
	bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }
	bool* getKeys() { return keys; }
	float getXChange() { float change = xChange; xChange = 0.0; return change; }
	float getYChange() { float change = yChange; yChange = 0.0; return change; }
	int getKeyPressed();
	void evictbackKeypress() { recentlyPressedKey = 0; }
	void swapBuffers() { glfwSwapBuffers(mainWindow); }
	GLFWwindow* getWindow() { return mainWindow;  }
	bool leftClickButtonPressed() { return glfwGetMouseButton(mainWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS; }
	bool rightClickButtonPressed() { return glfwGetMouseButton(mainWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS; }
	void initializeDepthBuffer();
	void bindDepthBuffer();
	void useDepthBuffer(int shaderId);
	void updateDepthTexture();
	double getXPos();
	double getYPos();
	~Window();

	void setMouseMoved() { mouseFirstMoved = true; }

private:
	GLFWwindow* mainWindow;
	GLint width, height, bufferWidth, bufferHeight;
	GLuint depthTex;
	GLuint fbo;
	bool keys[1024];
	int recentlyPressedKey;
	float lastX = 0, lastY = 0, xChange, yChange;
	double mouseX = 0, mouseY = 0;
	bool mouseFirstMoved;

	void createCalbacks();
	static void handleKeys(GLFWwindow *window, int key, int code, int action, int mode);
	static void handleMouse(GLFWwindow* window, double xPos, double yPos);
};

