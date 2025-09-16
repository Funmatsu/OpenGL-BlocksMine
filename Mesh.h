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

using namespace std;

class Mesh
{
public:
	Mesh();
	void createMesh(vector<float> vertices, vector<unsigned int> indices, unsigned int verticesCount, unsigned int indicesCount);
	void createMeshWater(vector<float> vertices, vector<unsigned int> indices, unsigned int verticesCount, unsigned int indicesCount);
	void renderMesh();
	void clearMesh();
	vector<float> verts;
	vector<unsigned int> inds;
	~Mesh();
private:
	unsigned int vao, vbo, ibo;
	GLsizei indexCount;

};

