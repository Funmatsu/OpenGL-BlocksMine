#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <glm/glm.hpp>
#include <glm\gtc/type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <cmath>
#include <vector>

#include <random>
#include <ctime>

#include <thread>
#include <mutex>
#include <queue>
#include <atomic>

//#include <SFML/Window.hpp>
//#include <SFML/Graphics.hpp>a

#include "Mesh.h"
#include "shader.h"
#include "Window.h"
#include "Camera.h"
#include "Texture.h"
#include "FastNoiseLite.h"

using namespace std;
using namespace glm;

#define WIDTH         1800
#define HEIGHT        1800

#define DIAMOND_ORE   1
#define GRASS_BLOCK   2
#define IRON_ORE      3
#define STONE_BLOCK   4
#define DIRT_BLOCK    5
#define OAK_WOOD      6
#define CLOUD         7
#define OAK_LEAVES    8
#define OAK_PLANK     9
#define GRASS         10

#define WOODEN_PICKAXE 11

const int CHUNK_SIZE = 5;
vector<vec2> chunkCoords;

int xdimens = 4, ydimens = 4;
int xdimensItem = 1, ydimensItem = 1;

unsigned int shader;
vector<Mesh> meshes;

vector<Shadergl*> shaders;
vector<Shadergl*> InventoryShaders;
vector<Shadergl*> craftInvShaders;
Window mainWindow;
Camera camera = Camera(vec3(CHUNK_SIZE / 2, CHUNK_SIZE * CHUNK_SIZE + 5, CHUNK_SIZE / 2), vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f, 2.5f, 0.5f);

vector<Texturegl*> Textures;

static const char* vshader = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/vshader.txt";
static const char* fshader = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/fshader.txt";

static const char* vshader2 = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/vshader2.txt";
static const char* fshader2 = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/fshader2.txt";

static const char* vshaderInv = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/vshaderInv.txt";
static const char* fshaderInv = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/fshaderInv.txt";

static const char* vshaderInvBlock = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/vshaderInvBlock.txt";
static const char* fshaderInvBlock = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/fshaderInvBlock.txt";

static const char* vshaderInvSlot = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/vshaderInvSlot.txt";
static const char* fshaderInvSlot = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/fshaderInvSlot.txt";

static const char* vshaderLooking = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/vshaderLooking.txt";
static const char* fshaderLooking = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/fshaderLooking.txt";

GLfloat deltaTime = 2.0f;
GLfloat lastTime = 0.0f;

int currentBlockType = 1;

Mesh currInvSlot[4][9];
Mesh craftInvSlot[2][2];
int craftInv[2][2];
int inv_slots[4][9];
vector<int> recipes = {OAK_WOOD, 0, 0, 0, OAK_PLANK, 0, OAK_WOOD, 0, 0, OAK_PLANK, 0, 0, OAK_WOOD, 0, OAK_PLANK, 0, 0, 0, OAK_WOOD, OAK_PLANK, STONE_BLOCK, IRON_ORE, 0, 0, WOODEN_PICKAXE, IRON_ORE, STONE_BLOCK, 0, 0, DIAMOND_ORE };
int slot = 0;

bool inv_change, inf_blocks = false, inventoryOn = false;

struct Block {
    vector<float> vertices;
    vector<unsigned int> indices;
    int type = 0;
    vec3 position;
    Mesh blockMesh;

    Block(vec3 pos, int blockType, vector<float> verts, vector<unsigned int> inds) {
        position = pos;
        vertices = verts;
        indices = inds;
        type = blockType;
    }
    vec3 getBlockPosition() { return position; }
};


float randomFloat(float min, float max) {
    static std::default_random_engine generator(std::random_device{}());
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(generator);
}

struct Chunk {
    Mesh mesh;
    vector<float> vertices;
    vector<unsigned int> indices;
    unsigned int blockNum = 0;
    bool needUpdate = false;
    int specialId;
    vec2 coords;
    vector<Block> blocks;
};

struct World {
    vector<Chunk> chunks;
    Block getBlockAt(vec3 blockPos) {
        for (int k = 0; k < chunks.size(); k++) {
            for (int i = 0; i < chunks[k].blocks.size(); i++) {
                ivec3 checkBlock = floor(chunks[k].blocks[i].position);
                if (checkBlock == ivec3(blockPos)) {
                    return chunks[k].blocks[i];
                }
            }
        }
    }
};

World world;

std::queue<vec2> chunkRequestQueue;
std::mutex chunkRequestMutex;

std::queue<Chunk> chunkResultQueue;
std::mutex chunkResultMutex;

std::queue<vec2> chunkRequestQueue2;
std::mutex chunkRequestMutex2;

std::queue<Chunk> chunkResultQueue2;
std::mutex chunkResultMutex2;

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
std::atomic<bool> blockBreaking = true;
std::atomic<bool> blockPlacing = true;
bool blockBreakingOut = false;
bool blockPlacingOut = false;

struct Frustum {
    glm::vec4 planes[6]; // Each plane: ax + by + cz + d = 0
};

Frustum extractFrustumPlanes(const glm::mat4& vp) {
    Frustum f;

    f.planes[0] = glm::vec4(vp[0][3] + vp[0][0], vp[1][3] + vp[1][0], vp[2][3] + vp[2][0], vp[3][3] + vp[3][0]); // Left
    f.planes[1] = glm::vec4(vp[0][3] - vp[0][0], vp[1][3] - vp[1][0], vp[2][3] - vp[2][0], vp[3][3] - vp[3][0]); // Right
    f.planes[2] = glm::vec4(vp[0][3] + vp[0][1], vp[1][3] + vp[1][1], vp[2][3] + vp[2][1], vp[3][3] + vp[3][1]); // Bottom
    f.planes[3] = glm::vec4(vp[0][3] - vp[0][1], vp[1][3] - vp[1][1], vp[2][3] - vp[2][1], vp[3][3] - vp[3][1]); // Top
    f.planes[4] = glm::vec4(vp[0][3] + vp[0][2], vp[1][3] + vp[1][2], vp[2][3] + vp[2][2], vp[3][3] + vp[3][2]); // Near
    f.planes[5] = glm::vec4(vp[0][3] - vp[0][2], vp[1][3] - vp[1][2], vp[2][3] - vp[2][2], vp[3][3] - vp[3][2]); // Far

    return f;
}

bool isChunkVisible(const Frustum& f, const glm::vec2& chunkPos) {
    glm::vec3 min(chunkPos.x * CHUNK_SIZE, 0.0f, chunkPos.y * CHUNK_SIZE);
    glm::vec3 max((chunkPos.x + 1) * CHUNK_SIZE, CHUNK_SIZE * CHUNK_SIZE, (chunkPos.y + 1) * CHUNK_SIZE);


    for (int i = 0; i < 6; ++i) {
        glm::vec4 plane = f.planes[i];
        plane /= glm::length(glm::vec3(plane));

        // Find the most positive vertex (farthest in direction of plane normal)
        glm::vec3 p = glm::vec3(
            (plane.x < 0) ? min.x : max.x,
            (plane.y < 0) ? min.y : max.y,
            (plane.z < 0) ? min.z : max.z
        );

        // If that vertex is outside the plane, chunk is outside
        if (plane.x * p.x + plane.y * p.y + plane.z * p.z + plane.w < 0)
            return false;
    }

    return true;
}

bool itemPlaceable(int itemType) {
    return (itemType > DIAMOND_ORE && itemType < GRASS);
}

bool isTool(int itemType) {
    return (itemType >= WOODEN_PICKAXE);
}

void addBlockToWorld(vec3 position, vector<float> vertices, vector<unsigned int> indices, int vertNum, int indNum, int blockType) {
    for (int i = 0; i < vertNum; i++) {
        world.chunks[world.chunks.size() - 1].vertices.push_back(vertices[i]);
    }
    for (int i = 0; i < indNum; i++) {
        world.chunks[world.chunks.size() - 1].indices.push_back(indices[i]);
    }
    world.chunks[world.chunks.size() - 1].needUpdate = true;
    //cout << "hey: " << world.chunks.size() << endl;
    //world.blocks.push_back(Block(position, blockType, vertices, indices));
}

void addBlockToWorld(Block block) {
    world.chunks.back().vertices.insert(world.chunks.back().vertices.end(), block.vertices.begin(), block.vertices.end());
    world.chunks.back().indices.insert(world.chunks.back().indices.end(), block.indices.begin(), block.indices.end());
    world.chunks.back().blocks.push_back(block);
    world.chunks.back().needUpdate = true;
}

bool contains(vector<vec2> vec, vec2 value) {
    return std::find(vec.begin(), vec.end(), value) != vec.end();
}

bool blockExistsAt(ivec3 blockPos) {
    if (world.chunks.size() == 0) {
        return false;
    }
    for (int k = 0; k < world.chunks.size(); k++) {
        for (int i = 0; i < world.chunks[k].blocks.size(); i++) {
            ivec3 checkBlock = floor(world.chunks[k].blocks[i].position);
            if (checkBlock == blockPos) {
                return true;
            }
        }
    }
    return false;
}

vec3 lookingAtBlock() {
    ivec3 blockPos = vec3(0.0f);
    glm::vec3 rayDir;
    glm::vec3 rayOrigin = camera.getCameraPos();
    rayDir = normalize(camera.getCameraFront());

    float maxDistance = 15.0f;
    float stepSize = 0.2f;
    for (float t = 0.0f; t < maxDistance; t += stepSize) {
        glm::vec3 point = rayOrigin + rayDir * t;
        blockPos = glm::floor(point);
        //cout << "block at " << floor(blockPos.x) << ", " << floor(blockPos.y) << ", " << floor(blockPos.z) << " ? : " << blockExistsAt(blockPos) << endl;
        if (blockExistsAt(blockPos)) {
            return blockPos;
        }
    }
    return vec3(-404.0f);
}

void deleteBlockFromWorld(vec3 blockPos) {
    
    for (int k = 0; k < world.chunks.size(); k++) {
        int blockVertNums = 0;
        bool blockBroken = false;
        for (int i = 0; i < world.chunks[k].blocks.size(); i++) {
            if (world.chunks[k].blocks[i].position == blockPos) {
                
                for (int m = 0; m < 4; m++) {
                    for (int l = 0; l < 9; l++) {
                        if (inv_slots[3 - m][l] == NULL) {
                            inv_slots[3 - m][l] = world.chunks[k].blocks[i].type;
                            inv_change = true;
                            blockBroken = true;
                            break;
                        }
                    }
                    if (blockBroken) {
                        break;
                    }
                }
                
                world.chunks[k].vertices.erase(world.chunks[k].vertices.begin() + blockVertNums, world.chunks[k].vertices.begin() + blockVertNums + world.chunks[k].blocks[i].vertices.size());
                world.chunks[k].blocks.erase(world.chunks[k].blocks.begin() + i);
                
                world.chunks[k].needUpdate = true;
                return;
            }
            blockVertNums += world.chunks[k].blocks[i].vertices.size();
        }
    }
}

