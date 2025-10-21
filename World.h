#pragma once

#include "Chunk.h"
#include "normals.h"
#include "recipes.h"
#include "inventory.h"

float randomFloat(float min, float max) {
    static std::default_random_engine generator(std::random_device{}());
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(generator);
}

bool contains(vector<vec2> vec, vec2 value) {
    return std::find(vec.begin(), vec.end(), value) != vec.end();
}

Camera camera = Camera(vec3(CHUNK_SIZE / 2, CHUNK_SIZE * CHUNK_SIZE + 5, CHUNK_SIZE / 2), vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f, 2.5f, 0.5f);
int xdimens = 5, ydimens = 5;
int xdimensItem = 2, ydimensItem = 2;

class World {
    public:
    vector<Chunk> chunks;

    Block getBlockAt(vec3 blockPos);

    void addChunk(Chunk newChunk);

    void addBlockToWorld(vec3 position, vector<float> vertices, vector<unsigned int> indices, int vertNum, int indNum, Item blockType);

    void addBlockToWorld(Block block);

    void deleteBlockFromWorld(vec3 blockPos);

    Mesh createMeshCube(float x, float y, float z, Item blockType);

    Block createMeshCube(vec3 blockPos, Item blockType);

    Mesh createMeshCube(float x, float y, float z, float scale, Item blockType);

    Block createMeshCube(vec3 blockPos, float scale, Item blockType);

    void createCube(float xoffset, float yoffset, float zoffset, Item blockType);

    void delBlocklook_at();

    void addBlocklook_at(Item blockType);


};

World world;
bool inv_change;
int slot = 0;
float slotX = 0.0f, slotY = 0.0f;



//.cpp part-----------------------------------------------------------------------------------------------------------------------



bool blockExistsAt(ivec3 blockPos) {
    if (world.chunks.size() == 0) {
        return false;
    }

    return (worldBlocks.find(blockPos) != worldBlocks.end());
    //auto it = worldBlocks.find(blockPos);
    //if (it == worldBlocks.end()) {
    //    //std::cout << "BlockData at " << blockPos.x << "," << blockPos.y << "," << blockPos.z << std::endl;
    //    // Print internal fields safely
    //    return false;
    //}
    //else {
    //    return true;
    //}

    ////cout << blockPos.x << " " << worldBlocks[blockPos].position.x << endl;
    //try {
    //    bool exists = (worldBlocks.find(blockPos) != worldBlocks.end());
    //    //std::cout << "Exists: " << exists << std::endl;
    //}
    //catch (...) {
    //    std::cout << "Crash during lookup!" << std::endl;
    //}

    //return (worldBlocks.count(blockPos) > 0);
    //return (blockPos == ivec3(worldBlocks[blockPos].position));
    //for (int k = 0; k < world.chunks.size(); k++) {
    //    for (int i = 0; i < world.chunks[k].blocks.size(); i++) {
    //        ivec3 checkBlock = floor(world.chunks[k].blocks[i].position);
    //        if (checkBlock == blockPos) {
    //            return true;
    //        }
    //    }
    //}
    //return false;
}

bool blockExistsAt(ivec3 blockPos, Item blockType) {
    if (world.chunks.size() == 0) {
        return false;
    }

    if (worldBlocks.find(blockPos) != worldBlocks.end()) {
        return (worldBlocks[blockPos].type == blockType || worldBlocks[blockPos].type == GRASS || worldBlocks[blockPos].type == POPPY);
    }
    return false;

    //for (int k = 0; k < world.chunks.size(); k++) {
    //    for (int i = 0; i < world.chunks[k].blocks.size(); i++) {
    //        ivec3 checkBlock = floor(world.chunks[k].blocks[i].position);
    //        if (checkBlock == blockPos && world.chunks[k].blocks[i].type == blockType) {
    //            return true;
    //        }
    //    }
    //}
    //cout << blockPos.x << " " << blockPos.y << " " << blockPos.z << endl;
}

vec3 lookingAtBlock() {
    ivec3 blockPos = vec3(0.0f);
    glm::vec3 rayDir;
    glm::vec3 rayOrigin = camera.getCameraPos();
    rayDir = normalize(camera.getCameraFront());

    float maxDistance = 7.0f;
    float stepSize = 0.2f;
    for (float t = 0.0f; t < maxDistance; t += stepSize) {
        glm::vec3 point = rayOrigin + rayDir * t;
        blockPos = glm::floor(point);
        
        if (blockExistsAt(blockPos)) {
            //if (worldBlocks[blockPos].type == AIR)
                //cout << "AIR" << endl;
            return blockPos;
        }
    }
    return vec3(-404.0f);
}

Block World::getBlockAt(vec3 blockPos) {
    return worldBlocks[ivec3(blockPos)];
    /*for (int k = 0; k < chunks.size(); k++) {
        for (int i = 0; i < chunks[k].blocks.size(); i++) {
            ivec3 checkBlock = floor(chunks[k].blocks[i].position);
            if (checkBlock == ivec3(blockPos)) {
                return chunks[k].blocks[i];
            }
        }
    }*/
}
void World::addChunk(Chunk newChunk) {
    chunks.push_back(newChunk);
}

void World::addBlockToWorld(vec3 position, vector<float> vertices, vector<unsigned int> indices, int vertNum, int indNum, Item blockType) {
    for (int i = 0; i < vertNum; i++) {
        chunks[chunks.size() - 1].vertices.push_back(vertices[i]);
    }
    for (int i = 0; i < indNum; i++) {
        chunks[chunks.size() - 1].indices.push_back(indices[i]);
    }
    chunks[chunks.size() - 1].needUpdate = true;
    Block newBlock(position, blockType, vertices, indices);
    chunks.back().blocks.push_back(newBlock);
    chunks.back().addBlock(newBlock);
}

void World::addBlockToWorld(Block block) {
    chunks.back().vertices.insert(chunks.back().vertices.end(), block.vertices.begin(), block.vertices.end());
    chunks.back().indices.insert(chunks.back().indices.end(), block.indices.begin(), block.indices.end());
    chunks.back().addBlock(block);
    chunks.back().needUpdate = true;
}

void World::deleteBlockFromWorld(vec3 blockPos) {
    for (int k = 0; k < chunks.size(); k++) {
        int blockVertNums = 0;
        bool blockBroken = false;
        for (int i = 0; i < chunks[k].blocks.size(); i++) {
            if (chunks[k].blocks[i].position == blockPos) {

                for (int m = 0; m < 4; m++) {
                    for (int l = 0; l < 9; l++) {
                        if (inventory.inv_slots[3 - m][l] == AIR) {
                            inventory.inv_slots[3 - m][l] = chunks[k].blocks[i].type;
                            inv_change = true;
                            blockBroken = true;
                            break;
                        }
                    }
                    if (blockBroken) {
                        break;
                    }
                }

                chunks[k].vertices.erase(chunks[k].vertices.begin() + blockVertNums, chunks[k].vertices.begin() + blockVertNums + chunks[k].blocks[i].vertices.size());
                chunks[k].blocks.erase(chunks[k].blocks.begin() + i);

                chunks[k].needUpdate = true;
                return;
            }
            blockVertNums += chunks[k].blocks[i].vertices.size();
        }
    }
    worldBlocks.erase(ivec3(floor(blockPos)));
}

