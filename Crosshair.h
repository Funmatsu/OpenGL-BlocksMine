#pragma once
#include "libraries.h"

extern int WIDTH ;
extern int HEIGHT;

extern float centerX;
extern float centerY;
extern float crosshairSize;

class Crosshair
{
public:
    void defineCrosshairGeometry();
    void drawCrosshair();

private:
    vector<glm::vec2> crosshairVertices;
    vector<GLfloat> vers;
    vector<unsigned int> inds;
    Mesh crosshair;
    GLuint crosshairVbo = 0;
    GLuint crosshairVao = 0;
    GLuint crosshairIbo = 0;
};

