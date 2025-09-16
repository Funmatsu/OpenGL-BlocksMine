#include "Window.h"

using namespace std;


Window::Window() {
	width = 800;
	height = 600;

    for (int i = 0; i < 1024; i++) {
        keys[i] = false;
    }

    xChange = 0.0f;
    yChange = 0.0f;
}
Window::Window(GLint windowWidth, GLint windowHeight) {
	width = windowWidth;
	height = windowHeight;

    for (int i = 0; i < 1024; i++) {
        keys[i] = false;
    }

    xChange = 0.0f;
    yChange = 0.0f;
}

int Window::initialize() {
    if (!glfwInit()) {
        cout << "GLFW initialization failed!" << endl;
        glfwTerminate();
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    mainWindow = glfwCreateWindow(width, height, "OpenGLBlocksMine", NULL, NULL);

    if (!mainWindow) {
        cout << "Default Window failed to open!" << endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(mainWindow);

    glewExperimental = GL_TRUE;

    createCalbacks();
    glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (glewInit() != GLEW_OK) {
        cout << "GLEW was not properly initialized!";
        return 1;
    }

    glEnable(GL_DEPTH_TEST);

    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);
    glViewport(0, 0, bufferWidth, bufferHeight);

    glfwSetWindowUserPointer(mainWindow, this);
}
void Window::createCalbacks() {
    glfwSetKeyCallback(mainWindow, handleKeys);
    glfwSetCursorPosCallback(mainWindow, handleMouse);
}
void Window::handleKeys(GLFWwindow* window, int key, int code, int action, int mode) {
    Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key >= 0 && key <= 1024) {
        if (action == GLFW_PRESS) {
            theWindow->keys[key] = true;
            //cout << "Pressed key!" << key << endl;
        }
        else if (action == GLFW_RELEASE) {
            theWindow->keys[key] = false;
            //cout << "Released key!" << key << endl;
        }
    }
}

void Window::handleMouse(GLFWwindow* window, double xPos, double yPos) {
    Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (theWindow->mouseFirstMoved) {
        theWindow->lastX = xPos;
        theWindow->lastY = yPos;
        theWindow->mouseFirstMoved = false;
    }
    
    theWindow->xChange = xPos - theWindow->lastX;
    theWindow->yChange = yPos - theWindow->lastY;

    theWindow->lastX = xPos;
    theWindow->lastY = yPos;
}

Window::~Window() {
    //cout << "Default Window failed to open!" << endl;
    glfwTerminate();
}