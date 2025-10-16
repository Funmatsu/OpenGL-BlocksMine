#include "Mesh.h"

Mesh::Mesh() {
	vao = 0;
	vbo = 0;
	ibo = 0;
	indexCount = 0;
}

void Mesh::createMesh(vector<float> vertices, vector<unsigned int> indices, unsigned int verticesCount, unsigned int indicesCount) {
    indexCount = indicesCount;
    if (indexCount != 0) {
        if (vao == 0)
            glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        if (ibo == 0)
            glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

        if (vbo == 0)
            glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(5 * sizeof(float)));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
        verts = move(vertices);
        inds = move(indices);
    }
    //cout << " Done creating Mesh! " << endl;
}

void Mesh::renderMesh() {
    if (indexCount != 0) {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

void Mesh::clearMesh() {
    if (ibo != 0) {
        glDeleteBuffers(1, &ibo);
        ibo = 0;
    }
    if (vbo != 0) {
       glDeleteBuffers(1, &vbo);
        vbo = 0;
    }
    if (vao != 0) {
       glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
    indexCount = 0;
    verts.clear();
    inds.clear();
}

Mesh::~Mesh() {
    //clearMesh();
}