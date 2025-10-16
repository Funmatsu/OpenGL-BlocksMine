#pragma once
#include "libraries.h"
#include "Chunk.h"
#include "normals.h"
#include "inventory.h"

vector<vec2> chunkCoords;

std::queue<vec2> chunkRequestQueue;
std::mutex chunkRequestMutex;

std::queue<Chunk> chunkResultQueue;
std::mutex chunkResultMutex;

std::queue<vec2> chunkRequestQueue2;
std::mutex chunkRequestMutex2;

std::queue<Chunk> chunkResultQueue2;
std::mutex chunkResultMutex2;

std::queue<vec2> chunkRequestQueue3;
std::mutex chunkRequestMutex3;

std::queue<Chunk> chunkResultQueue3;
std::mutex chunkResultMutex3;

std::queue<Chunk> breakResQueue;
std::mutex breakResMutex;

std::queue<vec3> breakReqQueue;
std::mutex breakReqMutex;

std::queue<vec3> placeReqQueue;
std::mutex placeReqMutex;

std::queue<vec3> placeResQueue;
std::mutex placeResMutex;

std::atomic<bool> chunkGenRunning = true;
std::atomic<bool> chunkGenRunning2 = true;
std::atomic<bool> chunkGenRunning3 = true;
std::atomic<bool> blockBreaking = true;
std::atomic<bool> blockPlacing = true;
bool blockBreakingOut = false;
bool blockPlacingOut = false;

vector<GLfloat> normalsLong[6] = {
    {
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f
        },
{
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f
            },
{
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f
            },
{
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f
            },
{
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f
            },
{
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f
            }
};

vector<GLfloat> normalsShort[6] = {
    {
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f
        },
{
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f
            },
};