Mesh World::createMeshCube(float x, float y, float z, Item blockType) {
    float xoffset = 0, yoffset = 1, xoffsetTop = 0, yoffsetTop = 0, xoffsetBottom = 0, yoffsetBottom = 0, transparency = 1.0f;

    int arbitraryoff = 0;
    if (blockType == GRASS_BLOCK) {
        xoffset = 0, yoffset = 2; xoffsetTop = 2; yoffsetTop = 0; xoffsetBottom = 2; yoffsetBottom = 2;
    }

    else if (blockType == IRON_ORE) {
        xoffset = 1; yoffset = 1 + arbitraryoff;
    }

    else if (blockType == STONE_BLOCK) {
        xoffset = 1; yoffset = 2;
    }

    else if (blockType == DIRT_BLOCK) {
        xoffset = 2; yoffset = 0;
    }

    else if (blockType == OAK_WOOD) {
        xoffset = 2, yoffset = 1; xoffsetTop = -2; yoffsetTop = 2; xoffsetBottom = -2; yoffsetBottom = 2;
    }

    else if (blockType == CLOUD) {
        xoffset = 1, yoffset = 0, transparency = 0.75f;
    }

    else if (blockType == OAK_PLANK) {
        xoffset = 3, yoffset = 0;
    }

    else if (blockType == OAK_LEAVES) {
        xoffset = 0, yoffset = 0;
    }

    else if (blockType == GRASS) {
        x = 3, y = 1; xoffsetTop = 0, xoffsetBottom = 0; yoffsetTop = 1, yoffsetBottom = 1;
    }

    else if (blockType == CRAFTING_TABLE) {
        x = 3; y = 2; xoffsetTop = -1, xoffsetBottom = 1; yoffsetTop = 0, yoffsetBottom = -3;
    }

    else if (blockType == BLUE_ORCHID) {
        xoffset = 0, yoffset = 4;
    }

    else if (blockType == BEDROCK) {
        xoffset = 4, yoffset = 0;
    }

    vector<unsigned int> indices = {
        0,  1,  2,
        3,  4,  5,
        6,  7,  8,
        9,  10, 11,
        12, 13, 14,
        15, 16, 17,
        18, 19, 20,
        21, 22, 23,
        24, 25, 26,
        27, 28, 29,
        30, 31, 32,
        33, 34, 35
    };
    float clipX = 0.03f, clipY = 0.97f;
    vector<GLfloat> globalUVs =
    {
        (clipX + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,

        (clipX + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,

        (clipX + xoffset + xoffsetBottom) / xdimens,   (clipX + yoffset + yoffsetBottom) / ydimens, transparency,
        (clipY + xoffset + xoffsetBottom) / xdimens,   (clipX + yoffset + yoffsetBottom) / ydimens, transparency,
        (clipX + xoffset + xoffsetBottom) / xdimens,   (clipY + yoffset + yoffsetBottom) / ydimens, transparency,
        (clipX + xoffset + xoffsetBottom) / xdimens,   (clipY + yoffset + yoffsetBottom) / ydimens, transparency,
        (clipY + xoffset + xoffsetBottom) / xdimens,   (clipX + yoffset + yoffsetBottom) / ydimens, transparency,
        (clipY + xoffset + xoffsetBottom) / xdimens,   (clipY + yoffset + yoffsetBottom) / ydimens, transparency,

        (clipX + xoffset + xoffsetTop) / xdimens,      (clipX + yoffset + yoffsetTop) / ydimens, transparency,
        (clipX + xoffset + xoffsetTop) / xdimens,      (clipY + yoffset + yoffsetTop) / ydimens, transparency,
        (clipY + xoffset + xoffsetTop) / xdimens,      (clipX + yoffset + yoffsetTop) / ydimens, transparency,
        (clipX + xoffset + xoffsetTop) / xdimens,      (clipY + yoffset + yoffsetTop) / ydimens, transparency,
        (clipY + xoffset + xoffsetTop) / xdimens,      (clipY + yoffset + yoffsetTop) / ydimens, transparency,
        (clipY + xoffset + xoffsetTop) / xdimens,      (clipX + yoffset + yoffsetTop) / ydimens, transparency,

        (clipX + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,

        (clipX + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency
    };

    vector<GLfloat> triangle = {
        0.0f + x, 0.0f + y, 0.0f + z,
        0.0f + x, 1.0f + y, 0.0f + z,
        0.0f + x, 0.0f + y, 0.1f + z,
        0.0f + x, 1.0f + y, 0.0f + z,
        0.0f + x, 1.0f + y, 0.1f + z,
        0.0f + x, 0.0f + y, 0.1f + z,
        1.0f + x, 0.0f + y, 0.0f + z,
        1.0f + x, 0.0f + y, 0.1f + z,
        1.0f + x, 1.0f + y, 0.0f + z,
        1.0f + x, 1.0f + y, 0.0f + z,
        1.0f + x, 0.0f + y, 0.1f + z,
        1.0f + x, 1.0f + y, 0.1f + z,
        0.0f + x, 0.0f + y, 0.0f + z,
        0.0f + x, 0.0f + y, 0.1f + z,
        1.0f + x, 0.0f + y, 0.0f + z,
        1.0f + x, 0.0f + y, 0.0f + z,
        0.0f + x, 0.0f + y, 0.1f + z,
        1.0f + x, 0.0f + y, 0.1f + z,
        0.0f + x, 1.0f + y, 0.0f + z,
        1.0f + x, 1.0f + y, 0.0f + z,
        0.0f + x, 1.0f + y, 0.1f + z,
        1.0f + x, 1.0f + y, 0.0f + z,
        1.0f + x, 1.0f + y, 0.1f + z,
        0.0f + x, 1.0f + y, 0.1f + z,
        0.0f + x, 0.0f + y, 0.0f + z,
        1.0f + x, 0.0f + y, 0.0f + z,
        0.0f + x, 1.0f + y, 0.0f + z,
        0.0f + x, 1.0f + y, 0.0f + z,
        1.0f + x, 0.0f + y, 0.0f + z,
        1.0f + x, 1.0f + y, 0.0f + z,
        0.0f + x, 0.0f + y, 0.1f + z,
        0.0f + x, 1.0f + y, 0.1f + z,
        1.0f + x, 0.0f + y, 0.1f + z,
        0.0f + x, 1.0f + y, 0.1f + z,
        1.0f + x, 1.0f + y, 0.1f + z,
        1.0f + x, 0.0f + y, 0.1f + z
    };

    if (blockType == GRASS) {
        indices = {
            0 , 1 , 2 ,
            3 , 4 , 5 ,
            6 , 7 , 8 ,
            9 , 10 , 11
        };

        triangle = {
            0.0f + x, 0.0f + y, 0.0f + z,
            0.0f + x, 1.0f + y, 0.0f + z,
            1.0f + x, 0.0f + y, 0.0f + z,
            0.0f + x, 1.0f + y, 0.0f + z,
            1.0f + x, 1.0f + y, 0.0f + z,
            1.0f + x, 0.0f + y, 0.0f + z,
        };

        globalUVs = {
            (clipX + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
            (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
            (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
            (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
            (clipY + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
            (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency
        };
    }

    vector<GLfloat> finalVerts;
    for (int i = 0; i < triangle.size() / 3; i++) {
        finalVerts.push_back(triangle[3 * i + 0]);
        finalVerts.push_back(triangle[3 * i + 1]);
        finalVerts.push_back(triangle[3 * i + 2]);

        finalVerts.push_back(globalUVs[3 * i + 0]);
        finalVerts.push_back(globalUVs[3 * i + 1]);
        finalVerts.push_back(globalUVs[3 * i + 2]);
    }
    calcAverageNormals(finalVerts, indices, 3, 6, 0);
    Mesh cubeMesh;
    cubeMesh.createMesh(finalVerts, indices, finalVerts.size(), indices.size());
    return cubeMesh;
}

Block World::createMeshCube(vec3 blockPos, Item blockType) {
    float xoffset = 0, yoffset = 1, xoffsetTop = 0, yoffsetTop = 0, xoffsetBottom = 0, yoffsetBottom = 0, transparency = 1.0f;

    int arbitraryoff = 0;
    if (blockType == GRASS_BLOCK) {
        xoffset = 0, yoffset = 2; xoffsetTop = 2; yoffsetTop = 0; xoffsetBottom = 2; yoffsetBottom = 2;
    }

    else if (blockType == IRON_ORE) {
        xoffset = 1; yoffset = 1 + arbitraryoff;
    }

    else if (blockType == STONE_BLOCK) {
        xoffset = 1; yoffset = 2;
    }

    else if (blockType == DIRT_BLOCK) {
        xoffset = 2; yoffset = 0;
    }

    else if (blockType == OAK_WOOD) {
        xoffset = 2, yoffset = 1; xoffsetTop = -2; yoffsetTop = 2; xoffsetBottom = -2; yoffsetBottom = 2;
    }

    else if (blockType == CLOUD) {
        xoffset = 1, yoffset = 0, transparency = 0.75f;
    }

    else if (blockType == OAK_PLANK) {
        xoffset = 3, yoffset = 0;
    }

    else if (blockType == OAK_LEAVES) {
        xoffset = 0, yoffset = 0;
    }

    else if (blockType == GRASS) {
        xoffset = 3, yoffset = 1; xoffsetTop = 0, xoffsetBottom = 0; yoffsetTop = 1, yoffsetBottom = 1;
    }

    else if (blockType == CRAFTING_TABLE) {
        xoffset = 2; yoffset = 3; xoffsetTop = -1, xoffsetBottom = 1; yoffsetTop = 0, yoffsetBottom = -3;
    }

    else if (blockType == BLUE_ORCHID) {
        xoffset = 0, yoffset = 4;
    }

    else if (blockType == BEDROCK) {
        xoffset = 4, yoffset = 0;
    }

    vector<unsigned int> indices = {
        0,  1,  2,
        3,  4,  5,
        6,  7,  8,
        9,  10, 11,
        12, 13, 14,
        15, 16, 17,
        18, 19, 20,
        21, 22, 23,
        24, 25, 26,
        27, 28, 29,
        30, 31, 32,
        33, 34, 35
    };
    float clipX = 0.0f, clipY = 1.0f;
    vector<GLfloat> globalUVs =
    {
        (clipX + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,

        (clipX + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,

        (clipX + xoffset + xoffsetBottom) / xdimens,   (clipX + yoffset + yoffsetBottom) / ydimens, transparency,
        (clipY + xoffset + xoffsetBottom) / xdimens,   (clipX + yoffset + yoffsetBottom) / ydimens, transparency,
        (clipX + xoffset + xoffsetBottom) / xdimens,   (clipY + yoffset + yoffsetBottom) / ydimens, transparency,
        (clipX + xoffset + xoffsetBottom) / xdimens,   (clipY + yoffset + yoffsetBottom) / ydimens, transparency,
        (clipY + xoffset + xoffsetBottom) / xdimens,   (clipX + yoffset + yoffsetBottom) / ydimens, transparency,
        (clipY + xoffset + xoffsetBottom) / xdimens,   (clipY + yoffset + yoffsetBottom) / ydimens, transparency,

        (clipX + xoffset + xoffsetTop) / xdimens,      (clipX + yoffset + yoffsetTop) / ydimens, transparency,
        (clipX + xoffset + xoffsetTop) / xdimens,      (clipY + yoffset + yoffsetTop) / ydimens, transparency,
        (clipY + xoffset + xoffsetTop) / xdimens,      (clipX + yoffset + yoffsetTop) / ydimens, transparency,
        (clipX + xoffset + xoffsetTop) / xdimens,      (clipY + yoffset + yoffsetTop) / ydimens, transparency,
        (clipY + xoffset + xoffsetTop) / xdimens,      (clipY + yoffset + yoffsetTop) / ydimens, transparency,
        (clipY + xoffset + xoffsetTop) / xdimens,      (clipX + yoffset + yoffsetTop) / ydimens, transparency,

        (clipX + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,

        (clipX + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency
    };

    vector<GLfloat> triangle = {
        0.0f + blockPos.x, 0.0f + blockPos.y, 0.0f + blockPos.z,
        0.0f + blockPos.x, 1.0f + blockPos.y, 0.0f + blockPos.z,
        0.0f + blockPos.x, 0.0f + blockPos.y, 1.0f + blockPos.z,
        0.0f + blockPos.x, 1.0f + blockPos.y, 0.0f + blockPos.z,
        0.0f + blockPos.x, 1.0f + blockPos.y, 1.0f + blockPos.z,
        0.0f + blockPos.x, 0.0f + blockPos.y, 1.0f + blockPos.z,
        1.0f + blockPos.x, 0.0f + blockPos.y, 0.0f + blockPos.z,
        1.0f + blockPos.x, 0.0f + blockPos.y, 1.0f + blockPos.z,
        1.0f + blockPos.x, 1.0f + blockPos.y, 0.0f + blockPos.z,
        1.0f + blockPos.x, 1.0f + blockPos.y, 0.0f + blockPos.z,
        1.0f + blockPos.x, 0.0f + blockPos.y, 1.0f + blockPos.z,
        1.0f + blockPos.x, 1.0f + blockPos.y, 1.0f + blockPos.z,
        0.0f + blockPos.x, 0.0f + blockPos.y, 0.0f + blockPos.z,
        0.0f + blockPos.x, 0.0f + blockPos.y, 1.0f + blockPos.z,
        1.0f + blockPos.x, 0.0f + blockPos.y, 0.0f + blockPos.z,
        1.0f + blockPos.x, 0.0f + blockPos.y, 0.0f + blockPos.z,
        0.0f + blockPos.x, 0.0f + blockPos.y, 1.0f + blockPos.z,
        1.0f + blockPos.x, 0.0f + blockPos.y, 1.0f + blockPos.z,
        0.0f + blockPos.x, 1.0f + blockPos.y, 0.0f + blockPos.z,
        1.0f + blockPos.x, 1.0f + blockPos.y, 0.0f + blockPos.z,
        0.0f + blockPos.x, 1.0f + blockPos.y, 1.0f + blockPos.z,
        1.0f + blockPos.x, 1.0f + blockPos.y, 0.0f + blockPos.z,
        1.0f + blockPos.x, 1.0f + blockPos.y, 1.0f + blockPos.z,
        0.0f + blockPos.x, 1.0f + blockPos.y, 1.0f + blockPos.z,
        0.0f + blockPos.x, 0.0f + blockPos.y, 0.0f + blockPos.z,
        1.0f + blockPos.x, 0.0f + blockPos.y, 0.0f + blockPos.z,
        0.0f + blockPos.x, 1.0f + blockPos.y, 0.0f + blockPos.z,
        0.0f + blockPos.x, 1.0f + blockPos.y, 0.0f + blockPos.z,
        1.0f + blockPos.x, 0.0f + blockPos.y, 0.0f + blockPos.z,
        1.0f + blockPos.x, 1.0f + blockPos.y, 0.0f + blockPos.z,
        0.0f + blockPos.x, 0.0f + blockPos.y, 1.0f + blockPos.z,
        0.0f + blockPos.x, 1.0f + blockPos.y, 1.0f + blockPos.z,
        1.0f + blockPos.x, 0.0f + blockPos.y, 1.0f + blockPos.z,
        0.0f + blockPos.x, 1.0f + blockPos.y, 1.0f + blockPos.z,
        1.0f + blockPos.x, 1.0f + blockPos.y, 1.0f + blockPos.z,
        1.0f + blockPos.x, 0.0f + blockPos.y, 1.0f + blockPos.z
    };
    vector<GLfloat> normals = {
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f
    };
    if (blockType == GRASS) {
        indices = {
            0 , 1 , 2 ,
            3 , 4 , 5,
            6 , 7 , 8 ,
            9 , 10 , 11
        };

        triangle = {
            0.0f + blockPos.x, 0.0f + blockPos.y, 0.0f + blockPos.z,
            0.0f + blockPos.x, 1.0f + blockPos.y, 0.0f + blockPos.z,
            1.0f + blockPos.x, 0.0f + blockPos.y, 0.0f + blockPos.z,
            0.0f + blockPos.x, 1.0f + blockPos.y, 0.0f + blockPos.z,
            1.0f + blockPos.x, 1.0f + blockPos.y, 0.0f + blockPos.z,
            1.0f + blockPos.x, 0.0f + blockPos.y, 0.0f + blockPos.z,
        };

        globalUVs = {
            (clipX + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
            (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
            (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
            (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
            (clipY + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
            (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency
        };

        vector<GLfloat> normals = {
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,

            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f
        };
    }

    vector<GLfloat> finalVerts;
    for (int i = 0; i < triangle.size() / 3; i++) {
        finalVerts.push_back(triangle[3 * i + 0]);
        finalVerts.push_back(triangle[3 * i + 1]);
        finalVerts.push_back(triangle[3 * i + 2]);

        finalVerts.push_back(globalUVs[3 * i + 0]);
        finalVerts.push_back(globalUVs[3 * i + 1]);
        finalVerts.push_back(globalUVs[3 * i + 2]);

        finalVerts.push_back(normals[3 * i + 0]);
        finalVerts.push_back(normals[3 * i + 1]);
        finalVerts.push_back(normals[3 * i + 2]);
    }

    Mesh cubeMesh;
    cubeMesh.createMesh(finalVerts, indices, finalVerts.size(), indices.size());
    return Block(blockPos, blockType, finalVerts, indices);
}

Mesh World::createMeshCube(float x, float y, float z, float scale, Item blockType) {
    if (blockType == AIR) {
        return Mesh();
    }
    float xoffset = 0, yoffset = 1, xoffsetTop = 0, yoffsetTop = 0, xoffsetBottom = 0, yoffsetBottom = 0, transparency = 1.0f;

    int arbitraryoff = 0;
    float yexponent = 1.5f;
    vector<GLfloat> triangle;
    vector<GLfloat> globalUVs;
    vector<unsigned int> indices;
    vector<GLfloat> normals;
    if (!recipe.isTool(blockType)) {

        if (blockType == GRASS_BLOCK) {
            xoffset = 0, yoffset = 2; xoffsetTop = 2; yoffsetTop = 0; xoffsetBottom = 2; yoffsetBottom = 2;
        }

        else if (blockType == IRON_ORE) {
            xoffset = 1; yoffset = 1 + arbitraryoff;
        }

        else if (blockType == STONE_BLOCK) {
            xoffset = 1; yoffset = 2;
        }

        else if (blockType == DIRT_BLOCK) {
            xoffset = 2; yoffset = 0;
        }

        else if (blockType == OAK_WOOD) {
            xoffset = 2, yoffset = 1; xoffsetTop = -2; yoffsetTop = 2; xoffsetBottom = -2; yoffsetBottom = 2;
        }

        else if (blockType == CLOUD) {
            xoffset = 1, yoffset = 0, transparency = 0.75f;
        }

        else if (blockType == OAK_PLANK) {
            xoffset = 3, yoffset = 0;
        }

        else if (blockType == OAK_LEAVES) {
            xoffset = 0, yoffset = 0;
        }

        else if (blockType == GRASS) {
            xoffset = 3, yoffset = 1; xoffsetTop = 0, xoffsetBottom = 0; yoffsetTop = 1, yoffsetBottom = 1;
        }

        else if (blockType == POPPY) {
            xoffset = 3, yoffset = 2; xoffsetTop = 0, xoffsetBottom = 0; yoffsetTop = 1, yoffsetBottom = 1;
        }

        else if (blockType == CRAFTING_TABLE) {
            xoffset = 2; yoffset = 3; xoffsetTop = -1, xoffsetBottom = 1; yoffsetTop = 0, yoffsetBottom = -3;
        }

        else if (blockType == BLUE_ORCHID) {
            xoffset = 0, yoffset = 4;
        }

        else if (blockType == BEDROCK) {
            xoffset = 4, yoffset = 0;
        }

        indices = {
            0,  1,  2,
            3,  4,  5,
            6,  7,  8,
            9,  10, 11,
            12, 13, 14,
            15, 16, 17,
            18, 19, 20,
            21, 22, 23,
            24, 25, 26,
            27, 28, 29,
            30, 31, 32,
            33, 34, 35
        };
        float clipX = 0.03f, clipY = 0.97f;
        globalUVs =
        {
            (clipX + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
            (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
            (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
            (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
            (clipY + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
            (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,

            (clipX + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
            (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
            (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
            (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
            (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
            (clipY + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,

            (clipX + xoffset + xoffsetBottom) / xdimens,   (clipX + yoffset + yoffsetBottom) / ydimens, transparency,
            (clipY + xoffset + xoffsetBottom) / xdimens,   (clipX + yoffset + yoffsetBottom) / ydimens, transparency,
            (clipX + xoffset + xoffsetBottom) / xdimens,   (clipY + yoffset + yoffsetBottom) / ydimens, transparency,
            (clipX + xoffset + xoffsetBottom) / xdimens,   (clipY + yoffset + yoffsetBottom) / ydimens, transparency,
            (clipY + xoffset + xoffsetBottom) / xdimens,   (clipX + yoffset + yoffsetBottom) / ydimens, transparency,
            (clipY + xoffset + xoffsetBottom) / xdimens,   (clipY + yoffset + yoffsetBottom) / ydimens, transparency,

            (clipX + xoffset + xoffsetTop) / xdimens,      (clipX + yoffset + yoffsetTop) / ydimens, transparency,
            (clipX + xoffset + xoffsetTop) / xdimens,      (clipY + yoffset + yoffsetTop) / ydimens, transparency,
            (clipY + xoffset + xoffsetTop) / xdimens,      (clipX + yoffset + yoffsetTop) / ydimens, transparency,
            (clipX + xoffset + xoffsetTop) / xdimens,      (clipY + yoffset + yoffsetTop) / ydimens, transparency,
            (clipY + xoffset + xoffsetTop) / xdimens,      (clipY + yoffset + yoffsetTop) / ydimens, transparency,
            (clipY + xoffset + xoffsetTop) / xdimens,      (clipX + yoffset + yoffsetTop) / ydimens, transparency,

            (clipX + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
            (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
            (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
            (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
            (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
            (clipY + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,

            (clipX + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
            (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
            (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
            (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
            (clipY + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
            (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency
        };

        triangle = {
            0.0f * scale + x, 0.0f * scale * yexponent + y, 0.0f * scale / 100 + z,
            0.0f * scale + x, 1.0f * scale * yexponent + y, 0.0f * scale / 100 + z,
            0.0f * scale + x, 0.0f * scale * yexponent + y, 0.1f * scale / 100 + z,
            0.0f * scale + x, 1.0f * scale * yexponent + y, 0.0f * scale / 100 + z,
            0.0f * scale + x, 1.0f * scale * yexponent + y, 0.1f * scale / 100 + z,
            0.0f * scale + x, 0.0f * scale * yexponent + y, 0.1f * scale / 100 + z,

            1.0f * scale + x, 0.0f * scale * yexponent + y, 0.0f * scale / 100 + z,
            1.0f * scale + x, 0.0f * scale * yexponent + y, 0.1f * scale / 100 + z,
            1.0f * scale + x, 1.0f * scale * yexponent + y, 0.0f * scale / 100 + z,
            1.0f * scale + x, 1.0f * scale * yexponent + y, 0.0f * scale / 100 + z,
            1.0f * scale + x, 0.0f * scale * yexponent + y, 0.1f * scale / 100 + z,
            1.0f * scale + x, 1.0f * scale * yexponent + y, 0.1f * scale / 100 + z,

            0.0f * scale + x, 0.0f * scale * yexponent + y, 0.0f * scale / 100 + z,
            0.0f * scale + x, 0.0f * scale * yexponent + y, 0.1f * scale / 100 + z,
            1.0f * scale + x, 0.0f * scale * yexponent + y, 0.0f * scale / 100 + z,
            1.0f * scale + x, 0.0f * scale * yexponent + y, 0.0f * scale / 100 + z,
            0.0f * scale + x, 0.0f * scale * yexponent + y, 0.1f * scale / 100 + z,
            1.0f * scale + x, 0.0f * scale * yexponent + y, 0.1f * scale / 100 + z,

            0.0f * scale + x, 1.0f * scale * yexponent + y, 0.0f * scale / 100 + z,
            1.0f * scale + x, 1.0f * scale * yexponent + y, 0.0f * scale / 100 + z,
            0.0f * scale + x, 1.0f * scale * yexponent + y, 0.1f * scale / 100 + z,
            1.0f * scale + x, 1.0f * scale * yexponent + y, 0.0f * scale / 100 + z,
            1.0f * scale + x, 1.0f * scale * yexponent + y, 0.1f * scale / 100 + z,
            0.0f * scale + x, 1.0f * scale * yexponent + y, 0.1f * scale / 100 + z,

            0.0f * scale + x, 0.0f * scale * yexponent + y, 0.0f * scale / 100 + z,
            1.0f * scale + x, 0.0f * scale * yexponent + y, 0.0f * scale / 100 + z,
            0.0f * scale + x, 1.0f * scale * yexponent + y, 0.0f * scale / 100 + z,
            0.0f * scale + x, 1.0f * scale * yexponent + y, 0.0f * scale / 100 + z,
            1.0f * scale + x, 0.0f * scale * yexponent + y, 0.0f * scale / 100 + z,
            1.0f * scale + x, 1.0f * scale * yexponent + y, 0.0f * scale / 100 + z,

            0.0f * scale + x, 0.0f * scale * yexponent + y, 0.1f * scale / 100 + z,
            0.0f * scale + x, 1.0f * scale * yexponent + y, 0.1f * scale / 100 + z,
            1.0f * scale + x, 0.0f * scale * yexponent + y, 0.1f * scale / 100 + z,
            0.0f * scale + x, 1.0f * scale * yexponent + y, 0.1f * scale / 100 + z,
            1.0f * scale + x, 1.0f * scale * yexponent + y, 0.1f * scale / 100 + z,
            1.0f * scale + x, 0.0f * scale * yexponent + y, 0.1f * scale / 100 + z
        };

        normals = {
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,

            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,

            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,

            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,

            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,

            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f
        };

        if (blockType == GRASS || blockType == POPPY) {
            indices = {
                0 , 1 , 2 ,
                3 , 4 , 5 
            };

            triangle = {
                0.0f * scale + x, 0.0f * scale * yexponent + y, 0.0f + z,
                0.0f * scale + x, 1.0f * scale * yexponent + y, 0.0f + z,
                1.0f * scale + x, 0.0f * scale * yexponent + y, 0.0f + z,
                0.0f * scale + x, 1.0f * scale * yexponent + y, 0.0f + z,
                1.0f * scale + x, 1.0f * scale * yexponent + y, 0.0f + z,
                1.0f * scale + x, 0.0f * scale * yexponent + y, 0.0f + z,
            };

            globalUVs = {
                (clipX + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
                (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
                (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
                (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
                (clipY + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
                (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency
            };

            normals = {
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
            };
        }
    }
    else {
        if (blockType == WOODEN_PICKAXE) {
            xoffset = 0, yoffset = 1;
        }

        else if (blockType == STICK) {
            xoffset = 1, yoffset = 1;
        }

        indices = {
            0,  1,  2,
            3,  4,  5
        };
        float clipX = 0.03f, clipY = 0.97f;

        triangle = {
                0.0f * scale + x, 0.0f * scale * yexponent + y, 0.0f + z,
                0.0f * scale + x, 1.0f * scale * yexponent + y, 0.0f + z,
                1.0f * scale + x, 0.0f * scale * yexponent + y, 0.0f + z,
                0.0f * scale + x, 1.0f * scale * yexponent + y, 0.0f + z,
                1.0f * scale + x, 1.0f * scale * yexponent + y, 0.0f + z,
                1.0f * scale + x, 0.0f * scale * yexponent + y, 0.0f + z,
        };

        globalUVs = {
                (clipX + xoffset) / xdimensItem,   (clipX + yoffset) / ydimensItem, transparency,
                (clipX + xoffset) / xdimensItem,   (clipY + yoffset) / ydimensItem, transparency,
                (clipY + xoffset) / xdimensItem,   (clipX + yoffset) / ydimensItem, transparency,
                (clipX + xoffset) / xdimensItem,   (clipY + yoffset) / ydimensItem, transparency,
                (clipY + xoffset) / xdimensItem,   (clipY + yoffset) / ydimensItem, transparency,
                (clipY + xoffset) / xdimensItem,   (clipX + yoffset) / ydimensItem, transparency
        };

        normals = {
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
        };
    }

    vector<GLfloat> finalVerts;
    for (int i = 0; i < triangle.size(); i += 3) {
        finalVerts.push_back(triangle[i + 0]);
        finalVerts.push_back(triangle[i + 1]);
        finalVerts.push_back(triangle[i + 2]);

        finalVerts.push_back(globalUVs[i + 0]);
        finalVerts.push_back(globalUVs[i + 1]);
        finalVerts.push_back(globalUVs[i + 2]);

        finalVerts.push_back(normals[i + 0]);
        finalVerts.push_back(normals[i + 1]);
        finalVerts.push_back(normals[i + 2]);
    }

    Mesh cubeMesh;
    cubeMesh.createMesh(finalVerts, indices, finalVerts.size(), indices.size());
    return cubeMesh;
}

Block World::createMeshCube(vec3 blockPos, float scale, Item blockType) {
    float xoffset = 1, yoffset = 0, transparency = 0.75f, xoffsetTop = 0, yoffsetTop = 0, xoffsetBottom = 0, yoffsetBottom = 0;

    int arbitraryoff = 0;
    float yexponent = 1.0f;
    if (blockType == GRASS_BLOCK) {
        xoffset = 0, yoffset = 2; xoffsetTop = 2; yoffsetTop = 0; xoffsetBottom = 2; yoffsetBottom = 0;
    }

    else if (blockType == IRON_ORE) {
        xoffset = 1; yoffset = 1 + arbitraryoff;
    }

    else if (blockType == STONE_BLOCK) {
        xoffset = 1; yoffset = 2;
    }

    else if (blockType == DIRT_BLOCK) {
        xoffset = 2; yoffset = 0;
    }

    else if (blockType == OAK_WOOD) {
        xoffset = 2, yoffset = 1; xoffsetTop = -2; yoffsetTop = 2; xoffsetBottom = -2; yoffsetBottom = 2;
    }

    else if (blockType == CLOUD) {
        xoffset = 1, yoffset = 0, transparency = 0.75f;
    }

    else if (blockType == OAK_PLANK) {
        xoffset = 3, yoffset = 0;
    }

    else if (blockType == OAK_LEAVES) {
        xoffset = 0, yoffset = 0;
    }

    else if (blockType == BEDROCK) {
        xoffset = 4, yoffset = 0;
    }

    vector<unsigned int> indices = {
        0,  1,  2,
        3,  4,  5,
        6,  7,  8,
        9,  10, 11,
        12, 13, 14,
        15, 16, 17,
        18, 19, 20,
        21, 22, 23,
        24, 25, 26,
        27, 28, 29,
        30, 31, 32,
        33, 34, 35
    };
    float clipX = 0.03f, clipY = 0.97f;
    vector<GLfloat> globalUVs =
    {
        (clipX + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,

        (clipX + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,

        (clipX + xoffset + xoffsetBottom) / xdimens,   (clipX + yoffset + yoffsetBottom) / ydimens, transparency,
        (clipY + xoffset + xoffsetBottom) / xdimens,   (clipX + yoffset + yoffsetBottom) / ydimens, transparency,
        (clipX + xoffset + xoffsetBottom) / xdimens,   (clipY + yoffset + yoffsetBottom) / ydimens, transparency,
        (clipX + xoffset + xoffsetBottom) / xdimens,   (clipY + yoffset + yoffsetBottom) / ydimens, transparency,
        (clipY + xoffset + xoffsetBottom) / xdimens,   (clipX + yoffset + yoffsetBottom) / ydimens, transparency,
        (clipY + xoffset + xoffsetBottom) / xdimens,   (clipY + yoffset + yoffsetBottom) / ydimens, transparency,

        (clipX + xoffset + xoffsetTop) / xdimens,      (clipX + yoffset + yoffsetTop) / ydimens, transparency,
        (clipX + xoffset + xoffsetTop) / xdimens,      (clipY + yoffset + yoffsetTop) / ydimens, transparency,
        (clipY + xoffset + xoffsetTop) / xdimens,      (clipX + yoffset + yoffsetTop) / ydimens, transparency,
        (clipX + xoffset + xoffsetTop) / xdimens,      (clipY + yoffset + yoffsetTop) / ydimens, transparency,
        (clipY + xoffset + xoffsetTop) / xdimens,      (clipY + yoffset + yoffsetTop) / ydimens, transparency,
        (clipY + xoffset + xoffsetTop) / xdimens,      (clipX + yoffset + yoffsetTop) / ydimens, transparency,

        (clipX + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,

        (clipX + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,                   (clipX + yoffset) / ydimens, transparency
    };

    float test[1][3][1] = { {{0.0}, {0.0}, {0.0}} };

    vector<GLfloat> triangle = {
        0.0f + blockPos.x + 0.1f * -scale, 0.0f + blockPos.y + 0.1f * -scale, 0.0f + blockPos.z + 0.1f * -scale,
        0.0f + blockPos.x + 0.1f * -scale, 1.0f + blockPos.y + 0.1f * scale, 0.0f + blockPos.z + 0.1f * -scale,
        0.0f + blockPos.x + 0.1f * -scale, 0.0f + blockPos.y + 0.1f * -scale, 1.0f + blockPos.z + 0.1f * scale,
        0.0f + blockPos.x + 0.1f * -scale, 1.0f + blockPos.y + 0.1f * scale, 0.0f + blockPos.z + 0.1f * -scale,
        0.0f + blockPos.x + 0.1f * -scale, 1.0f + blockPos.y + 0.1f * scale, 1.0f + blockPos.z + 0.1f * scale,
        0.0f + blockPos.x + 0.1f * -scale, 0.0f + blockPos.y + 0.1f * -scale, 1.0f + blockPos.z + 0.1f * scale,
                                
        1.0f + blockPos.x + 0.1f * scale, 0.0f + blockPos.y + 0.1f * -scale, 0.0f + blockPos.z + 0.1f * -scale,
        1.0f + blockPos.x + 0.1f * scale, 0.0f + blockPos.y + 0.1f * -scale, 1.0f + blockPos.z + 0.1f * scale,
        1.0f + blockPos.x + 0.1f * scale, 1.0f + blockPos.y + 0.1f * scale, 0.0f + blockPos.z + 0.1f * -scale,
        1.0f + blockPos.x + 0.1f * scale, 1.0f + blockPos.y + 0.1f * scale, 0.0f + blockPos.z + 0.1f * -scale,
        1.0f + blockPos.x + 0.1f * scale, 0.0f + blockPos.y + 0.1f * -scale, 1.0f + blockPos.z + 0.1f * scale,
        1.0f + blockPos.x + 0.1f * scale, 1.0f + blockPos.y + 0.1f * scale, 1.0f + blockPos.z + 0.1f * scale,
                                
        0.0f + blockPos.x + 0.1f * -scale, 0.0f + blockPos.y + 0.1f * -scale, 0.0f + blockPos.z + 0.1f * -scale,
        0.0f + blockPos.x + 0.1f * -scale, 0.0f + blockPos.y + 0.1f * -scale, 1.0f + blockPos.z + 0.1f * scale,
        1.0f + blockPos.x + 0.1f * scale, 0.0f + blockPos.y + 0.1f * -scale, 0.0f + blockPos.z + 0.1f * -scale,
        1.0f + blockPos.x + 0.1f * scale, 0.0f + blockPos.y + 0.1f * -scale, 0.0f + blockPos.z + 0.1f * -scale,
        0.0f + blockPos.x + 0.1f * -scale, 0.0f + blockPos.y + 0.1f * -scale, 1.0f + blockPos.z + 0.1f * scale,
        1.0f + blockPos.x + 0.1f * scale, 0.0f + blockPos.y + 0.1f * -scale, 1.0f + blockPos.z + 0.1f * scale,
                                
        0.0f + blockPos.x + 0.1f * -scale, 1.0f + blockPos.y + 0.1f * scale, 0.0f + blockPos.z + 0.1f * -scale,
        1.0f + blockPos.x + 0.1f * scale, 1.0f + blockPos.y + 0.1f * scale, 0.0f + blockPos.z + 0.1f * -scale,
        0.0f + blockPos.x + 0.1f * -scale, 1.0f + blockPos.y + 0.1f * scale, 1.0f + blockPos.z + 0.1f * scale,
        1.0f + blockPos.x + 0.1f * scale, 1.0f + blockPos.y + 0.1f * scale, 0.0f + blockPos.z + 0.1f * -scale,
        1.0f + blockPos.x + 0.1f * scale, 1.0f + blockPos.y + 0.1f * scale, 1.0f + blockPos.z + 0.1f * scale,
        0.0f + blockPos.x + 0.1f * -scale, 1.0f + blockPos.y + 0.1f * scale, 1.0f + blockPos.z + 0.1f * scale,
                                
        0.0f + blockPos.x + 0.1f * -scale, 0.0f + blockPos.y + 0.1f * -scale, 0.0f + blockPos.z + 0.1f * -scale,
        1.0f + blockPos.x + 0.1f * scale, 0.0f + blockPos.y + 0.1f * -scale, 0.0f + blockPos.z + 0.1f * -scale,
        0.0f + blockPos.x + 0.1f * -scale, 1.0f + blockPos.y + 0.1f * scale, 0.0f + blockPos.z + 0.1f * -scale,
        0.0f + blockPos.x + 0.1f * -scale, 1.0f + blockPos.y + 0.1f * scale, 0.0f + blockPos.z + 0.1f * -scale,
        1.0f + blockPos.x + 0.1f * scale, 0.0f + blockPos.y + 0.1f * -scale, 0.0f + blockPos.z + 0.1f * -scale,
        1.0f + blockPos.x + 0.1f * scale, 1.0f + blockPos.y + 0.1f * scale, 0.0f + blockPos.z + 0.1f * -scale,
                                
        0.0f + blockPos.x + 0.1f * -scale, 0.0f + blockPos.y + 0.1f * -scale, 1.0f + blockPos.z + 0.1f * scale,
        0.0f + blockPos.x + 0.1f * -scale, 1.0f + blockPos.y + 0.1f * scale, 1.0f + blockPos.z + 0.1f * scale,
        1.0f + blockPos.x + 0.1f * scale, 0.0f + blockPos.y + 0.1f * -scale, 1.0f + blockPos.z + 0.1f * scale,
        0.0f + blockPos.x + 0.1f * -scale, 1.0f + blockPos.y + 0.1f * scale, 1.0f + blockPos.z + 0.1f * scale,
        1.0f + blockPos.x + 0.1f * scale, 1.0f + blockPos.y + 0.1f * scale, 1.0f + blockPos.z + 0.1f * scale,
        1.0f + blockPos.x + 0.1f * scale, 0.0f + blockPos.y + 0.1f * -scale, 1.0f + blockPos.z + 0.1f * scale
    };

    vector<GLfloat> normals = {
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f
    };

    vector<GLfloat> finalVerts;
    for (int i = 0; i < triangle.size() / 3; i += 1) {
        finalVerts.push_back(triangle[3 * i + 0]);
        finalVerts.push_back(triangle[3 * i + 1]);
        finalVerts.push_back(triangle[3 * i + 2]);

        finalVerts.push_back(globalUVs[3 * i + 0]);
        finalVerts.push_back(globalUVs[3 * i + 1]);
        finalVerts.push_back(globalUVs[3 * i + 2]);

        finalVerts.push_back(normals[3 * i + 0]);
        finalVerts.push_back(normals[3 * i + 1]);
        finalVerts.push_back(normals[3 * i + 2]);
    }

    Block returnBlock = Block(blockPos, blockType, finalVerts, indices);
    returnBlock.blockMesh.createMesh(finalVerts, indices, finalVerts.size(), indices.size());;
    return returnBlock;
}

//void World::createCube(float xoffset, float yoffset, float zoffset, Item blockType) {
//    float x = 0, y = 1, xoffsetTop = 0, yoffsetTop = 0, xoffsetBottom = 0, yoffsetBottom = 0, transparency = 1.0f;
//
//    int arbitraryoff = 0;
//    if (blockType == GRASS_BLOCK) {
//        x = 0, y = 2; xoffsetTop = 2; yoffsetTop = 0; xoffsetBottom = 2; yoffsetBottom = 2;
//    }
//
//    else if (blockType == IRON_ORE) {
//        x = 1; y = 1 + arbitraryoff;
//    }
//
//    else if (blockType == STONE_BLOCK) {
//        x = 1; y = 2;
//    }
//
//    else if (blockType == DIRT_BLOCK) {
//        x = 2; y = 0;
//    }
//
//    else if (blockType == OAK_WOOD) {
//        x = 2, y = 1; xoffsetTop = -2; yoffsetTop = 2; xoffsetBottom = -2; yoffsetBottom = -2;
//    }
//
//    else if (blockType == CLOUD) {
//        x = 1, y = 0, transparency = 0.75f;
//    }
//
//    else if (blockType == OAK_PLANK) {
//        x = 3, y = 0;
//    }
//
//    else if (blockType == OAK_LEAVES) {
//        x = 0, y = 0;
//    }
//
//    else if (blockType == GRASS) {
//        x = 3, y = 1; xoffsetTop = 0, xoffsetBottom = 0; yoffsetTop = 1, yoffsetBottom = 1;
//    }
//
//    else if (blockType == POPPY) {
//        x = 3, y = 2; xoffsetTop = 0, xoffsetBottom = 0; yoffsetTop = 1, yoffsetBottom = 1;
//    }
//
//    else if (blockType == CRAFTING_TABLE) {
//        x = 2; y = 3; xoffsetTop = -1, xoffsetBottom = 1; yoffsetTop = 0, yoffsetBottom = -3;
//    }
//
//    else if (blockType == BLUE_ORCHID) {
//        x = 0, y = 4;
//    }
//
//    else if (blockType == BEDROCK) {
//        x = 4, y = 0;
//    }
//
//    unsigned int indexOffset = 0;
//    for (int i = 0; i < chunks.back().blocks.size(); i++) {
//        if (chunks.back().blocks.size() > 0) {
//            indexOffset += chunks.back().blocks[i].indices.size();
//        }
//    }
//    vector<unsigned int> indices1 = {
//        0 + indexOffset, 1 + indexOffset, 2 + indexOffset,
//        3 + indexOffset, 4 + indexOffset, 5 + indexOffset,
//        6 + indexOffset, 7 + indexOffset, 8 + indexOffset,
//        9 + indexOffset, 10 + indexOffset, 11 + indexOffset,
//        12 + indexOffset, 13 + indexOffset, 14 + indexOffset,
//        15 + indexOffset, 16 + indexOffset, 17 + indexOffset,
//        18 + indexOffset, 19 + indexOffset, 20 + indexOffset,
//        21 + indexOffset, 22 + indexOffset, 23 + indexOffset,
//        24 + indexOffset, 25 + indexOffset, 26 + indexOffset,
//        27 + indexOffset, 28 + indexOffset, 29 + indexOffset,
//        30 + indexOffset, 31 + indexOffset, 32 + indexOffset,
//        33 + indexOffset, 34 + indexOffset, 35 + indexOffset
//    };
//    float clipX = 0.03f, clipY = 0.97f;
//    vector<GLfloat> globalUVs =
//    {
//        (clipX + x) / xdimens,   (clipX + y) / ydimens, transparency,
//        (clipX + x) / xdimens,   (clipY + y) / ydimens, transparency,
//        (clipY + x) / xdimens,   (clipX + y) / ydimens, transparency,
//        (clipX + x) / xdimens,   (clipY + y) / ydimens, transparency,
//        (clipY + x) / xdimens,   (clipY + y) / ydimens, transparency,
//        (clipY + x) / xdimens,   (clipX + y) / ydimens, transparency,
//
//        (clipX + x) / xdimens,   (clipX + y) / ydimens, transparency,
//        (clipY + x) / xdimens,   (clipX + y) / ydimens, transparency,
//        (clipX + x) / xdimens,   (clipY + y) / ydimens, transparency,
//        (clipX + x) / xdimens,   (clipY + y) / ydimens, transparency,
//        (clipY + x) / xdimens,   (clipX + y) / ydimens, transparency,
//        (clipY + x) / xdimens,   (clipY + y) / ydimens, transparency,
//
//        (clipX + x + xoffsetBottom) / xdimens,   (clipX + y + yoffsetBottom) / ydimens, transparency,
//        (clipY + x + xoffsetBottom) / xdimens,   (clipX + y + yoffsetBottom) / ydimens, transparency,
//        (clipX + x + xoffsetBottom) / xdimens,   (clipY + y + yoffsetBottom) / ydimens, transparency,
//        (clipX + x + xoffsetBottom) / xdimens,   (clipY + y + yoffsetBottom) / ydimens, transparency,
//        (clipY + x + xoffsetBottom) / xdimens,   (clipX + y + yoffsetBottom) / ydimens, transparency,
//        (clipY + x + xoffsetBottom) / xdimens,   (clipY + y + yoffsetBottom) / ydimens, transparency,
//
//        (clipX + x + xoffsetTop) / xdimens,   (clipX + y + yoffsetTop) / ydimens, transparency,
//        (clipX + x + xoffsetTop) / xdimens,   (clipY + y + yoffsetTop) / ydimens, transparency,
//        (clipY + x + xoffsetTop) / xdimens,   (clipX + y + yoffsetTop) / ydimens, transparency,
//        (clipX + x + xoffsetTop) / xdimens,   (clipY + y + yoffsetTop) / ydimens, transparency,
//        (clipY + x + xoffsetTop) / xdimens,   (clipY + y + yoffsetTop) / ydimens, transparency,
//        (clipY + x + xoffsetTop) / xdimens,   (clipX + y + yoffsetTop) / ydimens, transparency,
//
//        (clipX + x) / xdimens,   (clipX + y) / ydimens, transparency,
//        (clipY + x) / xdimens,   (clipX + y) / ydimens, transparency,
//        (clipX + x) / xdimens,   (clipY + y) / ydimens, transparency,
//        (clipX + x) / xdimens,   (clipY + y) / ydimens, transparency,
//        (clipY + x) / xdimens,   (clipX + y) / ydimens, transparency,
//        (clipY + x) / xdimens,   (clipY + y) / ydimens, transparency,
//
//        (clipX + x) / xdimens,   (clipX + y) / ydimens, transparency,
//        (clipX + x) / xdimens,   (clipY + y) / ydimens, transparency,
//        (clipY + x) / xdimens,   (clipX + y) / ydimens, transparency,
//        (clipX + x) / xdimens,   (clipY + y) / ydimens, transparency,
//        (clipY + x) / xdimens,   (clipY + y) / ydimens, transparency,
//        (clipY + x) / xdimens,   (clipX + y) / ydimens, transparency
//    };
//
//    vector<GLfloat> triangle = {
//        0.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
//        0.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
//        0.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
//        0.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
//        0.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
//        0.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
//
//        1.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
//        1.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
//        1.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
//        1.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
//        1.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
//        1.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
//
//        0.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
//        0.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
//        1.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
//        1.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
//        0.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
//        1.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
//
//        0.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
//        1.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
//        0.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
//        1.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
//        1.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
//        0.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
//
//        0.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
//        1.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
//        0.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
//        0.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
//        1.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
//        1.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
//
//        0.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
//        0.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
//        1.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
//        0.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
//        1.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
//        1.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
//    };
//
//    vector<GLfloat> normals = {
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 0.0f
//    };
//
//    if (blockType == GRASS || blockType == POPPY || blockType == BLUE_ORCHID) {
//        indices1 = {
//            0 + indexOffset, 1 + indexOffset, 2 + indexOffset,
//            3 + indexOffset, 4 + indexOffset, 5 + indexOffset,
//            6 + indexOffset, 7 + indexOffset, 8 + indexOffset,
//            9 + indexOffset, 10 + indexOffset, 11 + indexOffset
//        };
//
//        triangle = {
//            0.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
//            0.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
//            1.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
//            0.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
//            1.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
//            1.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
//
//            0.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
//            0.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
//            1.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
//            0.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
//            1.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
//            1.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
//        };
//
//        globalUVs = {
//            (clipX + x) / xdimens,   (clipX + y) / ydimens, transparency,
//            (clipX + x) / xdimens,   (clipY + y) / ydimens, transparency,
//            (clipY + x) / xdimens,   (clipX + y) / ydimens, transparency,
//            (clipX + x) / xdimens,   (clipY + y) / ydimens, transparency,
//            (clipY + x) / xdimens,   (clipY + y) / ydimens, transparency,
//            (clipY + x) / xdimens,   (clipX + y) / ydimens, transparency,
//
//            (clipX + x) / xdimens,   (clipX + y) / ydimens, transparency,
//            (clipX + x) / xdimens,   (clipY + y) / ydimens, transparency,
//            (clipY + x) / xdimens,   (clipX + y) / ydimens, transparency,
//            (clipX + x) / xdimens,   (clipY + y) / ydimens, transparency,
//            (clipY + x) / xdimens,   (clipY + y) / ydimens, transparency,
//            (clipY + x) / xdimens,   (clipX + y) / ydimens, transparency
//        };
//
//        normals = {
//            0.0f, 0.0f, 0.0f,
//            0.0f, 0.0f, 0.0f,
//            0.0f, 0.0f, 0.0f,
//            0.0f, 0.0f, 0.0f,
//            0.0f, 0.0f, 0.0f,
//            0.0f, 0.0f, 0.0f,
//
//            0.0f, 0.0f, 0.0f,
//            0.0f, 0.0f, 0.0f,
//            0.0f, 0.0f, 0.0f,
//            0.0f, 0.0f, 0.0f,
//            0.0f, 0.0f, 0.0f,
//            0.0f, 0.0f, 0.0f
//        };
//    }
//    vector<GLfloat> finalVerts;
//    for (int i = 0; i < triangle.size() / 3; i++) {
//        finalVerts.push_back(triangle[3 * i + 0]);
//        finalVerts.push_back(triangle[3 * i + 1]);
//        finalVerts.push_back(triangle[3 * i + 2]);
//
//        finalVerts.push_back(globalUVs[3 * i + 0]);
//        finalVerts.push_back(globalUVs[3 * i + 1]);
//        finalVerts.push_back(globalUVs[3 * i + 2]);
//
//        finalVerts.push_back(normals[3 * i + 0]);
//        finalVerts.push_back(normals[3 * i + 1]);
//        finalVerts.push_back(normals[3 * i + 2]);
//    }
//    calcAverageNormals(finalVerts, indices1, 9, 6, indexOffset);
//    Block newBlock(vec3(xoffset, yoffset, zoffset), blockType, finalVerts, indices1);
//    addBlockToWorld(Block(vec3(xoffset, yoffset, zoffset), blockType, finalVerts, indices1));
//    //chunks.back().addBlock(Block(vec3(xoffset, yoffset, zoffset), blockType, finalVerts, indices1));
//    //chunks[chunks.size() - 1].blocks.push_back(Block(vec3(xoffset, yoffset, zoffset), blockType, finalVerts, indices1));
//}

void World::createCube(float x, float y, float z, Item blockType) {
    if (blockType == AIR) return;
    float UVs[7];
    getUVs(blockType, UVs);
    float xoffset = UVs[0],
        yoffset = UVs[1],
        xoffsetTop = UVs[2],
        yoffsetTop = UVs[3],
        xoffsetBottom = UVs[4],
        yoffsetBottom = UVs[5],
        transparency = 1.0f;

    float clipX = 0.03f, clipY = 0.97f;
    int num_of_faces = 6;

    unsigned int indexOffset = chunks.back().indexOffset;

    vector<GLfloat> vertices[6] = {
        {
            0.0f + x, 0.0f + y, 0.0f + z,
            0.0f + x, 1.0f + y, 0.0f + z,
            0.0f + x, 1.0f + y, 1.0f + z,
            0.0f + x, 0.0f + y, 1.0f + z
        },

        {
            1.0f + x, 0.0f + y, 0.0f + z,
            1.0f + x, 1.0f + y, 0.0f + z,
            1.0f + x, 1.0f + y, 1.0f + z,
            1.0f + x, 0.0f + y, 1.0f + z
        },

        {
            0.0f + x, 0.0f + y, 0.0f + z,
            0.0f + x, 1.0f + y, 0.0f + z,
            1.0f + x, 1.0f + y, 0.0f + z,
            1.0f + x, 0.0f + y, 0.0f + z
        },

        {
            0.0f + x, 0.0f + y, 1.0f + z,
            0.0f + x, 1.0f + y, 1.0f + z,
            1.0f + x, 1.0f + y, 1.0f + z,
            1.0f + x, 0.0f + y, 1.0f + z
        },

        {
            0.0f + x, 0.0f + y, 0.0f + z,
            0.0f + x, 0.0f + y, 1.0f + z,
            1.0f + x, 0.0f + y, 1.0f + z,
            1.0f + x, 0.0f + y, 0.0f + z
        },

        {
            0.0f + x, 1.0f + y, 0.0f + z,
            0.0f + x, 1.0f + y, 1.0f + z,
            1.0f + x, 1.0f + y, 1.0f + z,
            1.0f + x, 1.0f + y, 0.0f + z
        }
    };

    vector<unsigned int> indices[6] = {
        {0 + indexOffset,     1 + indexOffset,      2 + indexOffset,
        2 + indexOffset,     3 + indexOffset,      0 + indexOffset},

        {4 + indexOffset,     5 + indexOffset,      6 + indexOffset,
        6 + indexOffset,     7 + indexOffset,      4 + indexOffset},

        {8 + indexOffset,     9 + indexOffset,      10 + indexOffset,
        10 + indexOffset,    11 + indexOffset,     8 + indexOffset},

        {12 + indexOffset,     13 + indexOffset,      14 + indexOffset,
        14 + indexOffset,     15 + indexOffset,      12 + indexOffset},

        {16 + indexOffset,     17 + indexOffset,      18 + indexOffset,
        18 + indexOffset,     19 + indexOffset,      16 + indexOffset},

        {20 + indexOffset,     21 + indexOffset,      22 + indexOffset,
        22 + indexOffset,    23 + indexOffset,     20 + indexOffset},
    };

    vector<GLfloat> globalUVs[6];

    for (int i = 0; i < 6; i++) {
        int offsetX = 0, offsetY = 0;
        if (i == 4) { offsetX = xoffsetBottom; offsetY = yoffsetBottom; }
        else if (i == 5) { offsetX = xoffsetTop;  offsetY = yoffsetTop; }
        globalUVs[i].push_back((clipX + xoffset + offsetX) / xdimens); globalUVs[i].push_back((clipX + yoffset + offsetY) / ydimens); globalUVs[i].push_back(transparency);
        globalUVs[i].push_back((clipX + xoffset + offsetX) / xdimens); globalUVs[i].push_back((clipY + yoffset + offsetY) / ydimens); globalUVs[i].push_back(transparency);
        globalUVs[i].push_back((clipY + xoffset + offsetX) / xdimens); globalUVs[i].push_back((clipY + yoffset + offsetY) / ydimens); globalUVs[i].push_back(transparency);
        globalUVs[i].push_back((clipY + xoffset + offsetX) / xdimens); globalUVs[i].push_back((clipX + yoffset + offsetY) / ydimens); globalUVs[i].push_back(transparency);
    }

    vector<GLfloat> normals[6];

    for (int i = 0; i < 6; i++) {
        normals[i] = normalsLong[i];
    }

    if (blockType == GRASS || blockType == POPPY || blockType == BLUE_ORCHID) {
        num_of_faces = 2;
        for (int i = 0; i < 6; i++) {
            indices[i].clear();
        }

        indices[0] = {
            {0 + indexOffset,     1 + indexOffset,      2 + indexOffset,
            2 + indexOffset,     3 + indexOffset,      0 + indexOffset}
        };

        indices[1] = {
            {4 + indexOffset,     5 + indexOffset,      6 + indexOffset,
            6 + indexOffset,     7 + indexOffset,      4 + indexOffset}
        };

        for (int i = 0; i < 6; i++) {
            vertices[i].clear();
        }

        vertices[0] = {
            0.0f + x, 0.0f + y, 0.0f + z,
            0.0f + x, 1.0f + y, 0.0f + z,
            1.0f + x, 1.0f + y, 1.0f + z,
            1.0f + x, 0.0f + y, 1.0f + z
        };
        vertices[1] = {
            0.0f + x, 0.0f + y, 1.0f + z,
            0.0f + x, 1.0f + y, 1.0f + z,
            1.0f + x, 1.0f + y, 0.0f + z,
            1.0f + x, 0.0f + y, 0.0f + z
        };

        for (int i = 0; i < 6; i++) {
            globalUVs[i].clear();
        }
        for (int i = 0; i < 2; i++) {
            int offsetX = 0, offsetY = 0;
            globalUVs[i].push_back((clipX + xoffset + offsetX) / xdimens); globalUVs[i].push_back((clipX + yoffset + offsetY) / ydimens); globalUVs[i].push_back(transparency);
            globalUVs[i].push_back((clipX + xoffset + offsetX) / xdimens); globalUVs[i].push_back((clipY + yoffset + offsetY) / ydimens); globalUVs[i].push_back(transparency);
            globalUVs[i].push_back((clipY + xoffset + offsetX) / xdimens); globalUVs[i].push_back((clipY + yoffset + offsetY) / ydimens); globalUVs[i].push_back(transparency);
            globalUVs[i].push_back((clipY + xoffset + offsetX) / xdimens); globalUVs[i].push_back((clipX + yoffset + offsetY) / ydimens); globalUVs[i].push_back(transparency);
        }

        for (int i = 0; i < 6; i++) {
            normals[i] = normalsShort[i];
        }
    }

    vector<GLfloat> finalVertsFace[6];
    for (int k = 0; k < num_of_faces; k++) {
        for (int i = 0; i < vertices[k].size() / 3; i++) {
            finalVertsFace[k].push_back(vertices[k][3 * i + 0]);
            finalVertsFace[k].push_back(vertices[k][3 * i + 1]);
            finalVertsFace[k].push_back(vertices[k][3 * i + 2]);

            finalVertsFace[k].push_back(globalUVs[k][3 * i + 0]);
            finalVertsFace[k].push_back(globalUVs[k][3 * i + 1]);
            finalVertsFace[k].push_back(globalUVs[k][3 * i + 2]);

            finalVertsFace[k].push_back(normals[k][3 * i + 0]);
            finalVertsFace[k].push_back(normals[k][3 * i + 1]);
            finalVertsFace[k].push_back(normals[k][3 * i + 2]);
        }
    }

    vector<GLfloat> finalVerts;
    vector<unsigned int> finalInds;
    for (int i = 0; i < num_of_faces; i++) {
        finalVerts.insert(finalVerts.end(), finalVertsFace[i].begin(), finalVertsFace[i].end());
        finalInds.insert(finalInds.end(), indices[i].begin(), indices[i].end());
    }

    chunks.back().vertices.insert(chunks.back().vertices.end(), finalVerts.begin(), finalVerts.end());
    chunks.back().indices.insert(chunks.back().indices.end(), finalInds.begin(), finalInds.end());

    Block newBlock(vec3(x, y, z), blockType, finalVerts, finalInds);

    chunks.back().indexOffset += finalVerts.size() / 9;
    chunks.back().needUpdate = true;

    chunks.back().addBlock(newBlock);
}

void World::delBlocklook_at() {
    ivec3 blockPos = lookingAtBlock();
    if (!recipe.isBreakable(worldBlocks[blockPos].type) || blockPos.x == 404.0f) {
        return;
    }
    deleteBlockFromWorld(blockPos);
}

void World::addBlocklook_at(Item blockType) {
    if (recipe.isTool(blockType)) {
        return;
    }
    glm::vec3 rayDir;
    glm::vec3 rayOrigin = camera.getCameraPos();
    rayDir = normalize(camera.getCameraFront());

    float maxDistance = 15.0f;
    float stepSize = 0.2f;
    for (float t = 0.0f; t <= maxDistance; t += stepSize) {
        glm::vec3 point = rayOrigin + rayDir * t;
        glm::ivec3 blockPos = glm::floor(point);
        //cout << "block at " << floor(blockPos.x) << ", " << floor(blockPos.y) << ", " << floor(blockPos.z) << " ? : " << blockExistsAt(blockPos) << endl;
        if (blockExistsAt(blockPos)) {
            glm::vec3 placementPos = glm::floor(point - rayDir * stepSize); // just before the hit
            createCube(placementPos.x, placementPos.y, placementPos.z, blockType);
            if (!inventory.inf_blocks) {
                inventory.inv_slots[3][slot] = AIR;
                inventory.currInvSlot[3][slot] = Mesh();
            }
            //cout << blockType << endl;
            return;
        }
    }
}

void generateChunkAt(vec2 xyChunk) {
    world.chunks.push_back(Chunk());
    float xoffsetTop = 0.0f;
    Item blockType = AIR;
    std::random_device rd;
    std::mt19937 gen(rd()); // Mersenne Twister engine

    int blockTypes1[] = {
        STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, IRON_ORE.id
    };
    int blockTypes2[] = {
        1, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, STONE_BLOCK.id, IRON_ORE.id, IRON_ORE.id
    };
    int blockTypes3[] = {
        DIRT_BLOCK.id, STONE_BLOCK.id
    };
    std::uniform_int_distribution<> dist(0, sizeof(blockTypes1) / sizeof(int) - 1);
    std::uniform_int_distribution<> dist2(0, sizeof(blockTypes2) / sizeof(int) - 1);
    std::uniform_int_distribution<> dist3(0, sizeof(blockTypes3) / sizeof(int) - 1);

    for (int x = (xyChunk.x) * CHUNK_SIZE; x < (xyChunk.x + 1) * (CHUNK_SIZE); x++) {
        for (int z = (xyChunk.y) * CHUNK_SIZE; z < (xyChunk.y + 1) * (CHUNK_SIZE); z++) {
            /*float total = 0;
            int samples = 5000;
            for (int i = 0; i < samples; i++) {
                total += static_cast<float>(rand()) / RAND_MAX;
            }
            float totalrand = total / samples;*/
            FastNoiseLite noise;
            noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
            float height = noise.GetNoise((float)x, (float)z); // returns value in range [-1, 1]
            float scaledHeight = (height + 1.0f) * 0.65 * CHUNK_SIZE + 5;

            for (int y = 0; y < scaledHeight; y++) {
                if (y > scaledHeight - 2) {
                    blockType = GRASS_BLOCK;
                }
                else if (y > scaledHeight - 4 && y <= scaledHeight - 2) {
                    blockType = DIRT_BLOCK;
                }
                else if (y <= scaledHeight - 4 && y > scaledHeight - 7) {
                    blockType = items[blockTypes3[dist3(gen)]];
                }
                else if (y <= scaledHeight - 7 && y > scaledHeight - 20) {
                    blockType = items[blockTypes1[dist(gen)]];
                }
                else if (y <= scaledHeight - 20) {
                    blockType = items[blockTypes2[dist2(gen)]];
                }

                world.createCube((1.0f + xoffsetTop) * (float)x, (1.0f + xoffsetTop) * (float)y - 1, (1.0f + xoffsetTop) * (float)z, blockType);
            }
        }
    }

}