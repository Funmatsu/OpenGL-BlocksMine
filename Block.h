#pragma once

#include "libraries.h"

struct Face {
    ivec3 center = ivec3(0.0f);
    vector<float> vertices;
    vector<unsigned int> indices;
    int faceId = 0;
    bool operator==(const Face& face) {
        return (vertices == face.vertices);
    }
};

class Block {
public:
    vector<float> vertices;
    vector<unsigned int> indices;
    vec3 position = vec3(0.0f);
    Item type;
    Mesh blockMesh;
    Face faces[6];
    int faceNum = 6;
    int chunkId = 0;

    Block();

    Block(vec3 pos, Item blockType, vector<float> verts, vector<unsigned int> inds);

    Block(const Block& block);

    static void giveBlock(Block donor, Block receptor);

    vec3 getBlockPosition() { return position; }
};

Block::Block() {
    position = vec3();
    vertices = {};
    indices = {};
    type = AIR;
}

Block::Block(vec3 pos, Item blockType, vector<float> verts, vector<unsigned int> inds) {
    position = pos;
    vertices = verts;
    indices = inds;
    type = blockType;
    blockMesh = Mesh();
}

Block::Block(const Block& block) {
    position = block.position;
    //position.x = 5.0f;
    //cout << position.x << endl;
    vertices = block.vertices;
    indices = block.indices;
    type = block.type;
    blockMesh = block.blockMesh;
    faceNum = block.faceNum;
    chunkId = block.chunkId;
}

void Block::giveBlock(Block donor, Block receptor) {
    receptor.position = donor.position;
    receptor.vertices = donor.vertices;

    receptor.indices = donor.indices;
    receptor.type = donor.type;
    receptor.blockMesh = donor.blockMesh;
}

vector<GLfloat> blockVerts = {
    0.0f,       0.0f,      0.0f , // 0
    1.0f,       0.0f,      0.0f , // 1
    1.0f,       1.0f,      0.0f , // 2
    0.0f,       1.0f,      0.0f , // 3
    0.0f,       0.0f,      1.0f , // 4
    1.0f,       0.0f,      1.0f , // 5
    1.0f,       1.0f,      1.0f , // 6
    0.0f,       1.0f,      1.0f   // 7
};