//void createCubeInThread(float x, float y, float z, Chunk& repChunk, int blockType, int dummy, int dummy_dummy) {
//    float xoffset = 0, yoffset = 1, xoffsetTop = 0, yoffsetTop = 0, xoffsetBottom = 0, yoffsetBottom = 0, transparency = 1.0f;
//
//    int arbitraryoff = 0;
//    if (blockType == GRASS_BLOCK) {
//        xoffset = 0, yoffset = 2; xoffsetTop = 2; yoffsetTop = 0; xoffsetBottom = 2; yoffsetBottom = 2;
//    }
//
//    if (blockType == IRON_ORE) {
//        xoffset = 1; yoffset = 1 + arbitraryoff;
//    }
//
//    if (blockType == STONE_BLOCK) {
//        xoffset = 1; yoffset = 2;
//    }
//
//    if (blockType == DIRT_BLOCK) {
//        xoffset = 2; yoffset = 0;
//    }
//
//    if (blockType == OAK_WOOD) {
//        xoffset = 2, yoffset = 1; xoffsetTop = -2; yoffsetTop = 2; xoffsetBottom = -2; yoffsetBottom = 2;
//    }
//
//    if (blockType == CLOUD) {
//        xoffset = 1, yoffset = 0, transparency = 0.95f;
//    }
//
//    if (blockType == OAK_PLANK) {
//        xoffset = 3, yoffset = 0;
//    }
//
//    if (blockType == OAK_LEAVES) {
//        xoffset = 0, yoffset = 0;
//    }
//
//    if (blockType == GRASS) {
//        xoffset = 3, yoffset = 1; xoffsetTop = 0, xoffsetBottom = 0; yoffsetTop = 1, yoffsetBottom = 1;
//    }
//
//    if (blockType == POPPY) {
//        xoffset = 3, yoffset = 2; xoffsetTop = 0, xoffsetBottom = 0; yoffsetTop = 1, yoffsetBottom = 1;
//    }
//
//    float clipX = 0.03f, clipY = 0.97f;
//
//    unsigned int indexOffset = 0;
//    for (int i = 0; i < repChunk.blocks.size(); i++) {
//        if (repChunk.blocks.size() > 0) {
//            indexOffset += repChunk.blocks[i].indices.size();
//        }
//    }
//    vector<unsigned int> indices = {
//        0 + (indexOffset), 1 + (indexOffset), 2 + (indexOffset),
//        3 + (indexOffset), 4 + (indexOffset), 5 + (indexOffset),
//        6 + (indexOffset), 7 + (indexOffset), 8 + (indexOffset),
//        9 + (indexOffset), 10 + (indexOffset), 11 + (indexOffset),
//        12 + (indexOffset), 13 + (indexOffset), 14 + (indexOffset),
//        15 + (indexOffset), 16 + (indexOffset), 17 + (indexOffset),
//        18 + (indexOffset), 19 + (indexOffset), 20 + (indexOffset),
//        21 + (indexOffset), 22 + (indexOffset), 23 + (indexOffset),
//        24 + (indexOffset), 25 + (indexOffset), 26 + (indexOffset),
//        27 + (indexOffset), 28 + (indexOffset), 29 + (indexOffset),
//        30 + (indexOffset), 31 + (indexOffset), 32 + (indexOffset),
//        33 + (indexOffset), 34 + (indexOffset), 35 + (indexOffset)
//    };
//
//    vector<unsigned int> blockIndices = {
//    0, 1, 2,
//    3, 4, 5,
//    6, 7, 8,
//    9, 10, 11,
//    12, 13, 14,
//    15, 16, 17,
//    18, 19, 20,
//    21, 22, 23,
//    24, 25, 26,
//    27, 28, 29,
//    30, 31, 32,
//    33, 34, 35
//    };
//
//    vector<GLfloat> globalUVs =
//    {
//        (clipX + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
//        (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
//        (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
//        (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
//        (clipY + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
//        (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
//
//        (clipX + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
//        (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
//        (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
//        (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
//        (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
//        (clipY + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
//
//        (clipX + xoffset + xoffsetBottom) / xdimens,   (clipX + yoffset + yoffsetBottom) / ydimens, transparency,
//        (clipY + xoffset + xoffsetBottom) / xdimens,   (clipX + yoffset + yoffsetBottom) / ydimens, transparency,
//        (clipX + xoffset + xoffsetBottom) / xdimens,   (clipY + yoffset + yoffsetBottom) / ydimens, transparency,
//        (clipX + xoffset + xoffsetBottom) / xdimens,   (clipY + yoffset + yoffsetBottom) / ydimens, transparency,
//        (clipY + xoffset + xoffsetBottom) / xdimens,   (clipX + yoffset + yoffsetBottom) / ydimens, transparency,
//        (clipY + xoffset + xoffsetBottom) / xdimens,   (clipY + yoffset + yoffsetBottom) / ydimens, transparency,
//
//        (clipX + xoffset + xoffsetTop) / xdimens,   (clipX + yoffset + yoffsetTop) / ydimens, transparency,
//        (clipX + xoffset + xoffsetTop) / xdimens,   (clipY + yoffset + yoffsetTop) / ydimens, transparency,
//        (clipY + xoffset + xoffsetTop) / xdimens,   (clipX + yoffset + yoffsetTop) / ydimens, transparency,
//        (clipX + xoffset + xoffsetTop) / xdimens,   (clipY + yoffset + yoffsetTop) / ydimens, transparency,
//        (clipY + xoffset + xoffsetTop) / xdimens,   (clipY + yoffset + yoffsetTop) / ydimens, transparency,
//        (clipY + xoffset + xoffsetTop) / xdimens,   (clipX + yoffset + yoffsetTop) / ydimens, transparency,
//
//        (clipX + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
//        (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
//        (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
//        (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
//        (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
//        (clipY + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
//
//        (clipX + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
//        (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
//        (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
//        (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
//        (clipY + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
//        (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency
//    };
//
//    vector<GLfloat> triangle = {
//        0.0f + x, 0.0f + y, 0.0f + z,
//        0.0f + x, 1.0f + y, 0.0f + z,
//        0.0f + x, 0.0f + y, 1.0f + z,
//        0.0f + x, 1.0f + y, 0.0f + z,
//        0.0f + x, 1.0f + y, 1.0f + z,
//        0.0f + x, 0.0f + y, 1.0f + z,
//
//        1.0f + x, 0.0f + y, 0.0f + z,
//        1.0f + x, 0.0f + y, 1.0f + z,
//        1.0f + x, 1.0f + y, 0.0f + z,
//        1.0f + x, 1.0f + y, 0.0f + z,
//        1.0f + x, 0.0f + y, 1.0f + z,
//        1.0f + x, 1.0f + y, 1.0f + z,
//
//        0.0f + x, 0.0f + y, 0.0f + z,
//        0.0f + x, 0.0f + y, 1.0f + z,
//        1.0f + x, 0.0f + y, 0.0f + z,
//        1.0f + x, 0.0f + y, 0.0f + z,
//        0.0f + x, 0.0f + y, 1.0f + z,
//        1.0f + x, 0.0f + y, 1.0f + z,
//
//        0.0f + x, 1.0f + y, 0.0f + z,
//        1.0f + x, 1.0f + y, 0.0f + z,
//        0.0f + x, 1.0f + y, 1.0f + z,
//        1.0f + x, 1.0f + y, 0.0f + z,
//        1.0f + x, 1.0f + y, 1.0f + z,
//        0.0f + x, 1.0f + y, 1.0f + z,
//
//        0.0f + x, 0.0f + y, 0.0f + z,
//        1.0f + x, 0.0f + y, 0.0f + z,
//        0.0f + x, 1.0f + y, 0.0f + z,
//        0.0f + x, 1.0f + y, 0.0f + z,
//        1.0f + x, 0.0f + y, 0.0f + z,
//        1.0f + x, 1.0f + y, 0.0f + z,
//
//        0.0f + x, 0.0f + y, 1.0f + z,
//        0.0f + x, 1.0f + y, 1.0f + z,
//        1.0f + x, 0.0f + y, 1.0f + z,
//        0.0f + x, 1.0f + y, 1.0f + z,
//        1.0f + x, 1.0f + y, 1.0f + z,
//        1.0f + x, 0.0f + y, 1.0f + z
//    };
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
//    if (blockType == GRASS || blockType == POPPY) {
//        indices = {
//            0 + indexOffset, 1 + indexOffset, 2 + indexOffset,
//            3 + indexOffset, 4 + indexOffset, 5 + indexOffset,
//            6 + indexOffset, 7 + indexOffset, 8 + indexOffset,
//            9 + indexOffset, 10 + indexOffset, 11 + indexOffset
//        };
//
//        blockIndices = {
//            0, 1, 2,
//            3, 4, 5,
//            6, 7, 8,
//            9, 10, 11,
//        };
//
//        triangle = {
//            0.0f + x, 0.0f + y, 0.0f + z,
//            0.0f + x, 1.0f + y, 0.0f + z,
//            1.0f + x, 0.0f + y, 1.0f + z,
//            0.0f + x, 1.0f + y, 0.0f + z,
//            1.0f + x, 1.0f + y, 1.0f + z,
//            1.0f + x, 0.0f + y, 1.0f + z,
//
//            0.0f + x, 0.0f + y, 1.0f + z,
//            0.0f + x, 1.0f + y, 1.0f + z,
//            1.0f + x, 0.0f + y, 0.0f + z,
//            0.0f + x, 1.0f + y, 1.0f + z,
//            1.0f + x, 1.0f + y, 0.0f + z,
//            1.0f + x, 0.0f + y, 0.0f + z,
//        };
//
//
//        globalUVs = {
//            (clipX + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
//            (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
//            (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
//            (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
//            (clipY + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
//            (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
//
//            (clipX + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
//            (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
//            (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
//            (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
//            (clipY + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
//            (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency
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
//
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
//    calcAverageNormals(finalVerts, indices, 9, 6, indexOffset);
//    //cout << finalVerts[6] << endl;
//    for (int i = 0; i < finalVerts.size(); i++) {
//        repChunk.vertices.push_back(finalVerts[i]);
//    }
//    for (int i = 0; i < indices.size(); i++) {
//        repChunk.indices.push_back(indices[i]);
//    }
//
//    repChunk.needUpdate = true;
//
//    Block newBlock(vec3(x, y, z), blockType, finalVerts, blockIndices);
//    //newBlock.blockMesh.createMesh(finalVerts, indices, finalVerts.size(), indices.size());
//    repChunk.addBlock(newBlock);
//    //cout << newBlock.position.x << " " << newBlock.position.y << " " << newBlock.position.z << endl;
//    repChunk.blockNum++;
//}
//
//void createCubeInThread(float x, float y, float z, Chunk& repChunk, int blockType, int dummy) {
//    float xoffset = 0, yoffset = 1, xoffsetTop = 0, yoffsetTop = 0, xoffsetBottom = 0, yoffsetBottom = 0, transparency = 1.0f;
//
//    int arbitraryoff = 0;
//    if (blockType == GRASS_BLOCK) {
//        xoffset = 0, yoffset = 2; xoffsetTop = 2; yoffsetTop = 0; xoffsetBottom = 2; yoffsetBottom = 2;
//    }
//
//    else if (blockType == IRON_ORE) {
//        xoffset = 1; yoffset = 1 + arbitraryoff;
//    }
//
//    else if (blockType == STONE_BLOCK) {
//        xoffset = 1; yoffset = 2;
//    }
//
//    else if (blockType == DIRT_BLOCK) {
//        xoffset = 2; yoffset = 0;
//    }
//
//    else if (blockType == OAK_WOOD) {
//        xoffset = 2, yoffset = 1; xoffsetTop = -2; yoffsetTop = 2; xoffsetBottom = -2; yoffsetBottom = 2;
//    }
//
//    else if (blockType == CLOUD) {
//        xoffset = 1, yoffset = 0, transparency = 0.95f;
//    }
//
//    else if (blockType == OAK_PLANK) {
//        xoffset = 3, yoffset = 0;
//    }
//
//    else if (blockType == OAK_LEAVES) {
//        xoffset = 0, yoffset = 0;
//    }
//
//    else if (blockType == GRASS) {
//        xoffset = 3, yoffset = 1; xoffsetTop = 0, xoffsetBottom = 0; yoffsetTop = 1, yoffsetBottom = 1;
//    }
//
//    else if (blockType == POPPY) {
//        xoffset = 3, yoffset = 2; xoffsetTop = 0, xoffsetBottom = 0; yoffsetTop = 1, yoffsetBottom = 1;
//    }
//
//    float clipX = 0.03f, clipY = 0.97f;
//
//    unsigned int indexOffset = 0;
//    for (int i = 0; i < repChunk.blocks.size(); i++) {
//        if (repChunk.blocks.size() > 0) {
//            indexOffset += repChunk.blocks[i].indices.size();
//        }
//    }
//    vector<unsigned int> indices;
//    indices.reserve(36);
//
//    for (int i = 0; i < 36; i++) {
//        indices.push_back(i + indexOffset);
//    }
//
//    vector<unsigned int> blockIndices;
//    blockIndices.reserve(36);
//
//    for (int i = 0; i < 36; i++) {
//        blockIndices.push_back(i);
//    }
//
//    vector<GLfloat> globalUVs;
//    globalUVs.reserve(36);
//
//    for (int i = 0; i < 6; i++) {
//        int offsetX = 0, offsetY = 0;
//        if (i == 4) { offsetX = xoffsetTop; offsetY = yoffsetTop; }
//        else if (i == 5) { offsetX = xoffsetBottom;  offsetY = yoffsetTop; }
//        globalUVs.push_back((clipX + xoffset + offsetX) / xdimens); globalUVs.push_back((clipX + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
//        globalUVs.push_back((clipX + xoffset + offsetX) / xdimens); globalUVs.push_back((clipY + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
//        globalUVs.push_back((clipY + xoffset + offsetX) / xdimens); globalUVs.push_back((clipX + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
//        globalUVs.push_back((clipX + xoffset + offsetX) / xdimens); globalUVs.push_back((clipY + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
//        globalUVs.push_back((clipY + xoffset + offsetX) / xdimens); globalUVs.push_back((clipY + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
//        globalUVs.push_back((clipY + xoffset + offsetX) / xdimens); globalUVs.push_back((clipX + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
//    }
//
//    vector<GLfloat> triangle = {
//        0.0f + x, 0.0f + y, 0.0f + z,
//        0.0f + x, 1.0f + y, 0.0f + z,
//        0.0f + x, 0.0f + y, 1.0f + z,
//        0.0f + x, 1.0f + y, 0.0f + z,
//        0.0f + x, 1.0f + y, 1.0f + z,
//        0.0f + x, 0.0f + y, 1.0f + z,
//
//        1.0f + x, 0.0f + y, 0.0f + z,
//        1.0f + x, 1.0f + y, 0.0f + z,
//        1.0f + x, 0.0f + y, 1.0f + z,
//        1.0f + x, 1.0f + y, 0.0f + z,
//        1.0f + x, 1.0f + y, 1.0f + z,
//        1.0f + x, 0.0f + y, 1.0f + z,
//
//        0.0f + x, 0.0f + y, 0.0f + z,
//        0.0f + x, 1.0f + y, 0.0f + z,
//        1.0f + x, 0.0f + y, 0.0f + z,        
//        0.0f + x, 1.0f + y, 0.0f + z,
//        1.0f + x, 1.0f + y, 0.0f + z,
//        1.0f + x, 0.0f + y, 0.0f + z,
//        
//        0.0f + x, 0.0f + y, 1.0f + z,
//        0.0f + x, 1.0f + y, 1.0f + z,
//        1.0f + x, 0.0f + y, 1.0f + z,
//        0.0f + x, 1.0f + y, 1.0f + z,
//        1.0f + x, 1.0f + y, 1.0f + z,
//        1.0f + x, 0.0f + y, 1.0f + z,
//
//        0.0f + x, 0.0f + y, 0.0f + z,
//        1.0f + x, 0.0f + y, 0.0f + z,
//        0.0f + x, 0.0f + y, 1.0f + z,
//        1.0f + x, 0.0f + y, 0.0f + z,
//        1.0f + x, 0.0f + y, 1.0f + z,
//        0.0f + x, 0.0f + y, 1.0f + z,
//
//        0.0f + x, 1.0f + y, 0.0f + z,
//        1.0f + x, 1.0f + y, 0.0f + z,
//        0.0f + x, 1.0f + y, 1.0f + z,
//        1.0f + x, 1.0f + y, 0.0f + z,
//        1.0f + x, 1.0f + y, 1.0f + z,
//        0.0f + x, 1.0f + y, 1.0f + z,
//    };
//    vector<GLfloat> normals;
//
//    for (int i = 0; i < 36 * 3; i++) {
//        normals.push_back(0.0f);
//    }
//    if (blockType == GRASS || blockType == POPPY) {
//        indices.clear();
//        for (int i = 0; i < 12; i++) {
//            indices.push_back(i + indexOffset);
//        }
//
//        blockIndices.clear();
//        for (int i = 0; i < 12; i++) {
//            blockIndices.push_back(i);
//        }
//
//        triangle = {
//            0.0f + x, 0.0f + y, 0.0f + z,
//            0.0f + x, 1.0f + y, 0.0f + z,
//            1.0f + x, 0.0f + y, 1.0f + z,
//            0.0f + x, 1.0f + y, 0.0f + z,
//            1.0f + x, 1.0f + y, 1.0f + z,
//            1.0f + x, 0.0f + y, 1.0f + z,
//
//            0.0f + x, 0.0f + y, 1.0f + z,
//            0.0f + x, 1.0f + y, 1.0f + z,
//            1.0f + x, 0.0f + y, 0.0f + z,
//            0.0f + x, 1.0f + y, 1.0f + z,
//            1.0f + x, 1.0f + y, 0.0f + z,
//            1.0f + x, 0.0f + y, 0.0f + z,
//        };
//
//        globalUVs.clear();
//        for (int i = 0; i < 2; i++) {
//            int offsetX = 0, offsetY = 0;
//            globalUVs.push_back((clipX + xoffset + offsetX) / xdimens); globalUVs.push_back((clipX + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
//            globalUVs.push_back((clipX + xoffset + offsetX) / xdimens); globalUVs.push_back((clipY + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
//            globalUVs.push_back((clipY + xoffset + offsetX) / xdimens); globalUVs.push_back((clipX + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
//            globalUVs.push_back((clipX + xoffset + offsetX) / xdimens); globalUVs.push_back((clipY + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
//            globalUVs.push_back((clipY + xoffset + offsetX) / xdimens); globalUVs.push_back((clipY + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
//            globalUVs.push_back((clipY + xoffset + offsetX) / xdimens); globalUVs.push_back((clipX + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
//        }
//
//        for (int i = 0; i < 12 * 3; i++) {
//            normals.push_back(0.0f);
//        }
//    }
//
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
//    calcAverageNormals(finalVerts, indices, 9, 6, indexOffset);
//    //cout << finalVerts[6] << endl;
//    repChunk.vertices.insert(repChunk.vertices.end(), finalVerts.begin(), finalVerts.end());
//    repChunk.indices.insert(repChunk.indices.end(), indices.begin(), indices.end());
//
//    repChunk.needUpdate = true;
//
//    Block newBlock(vec3(x, y, z), blockType, finalVerts, blockIndices);
//    //newBlock.blockMesh.createMesh(finalVerts, indices, finalVerts.size(), indices.size());
//    repChunk.addBlock(newBlock);
//    //cout << newBlock.position.x << " " << newBlock.position.y << " " << newBlock.position.z << endl;
//    repChunk.blockNum++;
//}

