#pragma once
#include "libraries.h"

static const char* vshaderSky = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/vshaderSky.txt";
static const char* fshaderSky = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/fshaderSky.txt";

static const char* vshaderother = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/vshader.txt";
static const char* fshaderother = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/fshader.txt";

class Sky
{
public:
	vector<float> skyVerts = {
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

		1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

		-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

		-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f
	};

public:
	vector<float> sunVerts = {
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.5f, 0.0f, 0.0f, 0.0f,
	};

	vector<unsigned int> skyInds = {
			0, 1, 2,
			2, 3, 0,

			4, 5, 6,
			6, 7, 4,

			8, 9, 10,
			10, 11, 8,

			12, 13, 14,
			14, 15, 12,

			16, 17, 18,
			18, 19, 16,

			20, 21, 22,
			22, 23, 20
	};

	vector<unsigned int> sunInds = {
			0, 1, 2,
			2, 3, 0,
	};

	Shadergl shader = Shadergl();
	Shadergl sunshader = Shadergl();

	Sky() {}
	Mesh skyMesh;
	Mesh sunMesh;
	void useShader() {
		shader.useShader();
	}
	void buildSky() { 
		shader.createShaderFromFiles(vshaderSky, fshaderSky); 
		sunshader.createShaderFromFiles(vshaderother, fshaderother);
		skyMesh.createMesh(skyVerts, skyInds, skyVerts.size(), skyInds.size());
		sunMesh.createMesh(sunVerts, sunInds, sunVerts.size(), sunInds.size());
	}
	void displaySky() { skyMesh.renderMesh(); sunMesh.renderMesh(); }
	void applySky(mat4 view, mat4 projection) {
		glDepthMask(GL_FALSE);        // disable depth writes
		shader.useShader();
		glUniformMatrix4fv(shader.getViewLocation(), 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(shader.getProjectionLocation(), 1, GL_FALSE, value_ptr(projection));
		displaySky();
		glDepthMask(GL_TRUE);
	}
};