Mesh createMeshCube(float x, float y, float z, int blockType) {
    float xoffset = 0, yoffset = 1, xoffsetTop = 0, yoffsetTop = 0, xoffsetBottom = 0, yoffsetBottom = 0, transparency = 1.0f;

    int arbitraryoff = 0;
    if (blockType == GRASS_BLOCK) {
        xoffset = 0, yoffset = 2; xoffsetTop = 2; yoffsetTop = 0; xoffsetBottom = 2; yoffsetBottom = 2;
    }

    if (blockType == IRON_ORE) {
        xoffset = 1; yoffset = 1 + arbitraryoff;
    }

    if (blockType == STONE_BLOCK) {
        xoffset = 1; yoffset = 2;
    }

    if (blockType == DIRT_BLOCK) {
        xoffset = 2; yoffset = 0;
    }

    if (blockType == OAK_WOOD) {
        xoffset = 2, yoffset = 1; xoffsetTop = -2; yoffsetTop = 2; xoffsetBottom = -2; yoffsetBottom = 2;
    }

    if (blockType == CLOUD) {
        xoffset = 1, yoffset = 0, transparency = 0.75f;
    }

    if (blockType == OAK_PLANK) {
        xoffset = 3, yoffset = 0;
    }

    if (blockType == OAK_LEAVES) {
        xoffset = 0, yoffset = 0;
    }

    if (blockType == GRASS) {
        x = 3, y = 1; xoffsetTop = 0, xoffsetBottom = 0; yoffsetTop = 1, yoffsetBottom = 1;
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
        0.0f + x, 0.0f + y, 0.0f+ z,
        0.0f + x, 1.0f + y, 0.0f+ z,
        0.0f + x, 0.0f + y, 0.1f+ z,
        0.0f + x, 1.0f + y, 0.0f+ z,
        0.0f + x, 1.0f + y, 0.1f+ z,
        0.0f + x, 0.0f + y, 0.1f+ z,
        1.0f + x, 0.0f + y, 0.0f+ z,
        1.0f + x, 0.0f + y, 0.1f+ z,
        1.0f + x, 1.0f + y, 0.0f+ z,
        1.0f + x, 1.0f + y, 0.0f+ z,
        1.0f + x, 0.0f + y, 0.1f+ z,
        1.0f + x, 1.0f + y, 0.1f+ z,
        0.0f + x, 0.0f + y, 0.0f+ z,
        0.0f + x, 0.0f + y, 0.1f+ z,
        1.0f + x, 0.0f + y, 0.0f+ z,
        1.0f + x, 0.0f + y, 0.0f+ z,
        0.0f + x, 0.0f + y, 0.1f+ z,
        1.0f + x, 0.0f + y, 0.1f+ z,
        0.0f + x, 1.0f + y, 0.0f+ z,
        1.0f + x, 1.0f + y, 0.0f+ z,
        0.0f + x, 1.0f + y, 0.1f+ z,
        1.0f + x, 1.0f + y, 0.0f+ z,
        1.0f + x, 1.0f + y, 0.1f+ z,
        0.0f + x, 1.0f + y, 0.1f+ z,
        0.0f + x, 0.0f + y, 0.0f+ z,
        1.0f + x, 0.0f + y, 0.0f+ z,
        0.0f + x, 1.0f + y, 0.0f+ z,
        0.0f + x, 1.0f + y, 0.0f+ z,
        1.0f + x, 0.0f + y, 0.0f+ z,
        1.0f + x, 1.0f + y, 0.0f+ z,
        0.0f + x, 0.0f + y, 0.1f+ z,
        0.0f + x, 1.0f + y, 0.1f+ z,
        1.0f + x, 0.0f + y, 0.1f+ z,
        0.0f + x, 1.0f + y, 0.1f+ z,
        1.0f + x, 1.0f + y, 0.1f+ z,
        1.0f + x, 0.0f + y, 0.1f+ z
    };

    if (blockType == GRASS) {
        indices = {
            0 , 1 , 2 ,
            3 , 4 , 5
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

    Mesh cubeMesh;
    cubeMesh.createMesh(finalVerts, indices, finalVerts.size(), indices.size());
    return cubeMesh;
}

Block createMeshCube(vec3 blockPos, int blockType) {
    float xoffset = 0, yoffset = 1, xoffsetTop = 0, yoffsetTop = 0, xoffsetBottom = 0, yoffsetBottom = 0, transparency = 1.0f;

    int arbitraryoff = 0;
    if (blockType == GRASS_BLOCK) {
        xoffset = 0, yoffset = 2; xoffsetTop = 2; yoffsetTop = 0; xoffsetBottom = 2; yoffsetBottom = 2;
    }

    if (blockType == IRON_ORE) {
        xoffset = 1; yoffset = 1 + arbitraryoff;
    }

    if (blockType == STONE_BLOCK) {
        xoffset = 1; yoffset = 2;
    }

    if (blockType == DIRT_BLOCK) {
        xoffset = 2; yoffset = 0;
    }

    if (blockType == OAK_WOOD) {
        xoffset = 2, yoffset = 1; xoffsetTop = -2; yoffsetTop = 2; xoffsetBottom = -2; yoffsetBottom = 2;
    }

    if (blockType == CLOUD) {
        xoffset = 1, yoffset = 0, transparency = 0.75f;
    }

    if (blockType == OAK_PLANK) {
        xoffset = 3, yoffset = 0;
    }

    if (blockType == OAK_LEAVES) {
        xoffset = 0, yoffset = 0;
    }

    if (blockType == GRASS) {
        xoffset = 3, yoffset = 1; xoffsetTop = 0, xoffsetBottom = 0; yoffsetTop = 1, yoffsetBottom = 1;
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

    if (blockType == GRASS) {
        indices = {
            0 , 1 , 2 ,
            3 , 4 , 5
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

    Mesh cubeMesh;
    cubeMesh.createMesh(finalVerts, indices, finalVerts.size(), indices.size());
    return Block(blockPos, blockType, finalVerts, indices);
}

Mesh createMeshCube(float x, float y, float z, float scale, int blockType) {
    if (blockType == NULL) {
        return Mesh();
    }
    float xoffset = 0, yoffset = 1, xoffsetTop = 0, yoffsetTop = 0, xoffsetBottom = 0, yoffsetBottom = 0, transparency = 1.0f;

    int arbitraryoff = 0;
    float yexponent = 1.5f;

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

    if (blockType == GRASS) {
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
    }

    vector<GLfloat> finalVerts;
    for (int i = 0; i < triangle.size(); i+=3) {
        finalVerts.push_back(triangle[i + 0]);
        finalVerts.push_back(triangle[i + 1]);
        finalVerts.push_back(triangle[i + 2]);

        finalVerts.push_back(globalUVs[i + 0]);
        finalVerts.push_back(globalUVs[i + 1]);
        finalVerts.push_back(globalUVs[i + 2]);
    }

    Mesh cubeMesh;
    cubeMesh.createMesh(finalVerts, indices, finalVerts.size(), indices.size());
    return cubeMesh;
}

Mesh createMeshItem(float x, float y, float z, float scale, int itemType) {
    if (itemType == NULL) {
        return Mesh();
    }
    float xoffset = 0, yoffset = 1, transparency = 1.0f;

    int arbitraryoff = 0;
    float yexponent = 1.5f;

    if (itemType == WOODEN_PICKAXE) {
        xoffset = 0, yoffset = 0;
    }

    vector<unsigned int> indices = {
        0,  1,  2,
        3,  4,  5
    };
    float clipX = 0.03f, clipY = 0.97f;

    vector<float> triangle = {
            0.0f * scale + x, 0.0f * scale * yexponent + y, 0.0f + z,
            0.0f * scale + x, 1.0f * scale * yexponent + y, 0.0f + z,
            1.0f * scale + x, 0.0f * scale * yexponent + y, 0.0f + z,
            0.0f * scale + x, 1.0f * scale * yexponent + y, 0.0f + z,
            1.0f * scale + x, 1.0f * scale * yexponent + y, 0.0f + z,
            1.0f * scale + x, 0.0f * scale * yexponent + y, 0.0f + z,
    };

    vector<float> globalUVs = {
            (clipX + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
            (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
            (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
            (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
            (clipY + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
            (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency
    };

    vector<GLfloat> finalVerts;
    for (int i = 0; i < triangle.size(); i += 3) {
        finalVerts.push_back(triangle[i + 0]);
        finalVerts.push_back(triangle[i + 1]);
        finalVerts.push_back(triangle[i + 2]);

        finalVerts.push_back(globalUVs[i + 0]);
        finalVerts.push_back(globalUVs[i + 1]);
        finalVerts.push_back(globalUVs[i + 2]);
    }

    Mesh cubeMesh;
    cubeMesh.createMesh(finalVerts, indices, finalVerts.size(), indices.size());
    return cubeMesh;
}

Block createMeshCube(vec3 blockPos, float scale, int blockType) {
    float xoffset = 0, yoffset = 1, xoffsetTop = 0, yoffsetTop = 0, xoffsetBottom = 0, yoffsetBottom = 0, transparency = 1.0f;

    int arbitraryoff = 0;
    float yexponent = 1.5f;
    if (blockType == GRASS_BLOCK) {
        xoffset = 0, yoffset = 2; xoffsetTop = 2; yoffsetTop = 0; xoffsetBottom = 2; yoffsetBottom = 0;
    }

    if (blockType == IRON_ORE) {
        xoffset = 1; yoffset = 1 + arbitraryoff;
    }

    if (blockType == STONE_BLOCK) {
        xoffset = 1; yoffset = 2;
    }

    if (blockType == DIRT_BLOCK) {
        xoffset = 2; yoffset = 0;
    }

    if (blockType == OAK_WOOD) {
        xoffset = 2, yoffset = 1; xoffsetTop = -2; yoffsetTop = 2; xoffsetBottom = -2; yoffsetBottom = 2;
    }

    if (blockType == CLOUD) {
        xoffset = 1, yoffset = 0, transparency = 0.75f;
    }

    if (blockType == OAK_PLANK) {
        xoffset = 3, yoffset = 0;
    }

    if (blockType == OAK_LEAVES) {
        xoffset = 0, yoffset = 0;
    }

    if (blockType == GRASS) {
        xoffset = 3, yoffset = 1; xoffsetTop = 0, xoffsetBottom = 0; yoffsetTop = 1, yoffsetBottom = 1;
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
        0.0f + blockPos.x, 0.0f * scale * yexponent + blockPos.y, 0.0f * scale / 100 + blockPos.z,
        0.0f + blockPos.x, 1.0f * scale * yexponent + blockPos.y, 0.0f * scale / 100 + blockPos.z,
        0.0f + blockPos.x, 0.0f * scale * yexponent + blockPos.y, 0.1f * scale / 100 + blockPos.z,
        0.0f + blockPos.x, 1.0f * scale * yexponent + blockPos.y, 0.0f * scale / 100 + blockPos.z,
        0.0f + blockPos.x, 1.0f * scale * yexponent + blockPos.y, 0.1f * scale / 100 + blockPos.z,
        0.0f + blockPos.x, 0.0f * scale * yexponent + blockPos.y, 0.1f * scale / 100 + blockPos.z,
        1.0f + blockPos.x, 0.0f * scale * yexponent + blockPos.y, 0.0f * scale / 100 + blockPos.z,
        1.0f + blockPos.x, 0.0f * scale * yexponent + blockPos.y, 0.1f * scale / 100 + blockPos.z,
        1.0f + blockPos.x, 1.0f * scale * yexponent + blockPos.y, 0.0f * scale / 100 + blockPos.z,
        1.0f + blockPos.x, 1.0f * scale * yexponent + blockPos.y, 0.0f * scale / 100 + blockPos.z,
        1.0f + blockPos.x, 0.0f * scale * yexponent + blockPos.y, 0.1f * scale / 100 + blockPos.z,
        1.0f + blockPos.x, 1.0f * scale * yexponent + blockPos.y, 0.1f * scale / 100 + blockPos.z,
        0.0f + blockPos.x, 0.0f * scale * yexponent + blockPos.y, 0.0f * scale / 100 + blockPos.z,
        0.0f + blockPos.x, 0.0f * scale * yexponent + blockPos.y, 0.1f * scale / 100 + blockPos.z,
        1.0f + blockPos.x, 0.0f * scale * yexponent + blockPos.y, 0.0f * scale / 100 + blockPos.z,
        1.0f + blockPos.x, 0.0f * scale * yexponent + blockPos.y, 0.0f * scale / 100 + blockPos.z,
        0.0f + blockPos.x, 0.0f * scale * yexponent + blockPos.y, 0.1f * scale / 100 + blockPos.z,
        1.0f + blockPos.x, 0.0f * scale * yexponent + blockPos.y, 0.1f * scale / 100 + blockPos.z,
        0.0f + blockPos.x, 1.0f * scale * yexponent + blockPos.y, 0.0f * scale / 100 + blockPos.z,
        1.0f + blockPos.x, 1.0f * scale * yexponent + blockPos.y, 0.0f * scale / 100 + blockPos.z,
        0.0f + blockPos.x, 1.0f * scale * yexponent + blockPos.y, 0.1f * scale / 100 + blockPos.z,
        1.0f + blockPos.x, 1.0f * scale * yexponent + blockPos.y, 0.0f * scale / 100 + blockPos.z,
        1.0f + blockPos.x, 1.0f * scale * yexponent + blockPos.y, 0.1f * scale / 100 + blockPos.z,
        0.0f + blockPos.x, 1.0f * scale * yexponent + blockPos.y, 0.1f * scale / 100 + blockPos.z,
        0.0f + blockPos.x, 0.0f * scale * yexponent + blockPos.y, 0.0f * scale / 100 + blockPos.z,
        1.0f + blockPos.x, 0.0f * scale * yexponent + blockPos.y, 0.0f * scale / 100 + blockPos.z,
        0.0f + blockPos.x, 1.0f * scale * yexponent + blockPos.y, 0.0f * scale / 100 + blockPos.z,
        0.0f + blockPos.x, 1.0f * scale * yexponent + blockPos.y, 0.0f * scale / 100 + blockPos.z,
        1.0f + blockPos.x, 0.0f * scale * yexponent + blockPos.y, 0.0f * scale / 100 + blockPos.z,
        1.0f + blockPos.x, 1.0f * scale * yexponent + blockPos.y, 0.0f * scale / 100 + blockPos.z,
        0.0f + blockPos.x, 0.0f * scale * yexponent + blockPos.y, 0.1f * scale / 100 + blockPos.z,
        0.0f + blockPos.x, 1.0f * scale * yexponent + blockPos.y, 0.1f * scale / 100 + blockPos.z,
        1.0f + blockPos.x, 0.0f * scale * yexponent + blockPos.y, 0.1f * scale / 100 + blockPos.z,
        0.0f + blockPos.x, 1.0f * scale * yexponent + blockPos.y, 0.1f * scale / 100 + blockPos.z,
        1.0f + blockPos.x, 1.0f * scale * yexponent + blockPos.y, 0.1f * scale / 100 + blockPos.z,
        1.0f + blockPos.x, 0.0f * scale * yexponent + blockPos.y, 0.1f * scale / 100 + blockPos.z
    };

    if (blockType == GRASS) {
        indices = {
            0 , 1 , 2 ,
            3 , 4 , 5
        };

        triangle = {
            0.0f * scale + blockPos.x, 0.0f * scale * yexponent + blockPos.y, 0.0f * scale / 100 + blockPos.z,
            0.0f * scale + blockPos.x, 1.0f * scale * yexponent + blockPos.y, 0.0f * scale / 100 + blockPos.z,
            1.0f * scale + blockPos.x, 0.0f * scale * yexponent + blockPos.y, 0.0f * scale / 100 + blockPos.z,
            0.0f * scale + blockPos.x, 1.0f * scale * yexponent + blockPos.y, 0.0f * scale / 100 + blockPos.z,
            1.0f * scale + blockPos.x, 1.0f * scale * yexponent + blockPos.y, 0.0f * scale / 100 + blockPos.z,
            1.0f * scale + blockPos.x, 0.0f * scale * yexponent + blockPos.y, 0.0f * scale / 100 + blockPos.z,
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
    for (int i = 0; i < triangle.size(); i+=3) {
        finalVerts.push_back(triangle[i + 0]);
        finalVerts.push_back(triangle[i + 1]);
        finalVerts.push_back(triangle[i + 2]);

        finalVerts.push_back(globalUVs[i + 0]);
        finalVerts.push_back(globalUVs[i + 1]);
        finalVerts.push_back(globalUVs[i + 2]);
    }

    Block returnBlock = Block(blockPos, blockType, finalVerts, indices);
    returnBlock.blockMesh.createMesh(finalVerts, indices, finalVerts.size(), indices.size());;
    return returnBlock;
}

void createCube(float xoffset, float yoffset, float zoffset, int blockType) {
    float x = 0, y = 1, xoffsetTop = 0, yoffsetTop = 0, xoffsetBottom = 0, yoffsetBottom = 0, transparency = 1.0f;
    
    int arbitraryoff = 0;
    if (blockType == GRASS_BLOCK) {
        x = 0, y = 2; xoffsetTop = 2; yoffsetTop = 0; xoffsetBottom = 2; yoffsetBottom = 2;
    }

    if (blockType == IRON_ORE) {
        x = 1; y = 1 + arbitraryoff;
    }

    if (blockType == STONE_BLOCK) {
        x = 1; y = 2;
    }

    if (blockType == DIRT_BLOCK) {
        x = 2; y = 0;
    }

    if (blockType == OAK_WOOD) {
        x = 2, y = 1; xoffsetTop = -2; yoffsetTop = 2; xoffsetBottom = -2; yoffsetBottom = -2;
    }

    if (blockType == CLOUD) {
        x = 1, y = 0, transparency = 0.75f;
    }

    if (blockType == OAK_PLANK) {
        x = 3, y = 0;
    }

    if (blockType == OAK_LEAVES) {
        x = 0, y = 0;
    }

    if (blockType == GRASS) {
        x = 3, y = 1; xoffsetTop = 0, xoffsetBottom = 0; yoffsetTop = 1, yoffsetBottom = 1;
    }

    unsigned int indexOffset = 0;
    for (int i = 0; i < world.chunks.back().blocks.size(); i++) {
        if (world.chunks.back().blocks.size() > 0) {
            indexOffset += world.chunks.back().blocks[i].indices.size();
        }
    }
    vector<unsigned int> indices1 = {
        0 + indexOffset, 1 + indexOffset, 2 + indexOffset,
        3 + indexOffset, 4 + indexOffset, 5 + indexOffset,
        6 + indexOffset, 7 + indexOffset, 8 + indexOffset,
        9 + indexOffset, 10 + indexOffset, 11 + indexOffset,
        12 + indexOffset, 13 + indexOffset, 14 + indexOffset,
        15 + indexOffset, 16 + indexOffset, 17 + indexOffset,
        18 + indexOffset, 19 + indexOffset, 20 + indexOffset,
        21 + indexOffset, 22 + indexOffset, 23 + indexOffset,
        24 + indexOffset, 25 + indexOffset, 26 + indexOffset,
        27 + indexOffset, 28 + indexOffset, 29 + indexOffset,
        30 + indexOffset, 31 + indexOffset, 32 + indexOffset,
        33 + indexOffset, 34 + indexOffset, 35 + indexOffset,
        36 + indexOffset, 37 + indexOffset, 38 + indexOffset,
        39 + indexOffset, 40 + indexOffset, 41 + indexOffset,
        42 + indexOffset, 43 + indexOffset, 44 + indexOffset,
        45 + indexOffset, 46 + indexOffset, 47 + indexOffset
    };
    float clipX = 0.03f, clipY = 0.97f;
    vector<GLfloat> globalUVs =
    {
        (clipX + x) / xdimens,   (clipX + y) / ydimens, transparency,
        (clipX + x) / xdimens,   (clipY + y) / ydimens, transparency,
        (clipY + x) / xdimens,   (clipX + y) / ydimens, transparency,
        (clipX + x) / xdimens,   (clipY + y) / ydimens, transparency,
        (clipY + x) / xdimens,   (clipY + y) / ydimens, transparency,
        (clipY + x) / xdimens,   (clipX + y) / ydimens, transparency,

        (clipX + x) / xdimens,   (clipX + y) / ydimens, transparency,
        (clipY + x) / xdimens,   (clipX + y) / ydimens, transparency,
        (clipX + x) / xdimens,   (clipY + y) / ydimens, transparency,
        (clipX + x) / xdimens,   (clipY + y) / ydimens, transparency,
        (clipY + x) / xdimens,   (clipX + y) / ydimens, transparency,
        (clipY + x) / xdimens,   (clipY + y) / ydimens, transparency,

        (clipX + x + xoffsetBottom) / xdimens,   (clipX + y + yoffsetBottom) / ydimens, transparency,
        (clipY + x + xoffsetBottom) / xdimens,   (clipX + y + yoffsetBottom) / ydimens, transparency,
        (clipX + x + xoffsetBottom) / xdimens,   (clipY + y + yoffsetBottom) / ydimens, transparency,

        (clipX + x + xoffsetBottom) / xdimens,   (clipY + y + yoffsetBottom) / ydimens, transparency,
        (clipY + x + xoffsetBottom) / xdimens,   (clipX + y + yoffsetBottom) / ydimens, transparency,
        (clipY + x + xoffsetBottom) / xdimens,   (clipY + y + yoffsetBottom) / ydimens, transparency,

        (clipX + x + xoffsetTop) / xdimens,   (clipX + y + yoffsetTop) / ydimens, transparency,
        (clipX + x + xoffsetTop) / xdimens,   (clipY + y + yoffsetTop) / ydimens, transparency,
        (clipY + x + xoffsetTop) / xdimens,   (clipX + y + yoffsetTop) / ydimens, transparency,
        (clipX + x + xoffsetTop) / xdimens,   (clipY + y + yoffsetTop) / ydimens, transparency,
        (clipY + x + xoffsetTop) / xdimens,   (clipY + y + yoffsetTop) / ydimens, transparency,
        (clipY + x + xoffsetTop) / xdimens,   (clipX + y + yoffsetTop) / ydimens, transparency,

        (clipX + x) / xdimens,   (clipX + y) / ydimens, transparency,
        (clipY + x) / xdimens,   (clipX + y) / ydimens, transparency,
        (clipX + x) / xdimens,   (clipY + y) / ydimens, transparency,
        (clipX + x) / xdimens,   (clipY + y) / ydimens, transparency,
        (clipY + x) / xdimens,   (clipX + y) / ydimens, transparency,
        (clipY + x) / xdimens,   (clipY + y) / ydimens, transparency,

        (clipX + x) / xdimens,   (clipX + y) / ydimens, transparency,
        (clipX + x) / xdimens,   (clipY + y) / ydimens, transparency,
        (clipY + x) / xdimens,   (clipX + y) / ydimens, transparency,
        (clipX + x) / xdimens,   (clipY + y) / ydimens, transparency,
        (clipY + x) / xdimens,   (clipY + y) / ydimens, transparency,
        (clipY + x) / xdimens,   (clipX + y) / ydimens, transparency
    };
    
    vector<GLfloat> triangle = {
        0.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
        0.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
        0.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
        0.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
        0.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
        0.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,

        1.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
        1.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
        1.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
        1.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
        1.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
        1.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,

        0.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
        0.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
        1.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,

        1.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
        0.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
        1.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,

        0.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
        1.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
        0.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
        1.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
        1.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
        0.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,

        0.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
        1.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
        0.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
        0.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
        1.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
        1.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,

        0.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
        0.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
        1.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
        0.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
        1.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
        1.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
    };

    if (blockType == GRASS) {
        indices1 = {
            0 + indexOffset, 1 + indexOffset, 2 + indexOffset,
            3 + indexOffset, 4 + indexOffset, 5 + indexOffset,
            6 + indexOffset, 7 + indexOffset, 6 + indexOffset,
            9 + indexOffset, 10 + indexOffset, 11 + indexOffset,
            12 + indexOffset, 13 + indexOffset, 14 + indexOffset,
            15 + indexOffset, 16 + indexOffset, 17 + indexOffset,
            18 + indexOffset, 19 + indexOffset, 20 + indexOffset,
            21 + indexOffset, 22 + indexOffset, 23 + indexOffset,
            24 + indexOffset, 25 + indexOffset, 26 + indexOffset,
            27 + indexOffset, 28 + indexOffset, 29 + indexOffset,
            30 + indexOffset, 31 + indexOffset, 32 + indexOffset,
            33 + indexOffset, 34 + indexOffset, 35 + indexOffset,
            36 + indexOffset, 37 + indexOffset, 38 + indexOffset,
            39 + indexOffset, 40 + indexOffset, 41 + indexOffset,
            42 + indexOffset, 43 + indexOffset, 44 + indexOffset,
            45 + indexOffset, 46 + indexOffset, 47 + indexOffset
        };

        vector<float> verts2 = {
            0.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
            0.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
            1.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
            0.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
            1.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
            1.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
                                                    
            0.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
            0.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
            1.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
            0.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
            1.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
            1.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
        };

        triangle.insert(triangle.end(), verts2.begin(), verts2.end());

        globalUVs = {
            (clipX + x + xoffsetTop) / xdimens,   (clipX + y + yoffsetTop) / ydimens, transparency,
            (clipX + x + xoffsetTop) / xdimens,   (clipY + y + yoffsetTop) / ydimens, transparency,
            (clipY + x + xoffsetTop) / xdimens,   (clipX + y + yoffsetTop) / ydimens, transparency,
            (clipX + x + xoffsetTop) / xdimens,   (clipY + y + yoffsetTop) / ydimens, transparency,
            (clipY + x + xoffsetTop) / xdimens,   (clipY + y + yoffsetTop) / ydimens, transparency,
            (clipY + x + xoffsetTop) / xdimens,   (clipX + y + yoffsetTop) / ydimens, transparency,

            (clipX + x + xoffsetTop) / xdimens,   (clipX + y + yoffsetTop) / ydimens, transparency,
            (clipY + x + xoffsetTop) / xdimens,   (clipX + y + yoffsetTop) / ydimens, transparency,
            (clipX + x + xoffsetTop) / xdimens,   (clipY + y + yoffsetTop) / ydimens, transparency,
            (clipX + x + xoffsetTop) / xdimens,   (clipY + y + yoffsetTop) / ydimens, transparency,
            (clipY + x + xoffsetTop) / xdimens,   (clipX + y + yoffsetTop) / ydimens, transparency,
            (clipY + x + xoffsetTop) / xdimens,   (clipY + y + yoffsetTop) / ydimens, transparency,

            (clipX + x + xoffsetBottom) / xdimens,   (clipX + y + yoffsetBottom) / ydimens, transparency,
            (clipY + x + xoffsetBottom) / xdimens,   (clipX + y + yoffsetBottom) / ydimens, transparency,
            (clipX + x + xoffsetBottom) / xdimens,   (clipY + y + yoffsetBottom) / ydimens, transparency,

            (clipX + x + xoffsetBottom) / xdimens,   (clipY + y + yoffsetBottom) / ydimens, transparency,
            (clipY + x + xoffsetBottom) / xdimens,   (clipX + y + yoffsetBottom) / ydimens, transparency,
            (clipY + x + xoffsetBottom) / xdimens,   (clipY + y + yoffsetBottom) / ydimens, transparency,

            (clipX + x + xoffsetTop) / xdimens,   (clipX + y + yoffsetTop) / ydimens, transparency,
            (clipX + x + xoffsetTop) / xdimens,   (clipY + y + yoffsetTop) / ydimens, transparency,
            (clipY + x + xoffsetTop) / xdimens,   (clipX + y + yoffsetTop) / ydimens, transparency,
            (clipX + x + xoffsetTop) / xdimens,   (clipY + y + yoffsetTop) / ydimens, transparency,
            (clipY + x + xoffsetTop) / xdimens,   (clipY + y + yoffsetTop) / ydimens, transparency,
            (clipY + x + xoffsetTop) / xdimens,   (clipX + y + yoffsetTop) / ydimens, transparency,

            (clipX + x + xoffsetTop) / xdimens,   (clipX + y + yoffsetTop) / ydimens, transparency,
            (clipY + x + xoffsetTop) / xdimens,   (clipX + y + yoffsetTop) / ydimens, transparency,
            (clipX + x + xoffsetTop) / xdimens,   (clipY + y + yoffsetTop) / ydimens, transparency,
            (clipX + x + xoffsetTop) / xdimens,   (clipY + y + yoffsetTop) / ydimens, transparency,
            (clipY + x + xoffsetTop) / xdimens,   (clipX + y + yoffsetTop) / ydimens, transparency,
            (clipY + x + xoffsetTop) / xdimens,   (clipY + y + yoffsetTop) / ydimens, transparency,

            (clipX + x + xoffsetTop) / xdimens,   (clipX + y + yoffsetTop) / ydimens, transparency,
            (clipX + x + xoffsetTop) / xdimens,   (clipY + y + yoffsetTop) / ydimens, transparency,
            (clipY + x + xoffsetTop) / xdimens,   (clipX + y + yoffsetTop) / ydimens, transparency,
            (clipX + x + xoffsetTop) / xdimens,   (clipY + y + yoffsetTop) / ydimens, transparency,
            (clipY + x + xoffsetTop) / xdimens,   (clipY + y + yoffsetTop) / ydimens, transparency,
            (clipY + x + xoffsetTop) / xdimens,   (clipX + y + yoffsetTop) / ydimens, transparency,

            (clipX + x) / xdimens,   (clipX + y) / ydimens, transparency,
            (clipX + x) / xdimens,   (clipY + y) / ydimens, transparency,
            (clipY + x) / xdimens,   (clipX + y) / ydimens, transparency,
            (clipX + x) / xdimens,   (clipY + y) / ydimens, transparency,
            (clipY + x) / xdimens,   (clipY + y) / ydimens, transparency,
            (clipY + x) / xdimens,   (clipX + y) / ydimens, transparency,

            (clipX + x) / xdimens,   (clipX + y) / ydimens, transparency,
            (clipX + x) / xdimens,   (clipY + y) / ydimens, transparency,
            (clipY + x) / xdimens,   (clipX + y) / ydimens, transparency,
            (clipX + x) / xdimens,   (clipY + y) / ydimens, transparency,
            (clipY + x) / xdimens,   (clipY + y) / ydimens, transparency,
            (clipY + x) / xdimens,   (clipX + y) / ydimens, transparency
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

    addBlockToWorld(Block(vec3(xoffset, yoffset, zoffset), blockType, finalVerts, indices1));
    world.chunks[world.chunks.size() - 1].blockNum++;
    //world.chunks[world.chunks.size() - 1].blocks.push_back(Block(vec3(xoffset, yoffset, zoffset), blockType, finalVerts, indices1));
}

void delBlocklook_at() {
    ivec3 blockPos = lookingAtBlock();
    if (blockPos.x != 404.0f) {
        deleteBlockFromWorld(blockPos);
    }
}

void addBlocklook_at(int blockType) {
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
            if (!inf_blocks) {
                inv_slots[3][slot] = NULL;
                currInvSlot[3][slot] = Mesh();
            }
            //cout << blockType << endl;
            return;
        }
    }
}

bool clearDistantChunks(Chunk chunk) {
    if (chunk.coords.x < (camera.getCameraPos().x / CHUNK_SIZE) - 10 || chunk.coords.y < (camera.getCameraPos().z / CHUNK_SIZE) - 10) {
        return true;
    }
    return false;
}
//
void renderWorld() {
    //mat4 view = camera.calcViewMatrix();
    //mat4 projection = perspective(radians(45.0f), (float(mainWindow.getBufferWidth()) / float(mainWindow.getBufferHeight())), 0.1f, 1000.0f);
    //Frustum frustum = extractFrustumPlanes(projection * view);
    //for (int i = 0; i >= 0 && i < world.chunks.size(); i++) {
    //    //if (!isChunkVisible(frustum, world.chunks[i].coords)) {
    //    //    continue;
    //    //}
    //    //cout << i << " " << world.chunks.size() << endl;
    //    if (world.chunks[i].needUpdate) {
    //        //if (!clearDistantChunks(world.chunks[i])) {
    //        world.chunks[i].mesh.createMesh(world.chunks[i].vertices, world.chunks[i].indices, world.chunks[i].vertices.size(), world.chunks[i].indices.size());
    //        world.chunks[i].needUpdate = false;
    //        //}
    //    }
    //    //if (!clearDistantChunks(world.chunks[i])) {
    //        world.chunks[i].mesh.renderMesh();
    //    //}
    //}
  

    //for (int i = 0; i < world.chunks.size(); i++) {
    //    world.chunks[i].vertices = {};
    //    world.chunks[i].vertices = {};
    //    for (int j = 0; j < world.chunks[i].blocks.size(); j++) {
    //        if (world.chunks[i].needUpdate) {
    //            world.chunks[i].vertices.insert(world.chunks[i].vertices.end(), world.chunks[i].blocks[j].vertices.begin(), world.chunks[i].blocks[j].vertices.end());
    //            world.chunks[i].indices.insert(world.chunks[i].indices.end(), world.chunks[i].blocks[j].indices.begin(), world.chunks[i].blocks[j].indices.end());
    //            world.chunks[i].needUpdate = false;
    //        }
    //        world.chunks[i].mesh.createMesh(world.chunks[i].vertices, world.chunks[i].indices, world.chunks[i].vertices.size(), world.chunks[i].indices.size());
    //        world.chunks[i].mesh.renderMesh();
    //        /*world.chunks[i].blocks[j].blockMesh.createMesh(world.chunks[i].blocks[j].vertices, world.chunks[i].blocks[j].indices, world.chunks[i].blocks[j].vertices.size(), world.chunks[i].blocks[j].indices.size());
    //        world.chunks[i].blocks[j].blockMesh.renderMesh();*/
    //    }
    //}
    for (int i = 0; i < world.chunks.size(); i++) {
        if (world.chunks[i].needUpdate) {
            world.chunks[i].mesh.createMesh(world.chunks[i].vertices, world.chunks[i].indices, world.chunks[i].vertices.size(), world.chunks[i].indices.size());
            world.chunks[i].needUpdate = false;
        }
        world.chunks[i].mesh.renderMesh();
    }
    /*for (int i = 0; i < world.chunks.size(); i++) {
        for (int j = 0; j < world.chunks[i].blocks.size(); j++) {
            if (world.chunks[i].needUpdate) {
                world.chunks[i].blocks[j].blockMesh.createMesh(world.chunks[i].blocks[j].vertices, world.chunks[i].blocks[j].indices, world.chunks[i].blocks[j].vertices.size(), world.chunks[i].blocks[j].indices.size());
                world.chunks[i].needUpdate = false;
            }
            world.chunks[i].blocks[j].blockMesh.renderMesh();
        }
    }*/
}

void createShaders() {
    Shadergl* shader1 = new Shadergl();
    shader1->createShaderFromFiles(vshader, fshader);
    shaders.push_back(shader1);

    Shadergl* shader2 = new Shadergl();
    shader2->createShaderFromFiles(vshader2, fshader2);
    shaders.push_back(shader2);

    Shadergl* shaderInv = new Shadergl();
    shaderInv->createShaderFromFiles(vshaderInv, fshaderInv);
    shaders.push_back(shaderInv);

    Shadergl* shaderInvBlock = new Shadergl();
    shaderInvBlock->createShaderFromFiles(vshaderInvBlock, fshaderInvBlock);
    shaders.push_back(shaderInvBlock);

    for (int i = 0; i < 9; i++) {
        Shadergl* shaderInvSlot = new Shadergl();
        shaderInvSlot->createShaderFromFiles(vshaderInvSlot, fshaderInvSlot);
        shaders.push_back(shaderInvSlot);
    }

    Shadergl* shaderLooking = new Shadergl();
    shaderLooking->createShaderFromFiles(vshaderLooking, fshaderLooking);
    shaders.push_back(shaderLooking);

    for (int i = 0; i < 9 * 4; i++) {
        Shadergl* shaderInvSlot = new Shadergl();
        shaderInvSlot->createShaderFromFiles(vshaderInvSlot, fshaderInvSlot);
        InventoryShaders.push_back(shaderInvSlot);
    }

    for (int i = 0; i < 4; i++) {
        Shadergl* shaderInvSlot = new Shadergl();
        shaderInvSlot->createShaderFromFiles(vshaderInvSlot, fshaderInvSlot);
        craftInvShaders.push_back(shaderInvSlot);
    }

    Shadergl* craftedBlockShader = new Shadergl();
    craftedBlockShader->createShaderFromFiles(vshaderInvSlot, fshaderInvSlot);
    craftInvShaders.push_back(craftedBlockShader);
}

void generateChunkAt(vec2 xyChunk) {
    world.chunks.push_back(Chunk());
    float xoffsetTop = 0.0f;
    int blockType = 0;
    std::random_device rd;
    std::mt19937 gen(rd()); // Mersenne Twister engine

    int blockTypes[] = {
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3
    };
    int blockTypes2[] = {
        1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3
    };
    int blockTypes3[] = {
        5, 4
    };
    std::uniform_int_distribution<> dist(0, sizeof(blockTypes) / sizeof(int) - 1);
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
                    blockType = blockTypes3[dist3(gen)];
                }
                else if (y <= scaledHeight - 7 && y > scaledHeight - 20) {
                    blockType = blockTypes[dist(gen)];
                }
                else if (y <= scaledHeight - 20) {
                    blockType = blockTypes2[dist2(gen)];
                }

                createCube((1.0f + xoffsetTop) * (float)x, (1.0f + xoffsetTop) * (float)y - 1, (1.0f + xoffsetTop) * (float)z, blockType);
            }
        }
    }

}

void createCubeInThread(float x, float y, float z, Chunk& repChunk, int blockType) {
    float xoffset = 0, yoffset = 1, xoffsetTop = 0, yoffsetTop = 0, xoffsetBottom = 0, yoffsetBottom = 0, transparency = 1.0f;

    int arbitraryoff = 0;
    if (blockType == GRASS_BLOCK) {
        xoffset = 0, yoffset = 2; xoffsetTop = 2; yoffsetTop = 0; xoffsetBottom = 2; yoffsetBottom = 2;
    }

    if (blockType == IRON_ORE) {
        xoffset = 1; yoffset = 1 + arbitraryoff;
    }

    if (blockType == STONE_BLOCK) {
        xoffset = 1; yoffset = 2;
    }

    if (blockType == DIRT_BLOCK) {
        xoffset = 2; yoffset = 0;
    }

    if (blockType == OAK_WOOD) {
        xoffset = 2, yoffset = 1; xoffsetTop = -2; yoffsetTop = 2; xoffsetBottom = -2; yoffsetBottom = 2;
    }

    if (blockType == CLOUD) {
        xoffset = 1, yoffset = 0, transparency = 0.95f;
    }

    if (blockType == OAK_PLANK) {
        xoffset = 3, yoffset = 0;
    }

    if (blockType == OAK_LEAVES) {
        xoffset = 0, yoffset = 0;
    }

    if (blockType == GRASS) {
        xoffset = 3, yoffset = 1; xoffsetTop = 0, xoffsetBottom = 0; yoffsetTop = 1, yoffsetBottom = 1;
    }

    float clipX = 0.03f, clipY = 0.97f;

    unsigned int indexOffset = 0;
    for (int i = 0; i < repChunk.blocks.size(); i++) {
        if (repChunk.blocks.size() > 0) {
            indexOffset += repChunk.blocks[i].indices.size();
        }
    }
    vector<unsigned int> indices = {
        0 + (indexOffset), 1 + (indexOffset), 2 + (indexOffset),
        3 + (indexOffset), 4 + (indexOffset), 5 + (indexOffset),
        6 + (indexOffset), 7 + (indexOffset), 8 + (indexOffset),
        9 +  (indexOffset), 10 + (indexOffset), 11 + (indexOffset),
        12 + (indexOffset), 13 + (indexOffset), 14 + (indexOffset),
        15 + (indexOffset), 16 + (indexOffset), 17 + (indexOffset),
        18 + (indexOffset), 19 + (indexOffset), 20 + (indexOffset),
        21 + (indexOffset), 22 + (indexOffset), 23 + (indexOffset),
        24 + (indexOffset), 25 + (indexOffset), 26 + (indexOffset),
        27 + (indexOffset), 28 + (indexOffset), 29 + (indexOffset),
        30 + (indexOffset), 31 + (indexOffset), 32 + (indexOffset),
        33 + (indexOffset), 34 + (indexOffset), 35 + (indexOffset)
    };

    vector<GLfloat> globalUVs =
    {
        (clipX + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,

        (clipX + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,

        (clipX + xoffset + xoffsetBottom) / xdimens,   (clipX + yoffset + yoffsetBottom) / ydimens, transparency,
        (clipY + xoffset + xoffsetBottom) / xdimens,   (clipX + yoffset + yoffsetBottom) / ydimens, transparency,
        (clipX + xoffset + xoffsetBottom) / xdimens,   (clipY + yoffset + yoffsetBottom) / ydimens, transparency,

        (clipX + xoffset + xoffsetBottom) / xdimens,   (clipY + yoffset + yoffsetBottom) / ydimens, transparency,
        (clipY + xoffset + xoffsetBottom) / xdimens,   (clipX + yoffset + yoffsetBottom) / ydimens, transparency,
        (clipY + xoffset + xoffsetBottom) / xdimens,   (clipY + yoffset + yoffsetBottom) / ydimens, transparency,

        (clipX + xoffset + xoffsetTop) / xdimens,   (clipX + yoffset + yoffsetTop) / ydimens, transparency,
        (clipX + xoffset + xoffsetTop) / xdimens,   (clipY + yoffset + yoffsetTop) / ydimens, transparency,
        (clipY + xoffset + xoffsetTop) / xdimens,   (clipX + yoffset + yoffsetTop) / ydimens, transparency,
        (clipX + xoffset + xoffsetTop) / xdimens,   (clipY + yoffset + yoffsetTop) / ydimens, transparency,
        (clipY + xoffset + xoffsetTop) / xdimens,   (clipY + yoffset + yoffsetTop) / ydimens, transparency,
        (clipY + xoffset + xoffsetTop) / xdimens,   (clipX + yoffset + yoffsetTop) / ydimens, transparency,

        (clipX + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,

        (clipX + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
        (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
        (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency
    };

    vector<GLfloat> triangle = {
        0.0f + x, 0.0f + y, 0.0f + z,
        0.0f + x, 1.0f + y, 0.0f + z,
        0.0f + x, 0.0f + y, 1.0f + z,
        0.0f + x, 1.0f + y, 0.0f + z,
        0.0f + x, 1.0f + y, 1.0f + z,
        0.0f + x, 0.0f + y, 1.0f + z,

        1.0f + x, 0.0f + y, 0.0f + z,
        1.0f + x, 0.0f + y, 1.0f + z,
        1.0f + x, 1.0f + y, 0.0f + z,
        1.0f + x, 1.0f + y, 0.0f + z,
        1.0f + x, 0.0f + y, 1.0f + z,
        1.0f + x, 1.0f + y, 1.0f + z,

        0.0f + x, 0.0f + y, 0.0f + z,
        0.0f + x, 0.0f + y, 1.0f + z,
        1.0f + x, 0.0f + y, 0.0f + z,
        1.0f + x, 0.0f + y, 0.0f + z,
        0.0f + x, 0.0f + y, 1.0f + z,
        1.0f + x, 0.0f + y, 1.0f + z,

        0.0f + x, 1.0f + y, 0.0f + z,
        1.0f + x, 1.0f + y, 0.0f + z,
        0.0f + x, 1.0f + y, 1.0f + z,
        1.0f + x, 1.0f + y, 0.0f + z,
        1.0f + x, 1.0f + y, 1.0f + z,
        0.0f + x, 1.0f + y, 1.0f + z,

        0.0f + x, 0.0f + y, 0.0f + z,
        1.0f + x, 0.0f + y, 0.0f + z,
        0.0f + x, 1.0f + y, 0.0f + z,
        0.0f + x, 1.0f + y, 0.0f + z,
        1.0f + x, 0.0f + y, 0.0f + z,
        1.0f + x, 1.0f + y, 0.0f + z,

        0.0f + x, 0.0f + y, 1.0f + z,
        0.0f + x, 1.0f + y, 1.0f + z,
        1.0f + x, 0.0f + y, 1.0f + z,
        0.0f + x, 1.0f + y, 1.0f + z,
        1.0f + x, 1.0f + y, 1.0f + z,
        1.0f + x, 0.0f + y, 1.0f + z
    };
    if (blockType == GRASS) {
        indices = {
            0 + indexOffset, 1 + indexOffset, 2 + indexOffset,
            3 + indexOffset, 4 + indexOffset, 5 + indexOffset,
            6 + indexOffset, 7 + indexOffset, 6 + indexOffset,
            9 + indexOffset, 10 + indexOffset, 11 + indexOffset,
            12 + indexOffset, 13 + indexOffset, 14 + indexOffset,
            15 + indexOffset, 16 + indexOffset, 17 + indexOffset,
            18 + indexOffset, 19 + indexOffset, 20 + indexOffset,
            21 + indexOffset, 22 + indexOffset, 23 + indexOffset,
            24 + indexOffset, 25 + indexOffset, 26 + indexOffset,
            27 + indexOffset, 28 + indexOffset, 29 + indexOffset,
            30 + indexOffset, 31 + indexOffset, 32 + indexOffset,
            33 + indexOffset, 34 + indexOffset, 35 + indexOffset,
            36 + indexOffset, 37 + indexOffset, 38 + indexOffset,
            39 + indexOffset, 40 + indexOffset, 41 + indexOffset,
            42 + indexOffset, 43 + indexOffset, 44 + indexOffset,
            45 + indexOffset, 46 + indexOffset, 47 + indexOffset
        };

        vector<float> verts2 = {
            0.0f + x, 0.0f + y, 0.0f + z,
            0.0f + x, 1.0f + y, 0.0f + z,
            1.0f + x, 0.0f + y, 1.0f + z,
            0.0f + x, 1.0f + y, 0.0f + z,
            1.0f + x, 1.0f + y, 1.0f + z,
            1.0f + x, 0.0f + y, 1.0f + z,

            0.0f + x, 0.0f + y, 1.0f + z,
            0.0f + x, 1.0f + y, 1.0f + z,
            1.0f + x, 0.0f + y, 0.0f + z,
            0.0f + x, 1.0f + y, 1.0f + z,
            1.0f + x, 1.0f + y, 0.0f + z,
            1.0f + x, 0.0f + y, 0.0f + z,
        };

        triangle.insert(triangle.end(), verts2.begin(), verts2.end());

        globalUVs = {
            (clipX + xoffset + xoffsetTop) / xdimens,   (clipX + yoffset + yoffsetTop) / ydimens, transparency,
            (clipX + xoffset + xoffsetTop) / xdimens,   (clipY + yoffset + yoffsetTop) / ydimens, transparency,
            (clipY + xoffset + xoffsetTop) / xdimens,   (clipX + yoffset + yoffsetTop) / ydimens, transparency,
            (clipX + xoffset + xoffsetTop) / xdimens,   (clipY + yoffset + yoffsetTop) / ydimens, transparency,
            (clipY + xoffset + xoffsetTop) / xdimens,   (clipY + yoffset + yoffsetTop) / ydimens, transparency,
            (clipY + xoffset + xoffsetTop) / xdimens,   (clipX + yoffset + yoffsetTop) / ydimens, transparency,

            (clipX + xoffset + xoffsetTop) / xdimens,   (clipX + yoffset + yoffsetTop) / ydimens, transparency,
            (clipY + xoffset + xoffsetTop) / xdimens,   (clipX + yoffset + yoffsetTop) / ydimens, transparency,
            (clipX + xoffset + xoffsetTop) / xdimens,   (clipY + yoffset + yoffsetTop) / ydimens, transparency,
            (clipX + xoffset + xoffsetTop) / xdimens,   (clipY + yoffset + yoffsetTop) / ydimens, transparency,
            (clipY + xoffset + xoffsetTop) / xdimens,   (clipX + yoffset + yoffsetTop) / ydimens, transparency,
            (clipY + xoffset + xoffsetTop) / xdimens,   (clipY + yoffset + yoffsetTop) / ydimens, transparency,

            (clipX + xoffset + xoffsetBottom) / xdimens,   (clipX + yoffset + yoffsetBottom) / ydimens, transparency,
            (clipY + xoffset + xoffsetBottom) / xdimens,   (clipX + yoffset + yoffsetBottom) / ydimens, transparency,
            (clipX + xoffset + xoffsetBottom) / xdimens,   (clipY + yoffset + yoffsetBottom) / ydimens, transparency,
            (clipX + xoffset + xoffsetBottom) / xdimens,   (clipY + yoffset + yoffsetBottom) / ydimens, transparency,
            (clipY + xoffset + xoffsetBottom) / xdimens,   (clipX + yoffset + yoffsetBottom) / ydimens, transparency,
            (clipY + xoffset + xoffsetBottom) / xdimens,   (clipY + yoffset + yoffsetBottom) / ydimens, transparency,

            (clipX + xoffset + xoffsetTop) / xdimens,   (clipX + yoffset + yoffsetTop) / ydimens, transparency,
            (clipX + xoffset + xoffsetTop) / xdimens,   (clipY + yoffset + yoffsetTop) / ydimens, transparency,
            (clipY + xoffset + xoffsetTop) / xdimens,   (clipX + yoffset + yoffsetTop) / ydimens, transparency,
            (clipX + xoffset + xoffsetTop) / xdimens,   (clipY + yoffset + yoffsetTop) / ydimens, transparency,
            (clipY + xoffset + xoffsetTop) / xdimens,   (clipY + yoffset + yoffsetTop) / ydimens, transparency,
            (clipY + xoffset + xoffsetTop) / xdimens,   (clipX + yoffset + yoffsetTop) / ydimens, transparency,

            (clipX + xoffset + xoffsetTop) / xdimens,   (clipX + yoffset + yoffsetTop) / ydimens, transparency,
            (clipY + xoffset + xoffsetTop) / xdimens,   (clipX + yoffset + yoffsetTop) / ydimens, transparency,
            (clipX + xoffset + xoffsetTop) / xdimens,   (clipY + yoffset + yoffsetTop) / ydimens, transparency,
            (clipX + xoffset + xoffsetTop) / xdimens,   (clipY + yoffset + yoffsetTop) / ydimens, transparency,
            (clipY + xoffset + xoffsetTop) / xdimens,   (clipX + yoffset + yoffsetTop) / ydimens, transparency,
            (clipY + xoffset + xoffsetTop) / xdimens,   (clipY + yoffset + yoffsetTop) / ydimens, transparency,

            (clipX + xoffset + xoffsetTop) / xdimens,   (clipX + yoffset + yoffsetTop) / ydimens, transparency,
            (clipX + xoffset + xoffsetTop) / xdimens,   (clipY + yoffset + yoffsetTop) / ydimens, transparency,
            (clipY + xoffset + xoffsetTop) / xdimens,   (clipX + yoffset + yoffsetTop) / ydimens, transparency,
            (clipX + xoffset + xoffsetTop) / xdimens,   (clipY + yoffset + yoffsetTop) / ydimens, transparency,
            (clipY + xoffset + xoffsetTop) / xdimens,   (clipY + yoffset + yoffsetTop) / ydimens, transparency,
            (clipY + xoffset + xoffsetTop) / xdimens,   (clipX + yoffset + yoffsetTop) / ydimens, transparency,

            (clipX + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
            (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
            (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
            (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
            (clipY + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
            (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,

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

    for (int i = 0; i < finalVerts.size(); i++) {
        repChunk.vertices.push_back(finalVerts[i]);
    }
    for (int i = 0; i < indices.size(); i++) {
        repChunk.indices.push_back(indices[i]);
    }
    repChunk.needUpdate = true;   
    repChunk.blocks.push_back(Block(vec3(x, y, z), blockType, finalVerts, indices));
    repChunk.blockNum++;
}

void generateChunkAt(vec2 xyChunk, Chunk &repChunk) {
    float xoffsetTop = 0.0f;
    int blockType = 0;
    std::random_device rd;
    std::mt19937 gen(rd()); // Mersenne Twister engine

    int blockTypes1[] = {
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3
    };
    int blockTypes2[] = {
        1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3
    };
    int blockTypes3[] = {
        5, 4
    };
    std::uniform_int_distribution<> dist(0, sizeof(blockTypes1) / sizeof(int) - 1);
    std::uniform_int_distribution<> dist2(0, sizeof(blockTypes2) / sizeof(int) - 1);
    std::uniform_int_distribution<> dist3(0, sizeof(blockTypes3) / sizeof(int) - 1);

    for (int x = (xyChunk.x) * CHUNK_SIZE; x < (xyChunk.x + 1) * (CHUNK_SIZE); x++) {
        for (int z = (xyChunk.y) * CHUNK_SIZE; z < (xyChunk.y + 1) * (CHUNK_SIZE); z++) {
            //float total = 0;
            //int samples = 5000;
            //for (int i = 0; i < samples; i++) {
            //    total += static_cast<float>(rand()) / RAND_MAX;
            //}
            //float totalrand = total / samples;
            FastNoiseLite noise;
            //noise.SetSeed(42);   
            noise.SetFrequency(0.005f);
            float height = noise.GetNoise((float)x, (float)z); // returns value in range [-1, 1]
            float scaledHeight = (height + 1.0f) * 0.45f * CHUNK_SIZE * CHUNK_SIZE + 5.0f;

            FastNoiseLite caveNoise;
            caveNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
            caveNoise.SetFrequency(0.05f);       
            FastNoiseLite cloudNoise;
            cloudNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
            cloudNoise.SetFrequency(0.04);
            
            for (int y = 0; y < scaledHeight; y++) {
                float treeHeight = randomFloat(3.0, 10.0), treeDistrib = rand();
                if (y > scaledHeight - 1) {
                    blockType = GRASS_BLOCK;
                    if (treeDistrib > 0.0 && treeDistrib <= 100) {

                        for (int i = y; i < y + treeHeight; i++) {
                            if (!blockExistsAt(vec3(x, i, z))) {
                                createCubeInThread(x, (int)i, z, repChunk, OAK_WOOD);
                                //createCubeInThread(x, (int)i + 10, z, repChunk, OAK_PLANK);
                            }

                        }
                        glm::ivec3 center(x, y + treeHeight, z);

                        int radius = 3; // adjust for size

                        for (int dx = -radius; dx <= radius; ++dx) {
                            for (int dy = -radius; dy <= radius; ++dy) {
                                for (int dz = -radius; dz <= radius; ++dz) {
                                    glm::ivec3 pos = center + glm::ivec3(dx, dy, dz);
                                    float dist = glm::length(glm::vec3(dx, dy, dz));

                                    if (dist <= radius + randomFloat(-0.3f, 0.3f)) {
                                        createCubeInThread(pos.x, pos.y, pos.z, repChunk, OAK_LEAVES);
                                    }
                                }
                            }
                        }
                    }
                    
                }
                else if (y > scaledHeight - 4 && y <= scaledHeight - 1) {
                    blockType = DIRT_BLOCK;
                }
                else if (y <= scaledHeight - 4 && y > scaledHeight - 7) {
                    blockType = blockTypes3[dist3(gen)];
                }
                else if (y <= scaledHeight - 7 && y > scaledHeight - 20) {
                    blockType = blockTypes1[dist(gen)];
                }
                else if (y <= scaledHeight - 20) {
                    blockType = blockTypes2[dist2(gen)];
                }

                float cloudDensity = cloudNoise.GetNoise((float)x, (float)y, (float)z);
                float density = caveNoise.GetNoise((float)x, (float)y, (float)z);

                if (cloudDensity < -0.5f) {
                    createCubeInThread(x, CHUNK_SIZE * CHUNK_SIZE + 20, z, repChunk, CLOUD);
                }

                if (density < -0.4f) {
                    continue;
                }

                createCubeInThread(x, y, z, repChunk, blockType);
                if (blockType == GRASS_BLOCK) {
                    for (int i = 0; i < repChunk.blocks.size(); i++) {
                        if (repChunk.blocks[i].position == vec3(x, (int)scaledHeight, z)) {
                            if (rand() < 2000.0) {
                                createCubeInThread(x, (int)scaledHeight + 1, z, repChunk, GRASS);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
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
        generateChunkAt(coord, newChunk);

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
            } else {
                continue;
            }
        }

        Chunk newChunk;
        generateChunkAt(coord, newChunk);

        newChunk.coords = chunkCoords.back();
        newChunk.needUpdate = true;
        //cout << newChunk.blockNum << endl;
        {
            std::lock_guard<std::mutex> lock(chunkResultMutex2);
            chunkResultQueue2.push(std::move(newChunk));
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
            delBlocklook_at();
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
            addBlocklook_at(inv_slots[3][slot]);
            blockPlacingOut = false;
        }
    }
});

int getRecipe(int slots[2][2]) {
    for (int i = 0; i < recipes.size(); i+=5) {
        if (slots[0][0] == recipes[i] && slots[0][1] == recipes[i + 1] && slots[1][0] == recipes[i + 2] && slots[1][1] == recipes[i + 3]) {
            return recipes[i + 4];
        }
    }
    return 0;
}

int main()
{
    mainWindow = Window(WIDTH, HEIGHT);
    mainWindow.initialize();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    /*Textures.push_back(new Texturegl("textures\\block_atlas_4.png"));*/
    Textures.push_back(new Texturegl("textures\\block_atlas_13.png"));
    Textures.push_back(new Texturegl("textures\\clear_toolbar_2.png"));
    Textures.push_back(new Texturegl("textures\\clear_toolbar_3.png"));
    Textures.push_back(new Texturegl("textures\\main_inventory.jpg"));
    //Textures.push_back(new Texturegl("textures\\inventory_base.png"));

    Textures[0]->loadTexture();
    Textures[1]->loadTexture();
    Textures[2]->loadTexture();
    Textures[3]->loadTexture();

    createShaders();

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glFrontFace(GL_CW);
    
    chunkCoords.push_back({ 0, 0 });
    //generateChunkWith(1);
    {
        chunkGenRunning = true;
        std::lock_guard<std::mutex> lock(chunkRequestMutex);
        chunkRequestQueue.push(chunkCoords.back());
    }
    {
        std::lock_guard<std::mutex> lock(chunkResultMutex);
        //cout << chunkResultQueue.empty() << endl;
        while (!chunkResultQueue.empty()) {
            Chunk chunk = std::move(chunkResultQueue.front());
            chunkResultQueue.pop();
            world.chunks.push_back(std::move(chunk));
        }
    }

    camera.setCameraPos(vec3(CHUNK_SIZE / 2, CHUNK_SIZE * CHUNK_SIZE + 5, CHUNK_SIZE / 2));

    float centerX = WIDTH / 2.0f;
    float centerY = HEIGHT / 2.0;
    float size = 10.0f;

    std::vector<glm::vec2> crosshairVertices = {
        { centerX - size, centerY }, { centerX + size, centerY }, // horizontal line
        { centerX, centerY - 2 * size }, { centerX, centerY + 2 * size }  // vertical line
    };

    vector<GLfloat> vers = {
        crosshairVertices[0].x, crosshairVertices[0].y, 0.0, 0.0f, 0.0f,
        crosshairVertices[1].x, crosshairVertices[1].y, 0.0, 1.0f, 0.0f,
        crosshairVertices[2].x, crosshairVertices[2].y, 0.0, 0.0f, 1.0f,
        crosshairVertices[3].x, crosshairVertices[3].y, 0.0, 1.0f, 1.0f
    };

    vector<unsigned int> inds = {
        0, 1, 2,
        1, 2, 3
    };

    Mesh crosshair;
    crosshair.createMesh(vers, inds, 20, 6);

    int invSizeX = 310;
    int invSizeY = 65;
    int invHeight = 700;
    int slotLength = 69;
    std::vector<glm::vec2> inventoryVertices = {
        { centerX - invSizeX, centerY - invSizeY - invHeight}, { centerX + invSizeX, centerY - invSizeY - invHeight},
        { centerX - invSizeX, centerY + invSizeY - invHeight}, { centerX + invSizeX, centerY + invSizeY - invHeight},

        { centerX - invSizeX + slotLength, centerY - invSizeY - invHeight}, { centerX - invSizeX + slotLength, centerY + invSizeY - invHeight}
    };
    vector<GLfloat> versInv = {
        inventoryVertices[0].x, inventoryVertices[0].y, 0.0, 0.0f, 0.0f, 1.0f,
        inventoryVertices[1].x, inventoryVertices[1].y, 0.0, 1.0f, 0.0f, 1.0f,
        inventoryVertices[2].x, inventoryVertices[2].y, 0.0, 0.0f, 1.0f, 1.0f,
        inventoryVertices[3].x, inventoryVertices[3].y, 0.0, 1.0f, 1.0f, 1.0f
    };

    vector<unsigned int> indsInv = {
        0, 2, 1,
        1, 2, 3
    };

    int offsetX = 600, offsetY = -800;

    vector<GLfloat> versInvBlock = {
        centerX + offsetX + 0.0f,   centerY + offsetY + 0.0f, 0.0, 0.0f, 0.0f, 1.0f,
        centerX + offsetX + 100.0f, centerY + offsetY + 0.0f, 0.0, 1.0f, 0.0f, 1.0f,
        centerX + offsetX + 0.0f,   centerY + offsetY + 100.0f, 0.0, 0.0f, 1.0f, 1.0f,
        centerX + offsetX + 100.0f, centerY + offsetY + 100.0f, 0.0, 1.0f, 1.0f, 1.0f
    };

    vector<unsigned int> indsInvBlock = {
        0, 1, 2,
        1, 3, 2
    };

    Mesh inventory, currentBlock, currInvSlotSelector;

    mat4 model(1.0f), projection(1.0f), view(1.0f);
    //mat4 modelCur(1.0f);
    projection = perspective(radians(45.0f), (float(mainWindow.getBufferWidth()) / float(mainWindow.getBufferHeight())), 0.1f, 500.0f);    

    glm::mat4 ortho = glm::ortho(0.0f, float(WIDTH), 0.0f, float(HEIGHT));

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, crosshairVertices.size() * sizeof(glm::vec2), &crosshairVertices[0], GL_STATIC_DRAW);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    //world.chunks.push_back(Chunk());
    //createCube((int)camera.getCameraPos().x, (int)camera.getCameraPos().y - 3, (int)camera.getCameraPos().z, 6);
    //deleteBlockFromWorld(0.0f, 0.0f, 0.0f, 8);
    int offs = 1, ctrl = 0;
    inventory.createMesh(versInv, indsInv, 24, 6);
    mat4 modelCurSlots[4][9];
    mat4 modelCraftInvSlots[2][2];
    mat4 modelCraftedInvSlot(1.0f);
    mat4 modelCurSlotsMain[9];
    mat4 modelCurSlot(1.0f);
    mat4 modelCur(1.0f);
    for (int j = 0; j < 9; j++) {
        modelCurSlotsMain[j] = mat4(1.0f);
        modelCurSlotsMain[j] = translate(modelCurSlotsMain[j], vec3((centerX - 40.0f + j * 137) / WIDTH, 2 * (centerY + offsetY + 70.0f) / HEIGHT, 0.0f));
    }
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 9; j++) {
            modelCurSlots[i][j] = mat4(1.0f);
            modelCurSlots[i][j] = translate(modelCurSlots[i][j], vec3((centerX + 18.0f + (float)j * 123) / WIDTH, 2 * (centerY + offsetY + 323.0f + (3 - i) * 10 + 80.0f) / HEIGHT, 0.0f));
        }
    }
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            modelCraftInvSlots[i][j] = mat4(1.0f);
            modelCraftInvSlots[i][j] = translate(modelCraftInvSlots[i][j], vec3((centerX + 18.0f + (float)j * 123) / WIDTH, 2 * (centerY + offsetY + 323.0f + (3 - i) * 100 + 80.0f) / HEIGHT, 0.0f));
        }
    }
    modelCraftedInvSlot = translate(modelCraftedInvSlot, vec3((centerX + 18.0f + 300.0f) / WIDTH, 2 * (centerY + offsetY + 323.0f + 150 + 80.0f) / HEIGHT, 0.0f));
    modelCurSlot = rotate(modelCurSlot, radians(-30.0f), vec3(1.0f, 0.0f, 0.0f));
    int firstChunk = 0;
    int renderX = 1, renderY = 1;
    //world.chunks.push_back(Chunk());
    invSizeX = centerX / 3;
    invSizeY = centerY / 2;
    invHeight = 0;
    slotLength = 69;
    std::vector<glm::vec2> inventoryVertices2 = {
        { centerX - invSizeX, centerY - invSizeY - invHeight}, { centerX + invSizeX, centerY - invSizeY - invHeight},
        { centerX - invSizeX, centerY + invSizeY - invHeight}, { centerX + invSizeX, centerY + invSizeY - invHeight},

        { centerX - invSizeX + slotLength, centerY - invSizeY - invHeight}, { centerX - invSizeX + slotLength, centerY + invSizeY - invHeight}
    };
    vector<GLfloat> versInv2 = {
        inventoryVertices2[0].x, inventoryVertices2[0].y, 0.0, 0.0f, 0.0f, 1.0f,
        inventoryVertices2[1].x, inventoryVertices2[1].y, 0.0, 1.0f, 0.0f, 1.0f,
        inventoryVertices2[2].x, inventoryVertices2[2].y, 0.0, 0.0f, 1.0f, 1.0f,
        inventoryVertices2[3].x, inventoryVertices2[3].y, 0.0, 1.0f, 1.0f, 1.0f
    };

    vector<unsigned int> indsInv2 = {
        0, 2, 1,
        1, 2, 3
    };
    Mesh mainInventory;
    Block craftedItem(vec3(0.0f), 0, {}, {});
    mainInventory.createMesh(versInv2, indsInv2, versInv2.size(), indsInv2.size());
    while (!mainWindow.getShouldClose()) {
        int dimensions = 10;

        for (int i = 0; i < renderX; i++) {
            for (int j = 0; j < renderY; j += 2) {

                if (find(chunkCoords.begin(), chunkCoords.end(), vec2((int)(camera.getCameraPos().x / CHUNK_SIZE) + i, (int)(camera.getCameraPos().z / CHUNK_SIZE) + j)) == chunkCoords.end() || (chunkCoords[chunkCoords.size() - 1].x == 0 && chunkCoords[chunkCoords.size() - 1].y == 0) &&
                    find(chunkCoords.begin(), chunkCoords.end(), vec2((int)(camera.getCameraPos().x / CHUNK_SIZE) + i, (int)(camera.getCameraPos().z / CHUNK_SIZE) + j + 1)) == chunkCoords.end() || (chunkCoords[chunkCoords.size() - 1].x == 0 && chunkCoords[chunkCoords.size() - 1].y == 0)
                    ) {
                    chunkCoords.push_back({ int(camera.getCameraPos().x / CHUNK_SIZE) + i, int(camera.getCameraPos().z / CHUNK_SIZE) + j });
                    //generateChunkAt(chunkCoords[chunkCoords.size() - 1]);
                    {
                        chunkGenRunning = true;
                        std::lock_guard<std::mutex> lock(chunkRequestMutex);
                        chunkRequestQueue.push(chunkCoords.back());
                    }
                    chunkCoords.push_back({ int(camera.getCameraPos().x / CHUNK_SIZE) + i, int(camera.getCameraPos().z / CHUNK_SIZE) + j + 1 });
                    //generateChunkAt(chunkCoords[chunkCoords.size() - 1]);
                    {
                        chunkGenRunning2 = true;
                        std::lock_guard<std::mutex> lock(chunkRequestMutex2);
                        chunkRequestQueue2.push(chunkCoords.back());
                    }
                }

            }

        }
        int renderDistance = CHUNK_SIZE * 3;
        if (renderX < renderDistance) {
            renderX++;
            renderY++;
        }
        else {
            renderX = renderDistance;
            renderY = renderDistance;
        }
        {
            std::lock_guard<std::mutex> lock(chunkResultMutex);
            //cout << chunkResultQueue.empty() << endl;
            while (!chunkResultQueue.empty()) {
                Chunk chunk = std::move(chunkResultQueue.front());
                chunkResultQueue.pop();
                world.chunks.push_back(std::move(chunk));
            }
        }
        {
            std::lock_guard<std::mutex> lock(chunkResultMutex2);
            //cout << chunkResultQueue.empty() << endl;
            while (!chunkResultQueue2.empty()) {
                Chunk chunk = std::move(chunkResultQueue2.front());
                chunkResultQueue2.pop();
                world.chunks.push_back(std::move(chunk));
            }
        }

        Textures[0]->useTexture();
        shaders[0]->useShader();
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        lastTime = now;

        glfwPollEvents();

        glClearColor(0.45f, 0.75f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        int sensitivity = 1.01f;
        camera.keyControl(mainWindow.getKeys(), deltaTime);
        if (inventoryOn) {
            glfwSetInputMode(mainWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            camera.mouseControl(0.0f, 0.0f);
        }
        else {
            glfwSetInputMode(mainWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
        }

        view = camera.calcViewMatrix();
        glUniformMatrix4fv(shaders[0]->getModelLocation(), 1, GL_FALSE, value_ptr(model));
        glUniformMatrix4fv(shaders[0]->getViewLocation(), 1, GL_FALSE, value_ptr(view));
        glUniformMatrix4fv(shaders[0]->getProjectionLocation(), 1, GL_FALSE, value_ptr(projection));


        if (mainWindow.getKeys()[GLFW_KEY_SPACE]) {
            //currentBlockType = 1;
            //inv_change = true;qq  
            ctrl = 0;
        }
        else {
            ctrl = 20;
        }
        if (mainWindow.getKeys()[GLFW_KEY_1]) {
            currentBlockType = 1;
            slot = 0;
            inv_change = true;
        }
        else if (mainWindow.getKeys()[GLFW_KEY_2]) {
            currentBlockType = 2;
            slot = 1;
            inv_change = true;
        }
        else if (mainWindow.getKeys()[GLFW_KEY_3]) {
            currentBlockType = 3;
            slot = 2;
            inv_change = true;
        }
        else if (mainWindow.getKeys()[GLFW_KEY_4]) {
            currentBlockType = 4;
            slot = 3;
            inv_change = true;
        }
        else if (mainWindow.getKeys()[GLFW_KEY_5]) {
            currentBlockType = 5;
            slot = 4;
            inv_change = true;
        }
        else if (mainWindow.getKeys()[GLFW_KEY_6]) {
            currentBlockType = 6;
            slot = 5;
            inv_change = true;
        }
        else if (mainWindow.getKeys()[GLFW_KEY_7]) {
            //currentBlockType = 6;
            slot = 6;
            inv_change = true;
        }
        else if (mainWindow.getKeys()[GLFW_KEY_8]) {
            //currentBlockType = 6;
            slot = 7;
            inv_change = true;
        }
        else if (mainWindow.getKeys()[GLFW_KEY_9]) {
            //currentBlockType = 6;
            slot = 8;
            inv_change = true;
        }

        if (mainWindow.getKeys()[GLFW_KEY_I]) {
            //currentBlockType = 6;
            inf_blocks = true;
        }
        else if (mainWindow.getKeys()[GLFW_KEY_N]) {
            inf_blocks = false;
        }
        //slot = currentBlockType - 1;
        if (mainWindow.getKeys()[GLFW_KEY_P]) {
            if (mainWindow.getKeys()[GLFW_KEY_1])
                addBlocklook_at(1);
            else if (mainWindow.getKeys()[GLFW_KEY_2])
                addBlocklook_at(2);
            else if (mainWindow.getKeys()[GLFW_KEY_3])
                addBlocklook_at(3);
            else if (mainWindow.getKeys()[GLFW_KEY_4])
                addBlocklook_at(4);
            else if (mainWindow.getKeys()[GLFW_KEY_5])
                addBlocklook_at(5);
            else if (mainWindow.getKeys()[GLFW_KEY_6])
                addBlocklook_at(6);
            else if (mainWindow.getKeys()[GLFW_KEY_7])
                addBlocklook_at(7);
            else if (mainWindow.getKeys()[GLFW_KEY_8])
                addBlocklook_at(8);
            else if (mainWindow.getKeys()[GLFW_KEY_9])
                addBlocklook_at(9);
        }

        if (mainWindow.getKeys()[GLFW_KEY_T] || mainWindow.leftClickButtonPressed()) {
            {
                std::lock_guard<std::mutex> lock(breakReqMutex);
                breakReqQueue.push(vec3(1.0f));
                if (!blockBreakingOut) {
                    blockBreakingOut = true;
                }
            }
            {
                std::lock_guard<std::mutex> lock(breakResMutex);
                if (!breakResQueue.empty() && blockBreakingOut) {
                    breakResQueue.pop();
                }
            }
        }

        if (mainWindow.rightClickButtonPressed()) {
            if (inv_slots[3][slot] != NULL) {
                {
                    blockPlacingOut = true;
                    std::lock_guard<std::mutex> lock(placeReqMutex);
                    placeReqQueue.push(vec3(1.0f));
                }
                {
                    std::lock_guard<std::mutex> lock(placeResMutex);
                    if (!placeResQueue.empty()) {
                        placeResQueue.pop();
                    }
                }
            }
        }

        if (mainWindow.getShouldClose()) {
            chunkGenRunning = false;
            chunkGenRunning2 = false;
            blockPlacing = false;
            blockBreaking = false;
            chunkGenThread.join();
            chunkGenThread2.join();
            blockBreakThread.join();
            blockPlaceThread.join();
        }
        if (mainWindow.getKeys()[GLFW_KEY_Q]) {
            inv_slots[3][slot] = NULL;
            currInvSlot[3][slot].clearMesh();
            inv_change = true;
        }

        glEnable(GL_DEPTH_TEST);
        renderWorld(); // your blocks, terrain, etc.
        //shaders[13]->useShader();
        //view = camera.calcViewMatrix();
        //glUniformMatrix4fv(shaders[13]->getModelLocation(), 1, GL_FALSE, value_ptr(model));
        //glUniformMatrix4fv(shaders[13]->getViewLocation(), 1, GL_FALSE, value_ptr(view));
        //glUniformMatrix4fv(shaders[13]->getProjectionLocation(), 1, GL_FALSE, value_ptr(projection));

        //if (world.chunks.size() > 10) {
        //    Block cloud = world.getBlockAt(vec3(lookingAtBlock().x, lookingAtBlock().y, lookingAtBlock().z));
        //    //delBlocklook_at();
        //    //cloud.blockMesh.createMesh(cloud.vertices, cloud.indices, cloud.vertices.size(), cloud.indices.size());
        //    //cloud.blockMesh.renderMesh();
        //    cout << ;
        //}

        glDisable(GL_DEPTH_TEST); // so crosshair draws on top
        shaders[1]->useShader();
        glUniformMatrix4fv(glGetUniformLocation(shaders[1]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
        glBindVertexArray(vao);
        glLineWidth(3.0f);
        glDrawArrays(GL_LINES, 0, 4);
        Textures[1]->useTexture();

        if (inv_change)
        {
            if (inv_slots[3][slot] != NULL) {
                currentBlock = createMeshCube(centerX + offsetX, centerY + offsetY - 50.0f, 0.0f, 400.0f, inv_slots[3][slot]);
            }
            else {
                currentBlock = Mesh();
            }
            float itemHeight = 0.0;
            
            for (int i = 0; i < (sizeof(inv_slots) / sizeof(inv_slots[0])) - 2; i++) {
                for (int j = 0; j < (sizeof(inv_slots[0]) / sizeof(int)); j++) {
                    if (inv_slots[i][j] != GRASS) {
                        itemHeight = 10.0f;
                    }
                    if (currInvSlot[i][j].verts.size() == 0 && inv_slots[i][j] != NULL) {
                        currInvSlot[i][j] = createMeshCube(centerX / 5, (3 - i) * 90.0f, 0.0f, 35.0f, inv_slots[i][j]);
                    }
                }
            }

            for (int j = 0; j < (sizeof(inv_slots[0]) / sizeof(int)); j++) {
                if (currInvSlot[3][j].verts.size() == 0 && inv_slots[3][j] != NULL) {
                    currInvSlot[3][j] = createMeshCube(centerX / 5, 0.0f, 0.0f, 35.0f, inv_slots[3][j]);
                }
            }

            if (inventoryOn) {
                for (int i = 0; i < (sizeof(craftInv) / sizeof(craftInv[0])); i++) {
                    for (int j = 0; j < (sizeof(craftInv[0]) / sizeof(int)); j++) {
                        if (craftInvSlot[i][j].verts.size() == 0 && craftInv[i][j] != NULL) {
                            craftInvSlot[i][j] = createMeshCube(centerX / 5 + 270, centerY / 4 + 110, 0.0f, 35.0f, craftInv[i][j]);
                        }
                    }
                }
                if (getRecipe(craftInv) != 0) {
                    craftedItem.blockMesh = createMeshCube(centerX / 5 + 315, centerY / 4 + 200.0f, 0.0f, 35.0f, getRecipe(craftInv));
                }
                craftedItem.type = getRecipe(craftInv);

            }

            vector<GLfloat> versInvSlotSelector = {
                inventoryVertices[0].x + (slot) * 69 - 5, inventoryVertices[0].y - 5, 0.0, 0.0f, 0.0f, 1.0f,
                inventoryVertices[4].x + (slot) * 69 + 5, inventoryVertices[4].y - 5, 0.0, 0.0f, 1.0f, 1.0f,
                inventoryVertices[2].x + (slot) * 69 - 5, inventoryVertices[2].y + 5, 0.0, 1.0f, 0.0f, 1.0f,
                inventoryVertices[5].x + (slot) * 69 + 5, inventoryVertices[5].y + 5, 0.0, 1.0f, 1.0f, 1.0f
            };

            vector<unsigned int> indsInvSlotSelector = {
                0, 2, 1,
                1, 2, 3
            };
            currInvSlotSelector.createMesh(versInvSlotSelector, indsInvSlotSelector, 24, 6);

            inv_change = false;
        }

        shaders[2]->useShader();
        glUniformMatrix4fv(glGetUniformLocation(shaders[2]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
        Textures[1]->useTexture();
        inventory.renderMesh();
        Textures[3]->useTexture();
        if (mainWindow.getKeys()[GLFW_KEY_C]) {
            bool blockAdded = false;
            for (int i = (sizeof(inv_slots) / sizeof(inv_slots[3])) - 1; i >= 0; i--) {
                for (int j = 0; j < (sizeof(inv_slots[3]) / sizeof(int)); j++) {
                    if (inv_slots[i][j] == NULL) {
                        inv_slots[i][j] = craftedItem.type;
                        craftedItem.blockMesh.clearMesh();
                        for (int k = 0; k < sizeof(craftInv) / sizeof(craftInv[0]); k++) {
                            for (int l = 0; l < sizeof(craftInv[0]) / sizeof(int); l++) {
                                if (craftInv[k][l] != NULL) {
                                    craftInv[k][l] = NULL;
                                    craftInvSlot[k][l].clearMesh();
                                    inv_change = true;
                                }
                            }
                        }
                        blockAdded = true;
                        break;
                    }
                }
                if (blockAdded) {
                    break;
                }
            }
        }
        //if (mainWindow.getKeys()[GLFW_KEY_D]) {
        //    craftInv[0][0] = 0;
        //    craftInv[0][1] = 0;
        //    craftInv[1][0] = 0;
        //    craftInv[1][1] = 0;
        //}
        if (mainWindow.getKeys()[GLFW_KEY_E]) {
            inventoryOn = true;
        }
        if (mainWindow.getKeys()[GLFW_KEY_RIGHT_SHIFT]) {
            if (mainWindow.getKeys()[GLFW_KEY_E]) {
                for (int i = 0; i < (sizeof(inv_slots) / sizeof(inv_slots[3])); i++) {
                    for (int j = 0; j < (sizeof(inv_slots[3]) / sizeof(int)); j++) {
                        if (inv_slots[i][j] == NULL) {
                            for (int k = 0; k < sizeof(craftInv) / sizeof(craftInv[0]); k++) {
                                for (int l = 0; l < sizeof(craftInv[0]) / sizeof(int); l++) {
                                    if (craftInv[k][l] != NULL) {
                                        inv_slots[i][j] = craftInv[k][l];
                                        craftInv[k][l] = NULL;
                                        craftInvSlot[k][l] = Mesh();
                                        inv_change = true;
                                    }
                                }
                            }
                        }
                    }
                }
                inventoryOn = false;
            }
        }

        if (inventoryOn) {
            Textures[3]->useTexture();
            shaders[2]->useShader();
            glUniformMatrix4fv(glGetUniformLocation(shaders[2]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
            mainInventory.renderMesh();
            for (int i = 0; i < (sizeof(inv_slots) / sizeof(inv_slots[3])); i++) {
                for (int j = 0; j < (sizeof(inv_slots[3]) / sizeof(int)); j++) {
                    if (currInvSlot[i][j].verts.size() == 0 && inv_slots[i][j] != NULL) {
                        currInvSlot[i][j] = createMeshCube(centerX / 5, (3 - i) * 90 + 10, 0.0f, 35.0f, inv_slots[i][j]);
                    }
                }
            }
            for (int i = 0; i < (sizeof(inv_slots) / sizeof(inv_slots[3])); i++) {
                for (int j = 0; j < (sizeof(inv_slots[3]) / sizeof(int)); j++) {
                    Textures[0]->useTexture();
                    InventoryShaders[9 * i + j]->useShader();
                    glUniformMatrix4fv(glGetUniformLocation(InventoryShaders[9 * i + j]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
                    glUniformMatrix4fv(InventoryShaders[9 * i + j]->getModelLocation(), 1, GL_FALSE, value_ptr(modelCurSlots[i][j]));
                    currInvSlot[i][j].renderMesh();
                    inv_change = true;
                }
            }
            bool blockAdded = false, blockCrafting = false;
            if (mainWindow.getKeys()[GLFW_KEY_RIGHT_SHIFT]) {
                if (mainWindow.getKeys()[GLFW_KEY_ENTER]) {
                    for (int i = 0; i < (sizeof(inv_slots) / sizeof(inv_slots[3])); i++) {
                        for (int j = 0; j < (sizeof(inv_slots[3]) / sizeof(int)); j++) {
                            if (inv_slots[i][j] != NULL) {
                                 //craftInv[0][0] = inv_slots[i][j];
                                for (int k = 0; k < sizeof(craftInv) / sizeof(craftInv[0]); k++) {
                                    for (int l = 0; l < sizeof(craftInv[0]) / sizeof(int); l++) {
                                        if (craftInv[k][l] == NULL) {
                                            craftInv[k][l] = inv_slots[i][j];
                                            blockCrafting = true;
                                            blockAdded = true;
                                            currInvSlot[i][j] = Mesh();
                                            inv_slots[i][j] = NULL;
                                            break;
                                        }
                                    }
                                    if (blockCrafting) {
                                        break;
                                    }
                                }
                                if (blockAdded) {
                                    break;
                                }
                            }
                        }
                        if (blockAdded) {
                            break;
                        }
                    }
                }
            }
            for (int i = 0; i < (sizeof(craftInv) / sizeof(craftInv[0])); i++) {
                for (int j = 0; j < (sizeof(craftInv[0]) / sizeof(int)); j++) {
                    Textures[0]->useTexture();
                    craftInvShaders[2 * i + j]->useShader();
                    glUniformMatrix4fv(glGetUniformLocation(craftInvShaders[2 * i + j]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
                    glUniformMatrix4fv(craftInvShaders[2 * i + j]->getModelLocation(), 1, GL_FALSE, value_ptr(modelCraftInvSlots[i][j]));
                    craftInvSlot[i][j].renderMesh();
                    inv_change = true;
                }
                craftInvShaders[4]->useShader();
                glUniformMatrix4fv(glGetUniformLocation(craftInvShaders[4]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
                glUniformMatrix4fv(craftInvShaders[4]->getModelLocation(), 1, GL_FALSE, value_ptr(modelCraftedInvSlot));
                craftedItem.blockMesh.renderMesh();
            }
        }

        Textures[2]->useTexture();
        shaders[2]->useShader();
        currInvSlotSelector.renderMesh();
        Textures[0]->useTexture();
        
        for (int i = 0; i < 9; i++) {
            shaders[4 + i]->useShader();
            glUniformMatrix4fv(glGetUniformLocation(shaders[4 + i]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
            glUniformMatrix4fv(shaders[4 + i]->getModelLocation(), 1, GL_FALSE, value_ptr(modelCurSlotsMain[i]));
            currInvSlot[3][i].renderMesh();
            inv_change = true;
        }
   
        shaders[3]->useShader();
        modelCur = mat4(1.0f);
        modelCur = rotate(modelCur, radians(20.0f), vec3(0.5f, 0.5f, 0.0f));

        glUniformMatrix4fv(glGetUniformLocation(shaders[3]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
        glUniformMatrix4fv(shaders[3]->getModelLocation(), 1, GL_FALSE, value_ptr(modelCur));
        glEnable(GL_DEPTH_TEST);
        currentBlock.renderMesh();
        glDisable(GL_DEPTH_TEST);
        
        mainWindow.swapBuffers();

        if (!blockExistsAt(vec3((int)camera.getCameraPos().x, (int)camera.getCameraPos().y - 3, (int)camera.getCameraPos().z))) {
            camera.setCameraPos(vec3(camera.getCameraPos().x, camera.getCameraPos().y - 0.01 * ctrl, camera.getCameraPos().z));
        }
    }
}