void createCubeInThread(float x, float y, float z, Chunk& repChunk, Item blockType) {
    if (blockType == AIR) return;
    float xoffset = 0, yoffset = 1, xoffsetTop = 0, yoffsetTop = 0, xoffsetBottom = 0, yoffsetBottom = 0, transparency = 1.0f;

    int arbitraryoff = 0;
    if (blockType == AIR) {
        xoffset = 3; yoffset = 3;
    }

    else if (blockType == GRASS_BLOCK) {
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
        xoffset = 1, yoffset = 0, transparency = 0.25f;
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

    else if (blockType == BLUE_ORCHID) {
        xoffset = 0, yoffset = 4;
    }

    else if (blockType == BEDROCK) {
        xoffset = 4, yoffset = 0;
    }

    float clipX = 0.03f, clipY = 0.97f;
    int num_of_faces = 6;

    unsigned int indexOffset = repChunk.indexOffset;
    //cout << indexOffset << endl;
    //for (int i = 0; i < repChunk.blocks.size(); i++) {
        //cout << "conputer index offset!" << endl;
    //}
    vector<unsigned int> indices[6];

    for (int i = 0; i < 36; i++) {
        indices[i / 6].push_back(i + indexOffset);
    }

    vector<unsigned int> blockIndices;

    vector<GLfloat> globalUVs[6];

    for (int i = 0; i < 6; i++) {
        int offsetX = 0, offsetY = 0;
        if (i == 4) { offsetX = xoffsetBottom; offsetY = yoffsetBottom; }
        else if (i == 5) { offsetX = xoffsetTop;  offsetY = yoffsetTop; }
        globalUVs[i].push_back((clipX + xoffset + offsetX) / xdimens); globalUVs[i].push_back((clipX + yoffset + offsetY) / ydimens); globalUVs[i].push_back(transparency);
        globalUVs[i].push_back((clipX + xoffset + offsetX) / xdimens); globalUVs[i].push_back((clipY + yoffset + offsetY) / ydimens); globalUVs[i].push_back(transparency);
        globalUVs[i].push_back((clipY + xoffset + offsetX) / xdimens); globalUVs[i].push_back((clipX + yoffset + offsetY) / ydimens); globalUVs[i].push_back(transparency);
        globalUVs[i].push_back((clipX + xoffset + offsetX) / xdimens); globalUVs[i].push_back((clipY + yoffset + offsetY) / ydimens); globalUVs[i].push_back(transparency);
        globalUVs[i].push_back((clipY + xoffset + offsetX) / xdimens); globalUVs[i].push_back((clipY + yoffset + offsetY) / ydimens); globalUVs[i].push_back(transparency);
        globalUVs[i].push_back((clipY + xoffset + offsetX) / xdimens); globalUVs[i].push_back((clipX + yoffset + offsetY) / ydimens); globalUVs[i].push_back(transparency);
    }

    vector<GLfloat> triangle[6] = {
        {
            0.0f + x, 0.0f + y, 0.0f + z,
            0.0f + x, 1.0f + y, 0.0f + z,
            0.0f + x, 0.0f + y, 1.0f + z,
            0.0f + x, 1.0f + y, 0.0f + z,
            0.0f + x, 1.0f + y, 1.0f + z,
            0.0f + x, 0.0f + y, 1.0f + z
        },

        {
            1.0f + x, 0.0f + y, 0.0f + z,
            1.0f + x, 1.0f + y, 0.0f + z,
            1.0f + x, 0.0f + y, 1.0f + z,
            1.0f + x, 1.0f + y, 0.0f + z,
            1.0f + x, 1.0f + y, 1.0f + z,
            1.0f + x, 0.0f + y, 1.0f + z,
        },

        {
            0.0f + x, 0.0f + y, 0.0f + z,
            0.0f + x, 1.0f + y, 0.0f + z,
            1.0f + x, 0.0f + y, 0.0f + z,
            0.0f + x, 1.0f + y, 0.0f + z,
            1.0f + x, 1.0f + y, 0.0f + z,
            1.0f + x, 0.0f + y, 0.0f + z,
        },

        {
            0.0f + x, 0.0f + y, 1.0f + z,
            0.0f + x, 1.0f + y, 1.0f + z,
            1.0f + x, 0.0f + y, 1.0f + z,
            0.0f + x, 1.0f + y, 1.0f + z,
            1.0f + x, 1.0f + y, 1.0f + z,
            1.0f + x, 0.0f + y, 1.0f + z,
        },

        {
            0.0f + x, 0.0f + y, 0.0f + z,
            1.0f + x, 0.0f + y, 0.0f + z,
            0.0f + x, 0.0f + y, 1.0f + z,
            1.0f + x, 0.0f + y, 0.0f + z,
            1.0f + x, 0.0f + y, 1.0f + z,
            0.0f + x, 0.0f + y, 1.0f + z,
        },

        {
            0.0f + x, 1.0f + y, 0.0f + z,
            1.0f + x, 1.0f + y, 0.0f + z,
            0.0f + x, 1.0f + y, 1.0f + z,
            1.0f + x, 1.0f + y, 0.0f + z,
            1.0f + x, 1.0f + y, 1.0f + z,
            0.0f + x, 1.0f + y, 1.0f + z,
        }
    };
    vector<GLfloat> normals[6];

    for (int i = 0; i < 6; i++) {
        normals[i] = normalsLong[i];
    }
    //for (int i = 0; i < 36; i++) {
    //    for(int j = 0; j < 3; j++)
    //        normals[i / 6].push_back(0.0f);
    //}
    if (blockType == GRASS || blockType == POPPY || blockType == BLUE_ORCHID) {
        num_of_faces = 2;

        for (int i = 0; i < 6; i++) {
            indices[i].clear();
        }
        for (int i = 0; i < 12; i++) {
            indices[i/6].push_back(i + indexOffset);
        }

        //blockIndices.clear();
        //for (int i = 0; i < 12; i++) {
        //    blockIndices.push_back(i);
        //}

        triangle[0] = {
            0.0f + x, 0.0f + y, 0.0f + z,
            0.0f + x, 1.0f + y, 0.0f + z,
            1.0f + x, 0.0f + y, 1.0f + z,
            0.0f + x, 1.0f + y, 0.0f + z,
            1.0f + x, 1.0f + y, 1.0f + z,
            1.0f + x, 0.0f + y, 1.0f + z,
        };
        triangle[1] = {
            0.0f + x, 0.0f + y, 1.0f + z,
            0.0f + x, 1.0f + y, 1.0f + z,
            1.0f + x, 0.0f + y, 0.0f + z,
            0.0f + x, 1.0f + y, 1.0f + z,
            1.0f + x, 1.0f + y, 0.0f + z,
            1.0f + x, 0.0f + y, 0.0f + z,
        };

        for (int i = 0; i < 6; i++) {
            globalUVs[i].clear();
        }
        for (int i = 0; i < 2; i++) {
            int offsetX = 0, offsetY = 0;
            globalUVs[i].push_back((clipX + xoffset + offsetX) / xdimens); globalUVs[i].push_back((clipX + yoffset + offsetY) / ydimens); globalUVs[i].push_back(transparency);
            globalUVs[i].push_back((clipX + xoffset + offsetX) / xdimens); globalUVs[i].push_back((clipY + yoffset + offsetY) / ydimens); globalUVs[i].push_back(transparency);
            globalUVs[i].push_back((clipY + xoffset + offsetX) / xdimens); globalUVs[i].push_back((clipX + yoffset + offsetY) / ydimens); globalUVs[i].push_back(transparency);
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
        for (int i = 0; i < triangle[k].size() / 3; i++) {
            finalVertsFace[k].push_back(triangle[k][3 * i + 0]);
            finalVertsFace[k].push_back(triangle[k][3 * i + 1]);
            finalVertsFace[k].push_back(triangle[k][3 * i + 2]);

            finalVertsFace[k].push_back(globalUVs[k][3 * i + 0]);
            finalVertsFace[k].push_back(globalUVs[k][3 * i + 1]);
            finalVertsFace[k].push_back(globalUVs[k][3 * i + 2]);

            finalVertsFace[k].push_back(normals[k][3 * i + 0]);
            finalVertsFace[k].push_back(normals[k][3 * i + 1]);
            finalVertsFace[k].push_back(normals[k][3 * i + 2]);
        }
    }

    Face faces[6];
    vector<GLfloat> finalVerts;
    vector<unsigned int> finalInds;
    //cout << num_of_faces << " faces!" << endl;
    for (int i = 0; i < num_of_faces; i++) {
        faces[i].vertices = move(finalVertsFace[i]);
        faces[i].indices = move(indices[i]);
        int xc = 0, yc = 0, zc = 0;

        if (i == 0) {
            xc = -1;
        }
        else if (i == 1) {
            xc = 1;
        }
        else if (i == 2) {
            zc = -1;
        }
        else if (i == 3) {
            zc = 1;
        }
        else if (i == 4) {
            yc = -1;
        }
        else {
            yc = 1;
        }

        faces[i].center = ivec3((int)faces[i].vertices[0] + xc, (int)faces[i].vertices[1] + yc, (int)faces[i].vertices[2] + zc);
        //cout << faces[i].center.x << " x " << faces[i].center.y << " y " << faces[i].center.z << " z " << endl;
        //cout << faces[i].center.x << " " << faces[i].center.y << " " << faces[i].center.z << endl;
        //if(!blockExistsAt(ivec3(x, y, z)))u
        if (num_of_faces == 6){
            //if(i > 0)
            if (!blockExistsAt(ivec3(x + xc, y + yc, z + zc))) {
                finalVerts.insert(finalVerts.end(), faces[i].vertices.begin(), faces[i].vertices.end());
                repChunk.faces.insert(make_pair(faces[i].center, faces[i]));
            }
            else {
                //vector<GLfloat> unwantedface;
                //{
                //    Block targetBlock;
                //    std::lock_guard<std::mutex> lock(chunkRequestMutex);
                //    targetBlock = worldBlocks[ivec3(x + xc, y + yc, z + zc)];
                //    int target_face_size = 9 * 6; 
                //    unwantedface.assign(targetBlock.vertices.begin() + i * target_face_size, targetBlock.vertices.begin() + i * target_face_size + target_face_size);
                //}
                //auto it = std::search(repChunk.vertices.begin(), repChunk.vertices.end(), faces[i].vertices.begin(), faces[i].vertices.end());
                //if (it != repChunk.vertices.end()) {
                //    repChunk.vertices.erase(it, it + faces[i].vertices.size());
                //}
                
                //for (int j = 0; j < repChunk.vertices.size(); j++) {
                //    if (repChunk.vertices[j] == x && repChunk.vertices[j + 1] == y && repChunk.vertices[j + 2] == z) {
                //        repChunk.vertices.erase(repChunk.vertices.begin() + j + i * 6, repChunk.vertices.begin() + j + i * 6 + 6);
                //    }
                //}
                repChunk.faces.erase(faces[i].center);
            }
            //else {
            //    finalVerts.insert(finalVerts.end(), faces[i].vertices.begin(), faces[i].vertices.end());
            //}
            //else {
            //    if ((blockType == GRASS_BLOCK || blockType == DIRT_BLOCK || blockType == STONE_BLOCK)) {
            //        if (i == 5) {
            //            //    finalVerts.insert(finalVerts.end(), faces[i].vertices.begin(), faces[i].vertices.end());
            //            //    repChunk.faces.insert(make_pair(faces[i].center, faces[i]));
            //            //    goto skip;
            //            //}
            //            //srand(time(0));
            //            //if (repChunk.faces.find(faces[i].center) != repChunk.faces.end()) {
            //            //    goto skip;
            //            //}
            //            //if (repChunk.faces.find(faces[i].center) == repChunk.faces.end()) {
            //            finalVerts.insert(finalVerts.end(), faces[i].vertices.begin(), faces[i].vertices.end());
            //        }
            //    }
            //    else {
            //        finalVerts.insert(finalVerts.end(), faces[i].vertices.begin(), faces[i].vertices.end());
            //    }
            //}
                //repChunk.faces.insert(make_pair(faces[i].center, faces[i]));
        }
        else {
            finalVerts.insert(finalVerts.end(), faces[i].vertices.begin(), faces[i].vertices.end());
            //finalInds.insert(finalInds.end(), faces[i].indices.begin(), faces[i].indices.end());
            repChunk.faces.insert(make_pair(faces[i].center, faces[i]));
        }
    }

    //finalInds.clear(); 
    //cout << finalVerts.size() / 9 << endl;
    for (int i = 0; i < finalVerts.size() / 9; i++) {
        finalInds.push_back(i + indexOffset);
    }

    blockIndices.clear();
    for (int i = 0; i < 6 * num_of_faces; i++) {
        blockIndices.push_back(i);
    }
    
    calcAverageNormals(finalVerts, finalInds, 9, 6, indexOffset);
    //cout << finalVerts[6] << endl;
    repChunk.vertices.insert(repChunk.vertices.end(), finalVerts.begin(), finalVerts.end());
    repChunk.indices.insert(repChunk.indices.end(), finalInds.begin(), finalInds.end());

    repChunk.indexOffset += finalInds.size();

    repChunk.needUpdate = true;

    Block newBlock(vec3(x, y, z), blockType, finalVerts, finalInds);
    //newBlock.blockMesh.createMesh(finalVerts, indices[i], finalVerts.size(), indices[i].size());
    std::lock_guard<std::mutex> lock(chunkRequestMutex);
    repChunk.addBlock(newBlock);
    //cout << newBlock.position.x << " " << newBlock.position.y << " " << newBlock.position.z << endl;
}

void removeRucurrentFaces(vec3 pos, Chunk& repChunk) {
    for (int j = 0; j < repChunk.vertices.size(); j++) {
        if (repChunk.vertices[j] == pos.x && repChunk.vertices[j + 1] == pos.y && repChunk.vertices[j + 2] == pos.z) {
            repChunk.vertices.erase(repChunk.vertices.begin() + j * 6, repChunk.vertices.begin() + j * 6 + 6);
        }
    }
}



void generateChunkAt(vec2 xyChunk, Chunk& repChunk) {
    //std::lock_guard<std::mutex> lock(chunkResultMutex);
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

    int temp_chunk = CHUNK_SIZE;
    
    FastNoiseLite noise;
    //noise.SetSeed(42);   
    noise.SetFrequency(0.003f);

    

    FastNoiseLite caveNoise;
    caveNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    caveNoise.SetFrequency(0.05f);
    FastNoiseLite cloudNoise;
    cloudNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    cloudNoise.SetFrequency(0.04);

    float cloudDensity = 0;
    float density = 0;

    repChunk.blocks.clear();
    repChunk.blocks.reserve(CHUNK_SIZE * CHUNK_SIZE * 100); // heuristic reserve
    //CHUNK_SIZE = 2;
    for (int x = (xyChunk.x) * CHUNK_SIZE; x < (xyChunk.x + 1) * (CHUNK_SIZE); x++) {
        for (int z = (xyChunk.y) * CHUNK_SIZE; z < (xyChunk.y + 1) * (CHUNK_SIZE); z++) {
            createCubeInThread(x, -1, z, repChunk, BEDROCK);
            //float total = 0;
            //int samples = 5000;
            //for (int i = 0; i < samples; i++) {
            //    total += static_cast<float>(rand()) / RAND_MAX;
            //}
            //float totalrand = total / samples;
            float height = noise.GetNoise((float)x, (float)z); // returns value in range [-1, 1]
            float scaledHeight = ((height + 1.1f) * (CHUNK_SIZE * CHUNK_SIZE) + 5.0f);
            float treeHeight = randomFloat(5.0, 12.0), treeDistrib = rand();
            for (int y = 0; y < scaledHeight; y++) {
                
                if (y > scaledHeight - 1) {
                    blockType = GRASS_BLOCK;
                }
                else if (y > scaledHeight - 4 && y <= scaledHeight - 1) {
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

                cloudDensity = cloudNoise.GetNoise((float)x, (float)y, (float)z);
                density = caveNoise.GetNoise((float)x, (float)y, (float)z);

                if (cloudDensity < -0.5f) {
                    if(!blockExistsAt(ivec3(x, CHUNK_SIZE * CHUNK_SIZE + 10 * CHUNK_SIZE, z)))
                        createCubeInThread(x, CHUNK_SIZE * CHUNK_SIZE + 10 * CHUNK_SIZE, z, repChunk, CLOUD);
                }

                if (density < -0.4f) {
                    continue;
                }

                if (!blockExistsAt(ivec3(x, y, z)))
                    createCubeInThread(x, y, z, repChunk, blockType);
                if (blockType == GRASS_BLOCK) {
                    // Making the trees and leaves
                    if (treeDistrib > 0.0 && treeDistrib <= 200) {

                        for (int i = y + 1; i < y + treeHeight; i++) {
                            if (!blockExistsAt(ivec3(x, i, z)))
                                createCubeInThread(x, (int)i, z, repChunk, OAK_WOOD);
                        }
                        glm::ivec3 center(x, y + treeHeight, z);

                        int radius = 3; // adjust for size

                        for (int dx = -radius; dx <= radius; ++dx) {
                            for (int dy = -radius; dy <= radius; ++dy) {
                                for (int dz = -radius; dz <= radius; ++dz) {
                                    glm::ivec3 pos = center + glm::ivec3(dx, dy, dz);
                                    float dist = glm::length(glm::vec3(dx, dy, dz));

                                    if (dist <= radius + randomFloat(-0.3f, 0.3f)) {
                                        if (!blockExistsAt(pos))
                                            createCubeInThread(pos.x, pos.y, pos.z, repChunk, OAK_LEAVES);
                                    }
                                }
                            }
                        }
                    }

                    float randomNumberForGrass = rand(), randomNumberForPoppy = rand(), randomNumberForOrchid = rand();

                    //for (int i = 0; i < repChunk.blocks.size(); i++) {
                        if (blockType == GRASS_BLOCK) {
                            if (randomNumberForGrass < 2000.0) {
                                if (!blockExistsAt(ivec3(x, scaledHeight + 1, z)))
                                    createCubeInThread(x, (int)scaledHeight + 1, z, repChunk, GRASS);
                                break;
                            }

                            if (randomNumberForPoppy < 500.0) {
                                if (!blockExistsAt(ivec3(x, scaledHeight + 1, z)))
                                    createCubeInThread(x, (int)scaledHeight + 1, z, repChunk, POPPY);
                                break;
                            }

                            if (randomNumberForOrchid < 200.0) {
                                if (!blockExistsAt(ivec3(x, scaledHeight + 1, z)))
                                    createCubeInThread(x, (int)scaledHeight + 1, z, repChunk, BLUE_ORCHID);
                                break;
                            }
                        }
                    //}
                }
            }
            //for (int i = scaledHeight; i < scaledHeight + 15; i++) {
            //    if(!blockExistsAt(ivec3(x, i, z)))
            //        createCubeInThread(x, i, z, repChunk, AIR);
            //}
            //CHUNK_SIZE = 1;
        }
    }
    //calcAverageNormals(repChunk.vertices, repChunk.indices, 9, 6, repChunk.indexOffset);
}

std::thread chunkGenThread([&]() {
    while (chunkGenRunning) {
        vec2 coord;
        {
            std::lock_guard<std::mutex> lock(chunkRequestMutex);
            if (!chunkRequestQueue.empty()) {
                coord = chunkRequestQueue.front();
                chunkRequestQueue.pop();
            }
            else {
                continue;
            }
        }

        Chunk newChunk;
        {
            //std::lock_guard<std::mutex> lock(chunkRequestMutex);
            generateChunkAt(coord, newChunk);
        }

        newChunk.coords = chunkCoords.back();
        newChunk.needUpdate = true;
        //cout << newChunk.blockNum << endl;
        {
            std::lock_guard<std::mutex> lock(chunkResultMutex);
            chunkResultQueue.push(std::move(newChunk));
        }
    }
    });

std::thread chunkGenThread2([&]() {
    while (chunkGenRunning2) {
        vec2 coord;
        {
            std::lock_guard<std::mutex> lock(chunkRequestMutex2);
            if (!chunkRequestQueue2.empty()) {
                coord = chunkRequestQueue2.front();
                chunkRequestQueue2.pop();
            }
            else {
                continue;
            }
        }

        Chunk newChunk;
        {
            std::lock_guard<std::mutex> lock(chunkRequestMutex2);
            generateChunkAt(coord, newChunk);
        }

        newChunk.coords = chunkCoords.back();
        newChunk.needUpdate = true;
        //cout << newChunk.blockNum << endl;
        {
            std::lock_guard<std::mutex> lock(chunkResultMutex2);
            chunkResultQueue2.push(std::move(newChunk));
        }
    }
    });

std::thread chunkGenThread3([&]() {
    while (chunkGenRunning3) {
        vec2 coord;
        {
            std::lock_guard<std::mutex> lock(chunkRequestMutex3);
            if (!chunkRequestQueue3.empty()) {
                coord = chunkRequestQueue3.front();
                chunkRequestQueue3.pop();
            }
            else {
                continue;
            }
        }

        Chunk newChunk;
        generateChunkAt(coord, newChunk);

        newChunk.coords = chunkCoords.back();
        newChunk.needUpdate = true;
        //cout << newChunk.blockNum << endl;
        {
            std::lock_guard<std::mutex> lock(chunkResultMutex3);
            chunkResultQueue3.push(std::move(newChunk));
        }
    }
    });

std::thread blockBreakThread([&]() {
    while (blockBreaking) {
        {
            std::lock_guard<std::mutex> lock(breakReqMutex);
            if (!breakReqQueue.empty()) {
                breakReqQueue.pop();
            }
            else {
                continue;
            }
        }
        if (blockBreakingOut) {
            std::lock_guard<std::mutex> lock(breakReqMutex);
            world.delBlocklook_at();
            blockBreakingOut = false;
        }
    }
    });

std::thread blockPlaceThread([&]() {
    while (blockPlacing) {
        {
            std::lock_guard<std::mutex> lock(placeReqMutex);
            if (!placeReqQueue.empty()) {
                placeReqQueue.pop();
            }
            else {
                continue;
            }
        }
        if (blockPlacingOut) {
            std::lock_guard<std::mutex> lock(placeReqMutex);
            world.addBlocklook_at(inventory.inv_slots[3][slot]);
            blockPlacingOut = false;
        }
    }
    });