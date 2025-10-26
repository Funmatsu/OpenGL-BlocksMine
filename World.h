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
    //vector<Chunk> chunks;
    unordered_map<glm::ivec2, Chunk, ivec2_hash, ivec2_eq> chunkData;

    Block getBlockAt(vec3 blockPos);

    void addChunk(Chunk newChunk, ivec2 xyChunk);

    void updateChunk(const ivec2& chunkCoord);

    Mesh createMeshCube(float x, float y, float z, float scale, Item blockType);

    Block createMeshCube(vec3 blockPos, float scale, Item blockType);

    void createCube(vec3 blockPos, Item blockType);

    void deleteBlockFromWorld(vec3 blockPos);

    void delBlocklook_at();

    void addBlocklook_at(Item blockType);
};

World world;
bool inv_change;
int slot = 0;
float slotX = 0.0f, slotY = 0.0f;



//.cpp part-----------------------------------------------------------------------------------------------------------------------

Block World::getBlockAt(vec3 blockPos) {
    ivec2 chunkPos = ivec2(blockPos.x / CHUNK_SIZE, blockPos.z / CHUNK_SIZE);
    return Block(blockPos, world.chunkData[chunkPos].blockData[blockPos].blockType, {}, {});
}

bool blockExistsAt(ivec3 blockPos) {
    if (world.chunkData.empty()) return false;

    ivec2 chunkCoord(blockPos.x / CHUNK_SIZE, blockPos.z / CHUNK_SIZE);
    auto chunkIt = world.chunkData.find(chunkCoord);
    if (chunkIt == world.chunkData.end()) return false;

    const auto& blockMap = chunkIt->second.blockData;
    return (blockMap.find(blockPos) != blockMap.end());
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
        ivec2 chunkPos = ivec2(blockPos.x / CHUNK_SIZE, blockPos.z / CHUNK_SIZE);
        if (blockExistsAt(blockPos) &&
            world.chunkData[chunkPos].blockData[blockPos].blockType != AIR) {
            return blockPos;
        }
    }
    return vec3(-404.0f);
}

void World::addChunk(Chunk newChunk, ivec2 xyChunk) {
    chunkData[ivec2(xyChunk)] = newChunk;
}

//void World::addBlockToWorld(vec3 position, vector<float> vertices, vector<unsigned int> indices, int vertNum, int indNum, Item blockType) {
//    for (int i = 0; i < vertNum; i++) {
//        chunks[chunks.size() - 1].vertices.push_back(vertices[i]);
//    }
//    for (int i = 0; i < indNum; i++) {
//        chunks[chunks.size() - 1].indices.push_back(indices[i]);
//    }
//    chunks[chunks.size() - 1].needUpdate = true;
//    Block newBlock(position, blockType, vertices, indices);
//    chunks.back().blocks.push_back(newBlock);
//    chunks.back().addBlock(newBlock);
//}

//void World::addBlockToWorld(Block block) {
//    chunks.back().vertices.insert(chunks.back().vertices.end(), block.vertices.begin(), block.vertices.end());
//    chunks.back().indices.insert(chunks.back().indices.end(), block.indices.begin(), block.indices.end());
//    chunks.back().addBlock(block);
//    chunks.back().needUpdate = true;
//}

//void World::deleteBlockFromWorld(vec3 blockPos) {
//    for (int k = 0; k < chunks.size(); k++) {
//        int blockVertNums = 0;
//        bool blockBroken = false;
//        for (int i = 0; i < chunks[k].blocks.size(); i++) {
//            if (chunks[k].blocks[i].position == blockPos) {
//
//                for (int m = 0; m < 4; m++) {
//                    for (int l = 0; l < 9; l++) {
//                        if (inventory.inv_slots[3 - m][l] == AIR) {
//                            inventory.inv_slots[3 - m][l] = chunks[k].blocks[i].type;
//                            inv_change = true;
//                            blockBroken = true;
//                            break;
//                        }
//                    }
//                    if (blockBroken) {
//                        break;
//                    }
//                }
//
//                chunks[k].vertices.erase(chunks[k].vertices.begin() + blockVertNums, chunks[k].vertices.begin() + blockVertNums + chunks[k].blocks[i].vertices.size());
//                chunks[k].blocks.erase(chunks[k].blocks.begin() + i);
//
//                chunks[k].needUpdate = true;
//                return;
//            }
//            blockVertNums += chunks[k].blocks[i].vertices.size();
//        }
//    }
//    worldBlocks.erase(ivec3(floor(blockPos)));
//}

//void World::deleteBlockFromWorld(vec3 blockPos) {
//    for (int k = 0; k < chunks.size(); k++) {
//        if (chunks[k].blockData.find(blockPos) != chunks[k].blockData.end()) {
//            chunks[k].blockData[blockPos].blockType = AIR;
//            chunks[k].needUpdate = true;
//
//            return;
//        }
//    }
//}

