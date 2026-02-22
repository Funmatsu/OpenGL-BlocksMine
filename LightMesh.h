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

class LightMesh
{
public:
	LightMesh();
	void createMesh(vector<float>& vertices, vector<unsigned int>& indices, unsigned int verticesCount, unsigned int indicesCount);
	void createMesh(vector<float>& vertices, vector<unsigned int>& indices);
	void createMesh(float* vertsdata, unsigned int* indsdata, size_t indsize, size_t vertsize);
	void renderMesh();
	void renderMeshAsLines();
	void clearMesh();
	void giveMesh();
	~LightMesh();
	LightMesh(const LightMesh& mesh) {
		vao = mesh.vao, vbo = mesh.vbo, ibo = mesh.ibo;
		indexCount = mesh.indexCount;
	}
	void operator=(LightMesh mesh) {
		vao = mesh.vao, vbo = mesh.vbo, ibo = mesh.ibo;
		indexCount = mesh.indexCount;
	}
	bool operator==(LightMesh mesh) {
		return (vao == mesh.vao, vbo == mesh.vbo, ibo == mesh.ibo);
	}
	bool operator!=(LightMesh mesh) {
		return !(*this == mesh);
	}
private:
	unsigned int vao, vbo, ibo, indexCount;
};