void World::deleteBlockFromWorld(vec3 blockPos) {
    ivec2 chunkPos = ivec2(blockPos.x / CHUNK_SIZE, blockPos.z / CHUNK_SIZE);
    if (chunkData[chunkPos].blockData[blockPos].blockType.isBreakable) {
        bool blockBroken = false;
        for (int m = 0; m < 4; m++) {
            for (int l = 0; l < 9; l++) {
                if (inventory.inv_slots[3 - m][l] == AIR) {
                    inventory.inv_slots[3 - m][l] = chunkData[chunkPos].blockData[blockPos].blockType;
                    inv_change = true;
                    blockBroken = true;
                    break;
                }
            }
            if (blockBroken) {
                break;
            }
        }
        chunkData[chunkPos].blockData[blockPos].blockType = AIR;

        updateChunk(chunkPos);
    }
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
    float UVs[7] = { 1, 0, 0, 0, 0, 0, 1 };
    if (!blockType.isFlat)
        getUVs(blockType, UVs);
    float xoffset           = UVs[0],
          yoffset           = UVs[1],
          xoffsetTop        = UVs[2],
          yoffsetTop        = UVs[3],
          xoffsetBottom     = UVs[4],
          yoffsetBottom     = UVs[5],
          transparency      = UVs[6];
    
    float clipX = 0.03f, clipY = 0.97f;

    vector<unsigned int> indices = {
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
    vector<GLfloat> globalUVs;

    for (int i = 0; i < 6; i++) {
        int offsetX = 0, offsetY = 0;
        if (i == 4) { offsetX = xoffsetBottom; offsetY = yoffsetBottom; }
        else if (i == 5) { offsetX = xoffsetTop;  offsetY = yoffsetTop; }
        globalUVs.push_back((clipX + xoffset + offsetX) / xdimens); globalUVs.push_back((clipX + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
        globalUVs.push_back((clipY + xoffset + offsetX) / xdimens); globalUVs.push_back((clipX + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
        globalUVs.push_back((clipY + xoffset + offsetX) / xdimens); globalUVs.push_back((clipY + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
        globalUVs.push_back((clipX + xoffset + offsetX) / xdimens); globalUVs.push_back((clipY + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
    }

    vector<GLfloat> triangle = {
            0.0f + blockPos.x + 0.1f * -scale,   0.0f + blockPos.y + 0.1f * -scale,    0.0f + blockPos.z + 0.1f * -scale,
            0.0f + blockPos.x + 0.1f * -scale,   1.0f + blockPos.y + 0.1f * scale,     0.0f + blockPos.z + 0.1f * -scale,
            0.0f + blockPos.x + 0.1f * -scale,   1.0f + blockPos.y + 0.1f * scale,     1.0f + blockPos.z + 0.1f * scale,
            0.0f + blockPos.x + 0.1f * -scale,   0.0f + blockPos.y + 0.1f * -scale,    1.0f + blockPos.z + 0.1f * scale,
                                                 
            1.0f + blockPos.x + 0.1f * scale,    0.0f + blockPos.y + 0.1f * -scale,    0.0f + blockPos.z + 0.1f * -scale,
            1.0f + blockPos.x + 0.1f * scale,    1.0f + blockPos.y + 0.1f * scale,     0.0f + blockPos.z + 0.1f * -scale,
            1.0f + blockPos.x + 0.1f * scale,    1.0f + blockPos.y + 0.1f * scale,     1.0f + blockPos.z + 0.1f * scale,
            1.0f + blockPos.x + 0.1f * scale,    0.0f + blockPos.y + 0.1f * -scale,    1.0f + blockPos.z + 0.1f * scale,
                                                 
            0.0f + blockPos.x + 0.1f * -scale,   0.0f + blockPos.y + 0.1f * -scale,    0.0f + blockPos.z + 0.1f * -scale,
            1.0f + blockPos.x + 0.1f * scale,    0.0f + blockPos.y + 0.1f * -scale,    0.0f + blockPos.z + 0.1f * -scale,
            1.0f + blockPos.x + 0.1f * scale,    1.0f + blockPos.y + 0.1f * scale,     0.0f + blockPos.z + 0.1f * -scale,
            0.0f + blockPos.x + 0.1f * -scale,   1.0f + blockPos.y + 0.1f * scale,     0.0f + blockPos.z + 0.1f * -scale,
                                                 
            0.0f + blockPos.x + 0.1f * -scale,   0.0f + blockPos.y + 0.1f * -scale,    1.0f + blockPos.z + 0.1f * scale,
            1.0f + blockPos.x + 0.1f * scale,    0.0f + blockPos.y + 0.1f * -scale,    1.0f + blockPos.z + 0.1f * scale,
            1.0f + blockPos.x + 0.1f * scale,    1.0f + blockPos.y + 0.1f * scale,     1.0f + blockPos.z + 0.1f * scale,
            0.0f + blockPos.x + 0.1f * -scale,   1.0f + blockPos.y + 0.1f * scale,     1.0f + blockPos.z + 0.1f * scale,
                                                 
            0.0f + blockPos.x + 0.1f * -scale,   0.0f + blockPos.y + 0.1f * -scale,    0.0f + blockPos.z + 0.1f * -scale,
            1.0f + blockPos.x + 0.1f * scale,    0.0f + blockPos.y + 0.1f * -scale,    0.0f + blockPos.z + 0.1f * -scale,
            1.0f + blockPos.x + 0.1f * scale,    0.0f + blockPos.y + 0.1f * -scale,    1.0f + blockPos.z + 0.1f * scale,
            0.0f + blockPos.x + 0.1f * -scale,   0.0f + blockPos.y + 0.1f * -scale,    1.0f + blockPos.z + 0.1f * scale,
                                                 
            0.0f + blockPos.x + 0.1f * -scale,   1.0f + blockPos.y + 0.1f * scale,     0.0f + blockPos.z + 0.1f * -scale,
            1.0f + blockPos.x + 0.1f * scale,    1.0f + blockPos.y + 0.1f * scale,     0.0f + blockPos.z + 0.1f * -scale,
            1.0f + blockPos.x + 0.1f * scale,    1.0f + blockPos.y + 0.1f * scale,     1.0f + blockPos.z + 0.1f * scale,
            0.0f + blockPos.x + 0.1f * -scale,   1.0f + blockPos.y + 0.1f * scale,     1.0f + blockPos.z + 0.1f * scale
    };

    vector<GLfloat> normals = long_normals;

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
    returnBlock.blockMesh.createMesh(finalVerts, indices, finalVerts.size(), indices.size());
    return returnBlock;
}

void World::createCube(vec3 blockPos, Item blockType) {
    ivec2 chunkPos = ivec2(blockPos.x / CHUNK_SIZE, blockPos.z / CHUNK_SIZE);
    chunkData[chunkPos].blockData[blockPos] = blockData(blockPos, blockType);
    updateChunk(chunkPos);
}

bool isAir(Item item);

bool shouldEmitFace(vec2 xyChunk, Chunk& cd, int x, int y, int z, int dx, int dy, int dz);

void emitFace(Mesh& m, int baseX, int baseY, int baseZ,
    int face, Item blockType, float x, float y, float z);

void meshChunk(vec2 xyChunk, Chunk& cd, Mesh& out);

void generateChunkAt(vec2 xyChunk, Chunk& repChunk);

void regenerateChunk(vec2 xyChunk, Chunk& repChunk);

void World::updateChunk(const ivec2& chunkCoord) {
    auto it = chunkData.find(chunkCoord);
    if (it == chunkData.end()) return; // Chunk doesn't exist

    Chunk& chunk = it->second;

    //// Clear old mesh data
    chunk.mesh.verts.clear();
    chunk.mesh.inds.clear();

    // Rebuild mesh from current blockData
    meshChunk(chunkCoord, chunk, chunk.mesh);

    //// Update vertex/index buffers
    chunk.vertices.clear();
    chunk.indices.clear();

    chunk.vertices.insert(chunk.vertices.end(), chunk.mesh.verts.begin(), chunk.mesh.verts.end());

    uint32_t base = 0;
    for (auto idx : chunk.mesh.inds) {
        chunk.indices.push_back(base + idx);
    }

    chunk.indexOffset = static_cast<uint32_t>(chunk.mesh.verts.size() / 9);
    chunk.needUpdate = true;
}

void World::delBlocklook_at() {
    ivec3 blockPos = lookingAtBlock();
    if (!recipe.isBreakable(world.chunkData[ivec2(blockPos.x / CHUNK_SIZE, blockPos.z / CHUNK_SIZE)].blockData[blockPos].blockType) || blockPos.x == 404.0f) {
        return;
    }
    deleteBlockFromWorld(blockPos);
}

void World::addBlocklook_at(Item blockType) {
    if (recipe.isTool(blockType)) {
        return;
    }
    ivec3 blockPos = vec3(0.0f);
    glm::vec3 rayDir;
    glm::vec3 rayOrigin = camera.getCameraPos();
    rayDir = normalize(camera.getCameraFront());

    float maxDistance = 7.0f;
    float stepSize = 0.2f;
    for (float t = 0.0f; t < maxDistance; t += stepSize) {
        glm::vec3 point = rayOrigin + rayDir * t;
        blockPos = glm::floor(point);
        ivec2 chunkPos = ivec2(blockPos.x / CHUNK_SIZE, blockPos.z / CHUNK_SIZE);
        if (blockExistsAt(blockPos) &&
            world.chunkData[chunkPos].blockData[blockPos].blockType != AIR &&
            blockType.isPlaceable) {
            createCube(floor(point - rayDir * stepSize), blockType);
        }
    }
}