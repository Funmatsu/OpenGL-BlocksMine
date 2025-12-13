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

Camera camera = Camera(vec3(0.0f, CHUNK_SIZE / 2, 0.0f), vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f, 0.05f, 1.0f);
int xdimens = 5, ydimens = 5;
int xdimensItem = 2, ydimensItem = 2;

class World {
    public:
    //vector<Chunk> chunks;
    unordered_map<glm::ivec2, Chunk, ivec2_hash, ivec2_eq> chunkData;

    BlockData getBlockAt(ivec3 blockPos);

    void addChunk(Chunk* newChunk, ivec2 xyChunk);

    void updateChunk(const ivec2& chunkCoord, vec3 direction, vec3 blockPosition);

    Mesh createMeshCube(float x, float y, float z, float scale, Item blockType);

    Mesh createVertsOnlyMesh(ivec3 xyz, float scale, Item blockType);

    Block createMeshCube(ivec3 blockPos, float scale, Item blockType);

    void createItem(vec3 blockPos, Item blockType, vec3 direction);

    void deleteBlockFromWorld(vec3 blockPos);

    void delBlocklook_at();

    vec3 addBlocklook_at(Item blockType);
};

World world;


//.cpp part-----------------------------------------------------------------------------------------------------------------------
int floorDiv(float a, float b) {
    return (a >= 0) ? int(a / b) : int((a - b + 1) / b); // the + 1 is for negative multiples : -10/10 = -1, but (-10-10)/10 = -2 x wrong, (-10-10 + 1)/10 = -1.9 = -1 J correct
}

BlockData World::getBlockAt(ivec3 blockPos) {
    if (blockPos.y == -404) return BlockData(vec3(0, -1, 0), AIR);
    ivec2 chunkPos = ivec2(floorDiv(blockPos.x, CHUNK_SIZE), floorDiv(blockPos.z, CHUNK_SIZE));
    return
        //Block(blockPos, world.chunkData[chunkPos].blockData[blockPos].blockType, {}, {}); 
     BlockData(blockPos, world.chunkData[chunkPos].block_data[world.chunkData[chunkPos].at(blockPos)].blockType);
}

bool blockExistsAt(ivec3 blockPos) {
    if (world.chunkData.empty()) return false;

    ivec2 chunkCoord = ivec2(floorDiv(blockPos.x, CHUNK_SIZE), floorDiv(blockPos.z, CHUNK_SIZE));
    auto chunkIt = world.chunkData.find(chunkCoord);
    if (chunkIt == world.chunkData.end()) return false;
    
    //const auto& blockMap = chunkIt->second.blockData;
    //cout << world.chunkData[chunkCoord].coords.x << " " << world.chunkData[chunkCoord].coords.y << " " << "coords" << endl;
    int vec_blockPos = world.chunkData[chunkCoord].at(blockPos);
    //cout << vec_blockPos << " " << endl;
    if (vec_blockPos >= CHUNK_VOLUME || vec_blockPos < 0) {
        return false;
    }
    return 
        (world.chunkData[chunkCoord].block_data[vec_blockPos].blockType != AIR);
    //return (blockMap.find(blockPos) != blockMap.end());
}

vec3 lookingAtBlock() {
    ivec3 blockPos = ivec3(0.0f);
    glm::vec3 rayDir;
    glm::vec3 rayOrigin = camera.getCameraPos();
    rayDir = normalize(camera.getCameraFront());

    float maxDistance = 7.0f;
    float stepSize = 0.2f;
    for (float t = 0.0f; t < maxDistance; t += stepSize) {
        glm::vec3 point = rayOrigin + rayDir * t;
        blockPos = glm::floor(point);
        ivec2 chunkPos = ivec2(floorDiv(blockPos.x, CHUNK_SIZE), floorDiv(blockPos.z, CHUNK_SIZE));
        //int vec_pos = world.chunkData[chunkPos].at(blockPos);
        //cout << vec_pos << endl;
        if (world.chunkData[chunkPos].block_data[world.chunkData[chunkPos].at(blockPos)].blockType != AIR
            //||world.chunkData[chunkPos].blockData[blockPos].blockType != AIR            
            ) 
        {
            return blockPos;
        }
    }
    return ivec3(-404);
}

void World::addChunk(Chunk* newChunk, ivec2 xyChunk) {
    chunkData.insert({ ivec2(xyChunk), move(*newChunk) });
}

void World::deleteBlockFromWorld(vec3 blockPos) {
    ivec2 chunkPos = ivec2(floorDiv(blockPos.x, CHUNK_SIZE), floorDiv(blockPos.z, CHUNK_SIZE));
    if (/*chunkData[chunkPos].blockData[blockPos].blockType.isBreakable*/ chunkData[chunkPos].block_data[chunkData[chunkPos].at(blockPos)].blockType.isBreakable) {
        //inventory.assignAvailableSlot(chunkData[chunkPos].blockData[blockPos].blockType);
        inventory.assignAvailableSlot(chunkData[chunkPos].block_data[chunkData[chunkPos].at(blockPos)].blockType);
        //chunkData[chunkPos].blockData[blockPos].blockType.deassignLight(pointLights, blockPos); 
        //chunkData[chunkPos].blockData[blockPos].blockType = AIR;     
        chunkData[chunkPos].block_data[chunkData[chunkPos].at(blockPos)].blockType.deassignLight(pointLights, blockPos);
        chunkData[chunkPos].block_data[chunkData[chunkPos].at(blockPos)].blockType = AIR;
        
        updateChunk(chunkPos, vec3(0.0f, 1.0f, 0.0f), vec3(-404));
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

        float UVs[7];
        getUVs(blockType, UVs);
        float xoffset = UVs[0],
            yoffset = UVs[1],
            xoffsetTop = UVs[2],
            yoffsetTop = UVs[3],
            xoffsetBottom = UVs[4],
            yoffsetBottom = UVs[5],
            transparency = UVs[6];

        float clipX = 0.03f, clipY = 0.97f;

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
            float UVs[7];
            getUVs(blockType, UVs);
            float xoffset = UVs[0],
                yoffset = UVs[1],
                xoffsetTop = UVs[2],
                yoffsetTop = UVs[3],
                xoffsetBottom = UVs[4],
                yoffsetBottom = UVs[5],
                transparency = UVs[6];

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

    vector<GLfloat> colorMask;
    float tintr = 1.0f, tintg = 1.0f, tintb = 1.0f;
    if (blockType == GRASS) { tintr = 0.2f, tintg = 1.45f, tintb = 0.15f; }
    else if (blockType == OAK_LEAVES) { tintr = 0.2f, tintg = 1.0f, tintb = 0.2f; }

    for (int i = 0; i < triangle.size() / 3; i++) { colorMask.push_back(tintr); colorMask.push_back(tintg); colorMask.push_back(tintb); }

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

        finalVerts.push_back(colorMask[i + 0]);
        finalVerts.push_back(colorMask[i + 1]);
        finalVerts.push_back(colorMask[i + 2]);
    }

    Mesh cubeMesh;
    cubeMesh.createMesh(finalVerts, indices, finalVerts.size(), indices.size());
    return cubeMesh;
}

Mesh World::createVertsOnlyMesh(ivec3 xyz, float scale, Item blockType) {
    if (blockType == AIR) {
        return Mesh();
    }

    std::vector<unsigned int> indices;
    for (int e = 0; e < 6 * 4; e++) {
        int base = e * 4;
        indices.push_back(base + 0);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
        indices.push_back(base + 0);
        indices.push_back(base + 0);
        indices.push_back(base + 3);
        indices.push_back(base + 2);
        indices.push_back(base + 2);
        indices.push_back(base + 1);
        indices.push_back(base + 0);
    }

    float t = 0.0075f;

    vector<GLfloat> verts = {
        xyz.x + -scale + 0.0f    , -t + xyz.y + -scale + 0.0f   , xyz.z + -scale + 0.0f    ,   xyz.x + -scale + 0.0f       ,   xyz.y + t + -scale + 0.0f    , xyz.z + -scale + 0.0f            ,     xyz.x + scale + 1.0f+ t      ,    xyz.y + t + -scale + 0.0f     ,      xyz.z + -scale + 0.0f        ,     xyz.x + scale + 1.0f+ t       ,    xyz.y - t + -scale + 0.0f    ,    xyz.z + -scale + 0.0f        ,
        xyz.x + -scale + 0.0f    , -t + xyz.y + scale + 1.0f    , xyz.z + -scale + 0.0f    ,   xyz.x + -scale + 0.0f       ,   xyz.y + t + scale + 1.0f     , xyz.z + -scale + 0.0f            ,     xyz.x + scale + 1.0f+ t      ,    xyz.y + t + scale + 1.0f      ,     xyz.z + -scale + 0.0f        ,     xyz.x + scale + 1.0f+ t       ,    xyz.y - t + scale + 1.0f     ,   xyz.z + -scale + 0.0f        ,
        xyz.x + scale + 1.0f     ,  xyz.y + -scale + 0.0f       , xyz.z + -scale + 0.0f    ,   xyz.x + scale + 1.0f + t    ,   xyz.y + -scale + 0.0f        , xyz.z + -scale + 0.0f            ,     xyz.x + t + scale + 1.0f     ,    xyz.y + scale + 1.0f          ,     xyz.z + -scale + 0.0f        ,     xyz.x + scale + 1.0f          ,    xyz.y + scale + 1.0f         ,   xyz.z + -scale + 0.0f        ,
        xyz.x + -scale + 0.0f    ,  xyz.y + -scale + 0.0f       , xyz.z + -scale + 0.0f    ,   xyz.x + -scale + 0.0f + t   ,   xyz.y + -scale + 0.0f        , xyz.z + -scale + 0.0f            ,     xyz.x + t + -scale + 0.0f    ,     xyz.y + scale + 1.0f          ,     xyz.z + -scale + 0.0f        ,     xyz.x + -scale + 0.0f         ,     xyz.y + scale + 1.0f         ,   xyz.z + -scale + 0.0f        ,

        xyz.x + -scale + 0.0f    , -t + xyz.y + -scale + 0.0f   , xyz.z + scale + 1.0f     ,   xyz.x + -scale + 0.0f       ,   xyz.y + t + -scale + 0.0f    , xyz.z + scale + 1.0f             ,    xyz.x + scale + 1.0f + t      ,   xyz.y + t + -scale + 0.0f      ,     xyz.z + scale + 1.0f          ,   xyz.x + scale + 1.0f + t       ,   xyz.y - t + -scale + 0.0f     ,   xyz.z + scale + 1.0f          ,
        xyz.x + -scale + 0.0f    , -t + xyz.y + scale + 1.0f    , xyz.z + scale + 1.0f     ,   xyz.x + -scale + 0.0f       ,   xyz.y + t + scale + 1.0f     , xyz.z + scale + 1.0f             ,    xyz.x + scale + 1.0f + t      ,   xyz.y + t + scale + 1.0f       ,    xyz.z + scale + 1.0f          ,   xyz.x + scale + 1.0f + t       ,   xyz.y - t + scale + 1.0f      ,  xyz.z + scale + 1.0f          ,
        xyz.x + -scale + 0.0f    ,  xyz.y + -scale + 0.0f       , xyz.z + scale + 1.0f     ,   xyz.x + -scale + 0.0f + t   ,   xyz.y + -scale + 0.0f        , xyz.z + scale + 1.0f             ,    xyz.x + t + -scale + 0.0f     ,    xyz.y + scale + 1.0f           ,    xyz.z + scale + 1.0f          ,   xyz.x  + -scale + 0.0f         ,    xyz.y + scale + 1.0f          ,  xyz.z + scale + 1.0f          ,
        xyz.x + scale + 1.0f     ,  xyz.y + -scale + 0.0f       , xyz.z + scale + 1.0f     ,   xyz.x + scale + 1.0f + t    ,   xyz.y + -scale + 0.0f        , xyz.z + scale + 1.0f             ,    xyz.x + t + scale + 1.0f      ,   xyz.y + scale + 1.0f           ,    xyz.z + scale + 1.0f          ,   xyz.x  + scale + 1.0f          ,   xyz.y + scale + 1.0f          ,  xyz.z + scale + 1.0f          ,

        xyz.x + -scale + 0.0f    ,  -t + xyz.y + -scale + 0.0f  , xyz.z + -scale + 0.0f    ,   xyz.x + -scale + 0.0f       ,   xyz.y + t + -scale + 0.0f    , xyz.z + -scale + 0.0f            ,     xyz.x + -scale + 0.0f        ,     xyz.y + t + -scale + 0.0f     ,      xyz.z + scale + 1.0f + t     ,    xyz.x + -scale + 0.0f         ,     xyz.y - t + -scale + 0.0f    ,    xyz.z + scale + 1.0f + t     ,
        xyz.x + -scale + 0.0f    ,  -t + xyz.y + scale + 1.0f   , xyz.z + -scale + 0.0f    ,   xyz.x + -scale + 0.0f       ,   xyz.y + t + scale + 1.0f     , xyz.z + -scale + 0.0f            ,     xyz.x + -scale + 0.0f        ,     xyz.y + t + scale + 1.0f      ,     xyz.z + scale + 1.0f + t     ,    xyz.x + -scale + 0.0f         ,     xyz.y - t + scale + 1.0f     ,   xyz.z + scale + 1.0f + t     ,
        xyz.x + -scale + 0.0f    ,  xyz.y + -scale + 0.0f       , xyz.z + scale + 1.0f     ,   xyz.x + -scale + 0.0f       ,   xyz.y + -scale + 0.0f        , xyz.z + scale + 1.0f + t         ,    xyz.x + -scale + 0.0f         ,    xyz.y + scale + 1.0f           ,    xyz.z + t + scale + 1.0f      ,   xyz.x + -scale + 0.0f          ,    xyz.y + scale + 1.0f          ,  xyz.z  + scale + 1.0f         ,
        xyz.x + -scale + 0.0f    ,  xyz.y + -scale + 0.0f       , xyz.z + -scale + 0.0f    ,   xyz.x + -scale + 0.0f       ,   xyz.y + -scale + 0.0f        , xyz.z + -scale + 0.0f + t        ,     xyz.x + -scale + 0.0f        ,     xyz.y + scale + 1.0f          ,     xyz.z + t + -scale + 0.0f    ,     xyz.x + -scale + 0.0f         ,     xyz.y + scale + 1.0f         ,   xyz.z + -scale + 0.0f        ,
       
        xyz.x + scale + 1.0f     ,  -t + xyz.y + -scale + 0.0f  , xyz.z + -scale + 0.0f    ,   xyz.x + scale + 1.0f        ,   xyz.y + t + -scale + 0.0f    , xyz.z + -scale + 0.0f            ,     xyz.x + scale + 1.0f         ,    xyz.y + t + -scale + 0.0f     ,      xyz.z + scale + 1.0f + t     ,    xyz.x + scale + 1.0f          ,    xyz.y - t + -scale + 0.0f    ,    xyz.z + scale + 1.0f + t     ,
        xyz.x + scale + 1.0f     ,  -t + xyz.y + scale + 1.0f   , xyz.z + -scale + 0.0f    ,   xyz.x + scale + 1.0f        ,   xyz.y + t + scale + 1.0f     , xyz.z + -scale + 0.0f            ,     xyz.x + scale + 1.0f         ,    xyz.y + t + scale + 1.0f      ,     xyz.z + scale + 1.0f + t     ,    xyz.x + scale + 1.0f          ,    xyz.y - t + scale + 1.0f     ,   xyz.z + scale + 1.0f + t     ,
        xyz.x + scale + 1.0f     ,  xyz.y + -scale + 0.0f       , xyz.z + -scale + 0.0f    ,   xyz.x + scale + 1.0f        ,   xyz.y + -scale + 0.0f        , xyz.z + -scale + 0.0f + t        ,     xyz.x + scale + 1.0f         ,    xyz.y + scale + 1.0f          ,     xyz.z + t + -scale + 0.0f    ,     xyz.x + scale + 1.0f          ,    xyz.y + scale + 1.0f         ,   xyz.z + -scale + 0.0f        ,
        xyz.x + scale + 1.0f     ,  xyz.y + -scale + 0.0f       , xyz.z + scale + 1.0f     ,   xyz.x + scale + 1.0f        ,   xyz.y + -scale + 0.0f        , xyz.z + scale + 1.0f + t         ,    xyz.x + scale + 1.0f          ,   xyz.y + scale + 1.0f           ,    xyz.z + t + scale + 1.0f      ,   xyz.x + scale + 1.0f           ,   xyz.y + scale + 1.0f          ,  xyz.z + scale + 1.0f          ,
      
        xyz.x + -scale + 0.0f    ,  xyz.y + -scale + 0.0f       , xyz.z + -scale + 0.0f    ,   xyz.x + -scale + 0.0f + t   ,   xyz.y + -scale + 0.0f        , xyz.z + -scale + 0.0f            ,     xyz.x + -scale + 0.0f + t    ,     xyz.y + -scale + 0.0f         ,      xyz.z + scale + 1.0f         ,    xyz.x + -scale + 0.0f         ,     xyz.y + -scale + 0.0f        ,    xyz.z + scale + 1.0f         ,
        xyz.x + scale + 1.0f     ,  xyz.y + -scale + 0.0f       , xyz.z + -scale + 0.0f    ,   xyz.x + scale + 1.0f + t    ,   xyz.y + -scale + 0.0f        , xyz.z + -scale + 0.0f            ,     xyz.x + scale + 1.0f + t     ,    xyz.y + -scale + 0.0f         ,      xyz.z + scale + 1.0f         ,    xyz.x + scale + 1.0f          ,    xyz.y + -scale + 0.0f        ,    xyz.z + scale + 1.0f         ,
        xyz.x + -scale + 0.0f    ,  xyz.y + -scale + 0.0f       , xyz.z + -scale + 0.0f    ,   xyz.x + -scale + 0.0f       ,   xyz.y + -scale + 0.0f        , xyz.z + -scale + 0.0f + t        ,     xyz.x + scale + 1.0f         ,    xyz.y + -scale + 0.0f         ,      xyz.z + -scale + 0.0f + t    ,     xyz.x + scale + 1.0f          ,    xyz.y + -scale + 0.0f        ,    xyz.z + -scale + 0.0f        ,
        xyz.x + -scale + 0.0f    ,  xyz.y + -scale + 0.0f       , xyz.z + scale + 1.0f     ,   xyz.x + -scale + 0.0f       ,   xyz.y + -scale + 0.0f        , xyz.z + scale + 1.0f + t         ,    xyz.x + scale + 1.0f          ,   xyz.y + -scale + 0.0f          ,     xyz.z + scale + 1.0f + t      ,   xyz.x + scale + 1.0f           ,   xyz.y + -scale + 0.0f         ,   xyz.z + scale + 1.0f          ,
       
        xyz.x + -scale + 0.0f    ,  xyz.y + scale + 1.0f        , xyz.z + -scale + 0.0f    ,   xyz.x + -scale + 0.0f + t   ,   xyz.y + scale + 1.0f         , xyz.z + -scale + 0.0f            ,     xyz.x + -scale + 0.0f + t    ,     xyz.y + scale + 1.0f          ,     xyz.z + scale + 1.0f         ,    xyz.x + -scale + 0.0f         ,     xyz.y + scale + 1.0f         ,   xyz.z + scale + 1.0f         ,
        xyz.x + scale + 1.0f     ,  xyz.y + scale + 1.0f        , xyz.z + -scale + 0.0f    ,   xyz.x + scale + 1.0f + t    ,   xyz.y + scale + 1.0f         , xyz.z + -scale + 0.0f            ,     xyz.x + scale + 1.0f + t     ,    xyz.y + scale + 1.0f          ,     xyz.z + scale + 1.0f         ,    xyz.x + scale + 1.0f          ,    xyz.y + scale + 1.0f         ,   xyz.z + scale + 1.0f         ,
        xyz.x + -scale + 0.0f    ,  xyz.y + scale + 1.0f        , xyz.z + -scale + 0.0f    ,   xyz.x + -scale + 0.0f       ,   xyz.y + scale + 1.0f         , xyz.z + -scale + 0.0f + t        ,     xyz.x + scale + 1.0f         ,    xyz.y + scale + 1.0f          ,     xyz.z + -scale + 0.0f + t    ,     xyz.x + scale + 1.0f          ,    xyz.y + scale + 1.0f         ,   xyz.z + -scale + 0.0f        ,
        xyz.x + -scale + 0.0f    ,  xyz.y + scale + 1.0f        , xyz.z + scale + 1.0f     ,   xyz.x + -scale + 0.0f       ,   xyz.y + scale + 1.0f         , xyz.z + scale + 1.0f + t         ,    xyz.x + scale + 1.0f          ,   xyz.y + scale + 1.0f           ,    xyz.z + scale + 1.0f + t      ,   xyz.x + scale + 1.0f           ,   xyz.y + scale + 1.0f          ,  xyz.z + scale + 1.0f          ,
    };
    vector<GLfloat> globalUVs;
    for (int i = 0; i < verts.size(); i++) { globalUVs.push_back(0.0f); }

    vector<GLfloat> colorMask;
    for (int i = 0; i < verts.size(); i++) { colorMask.push_back(1.0f); }

    vector<GLfloat> normals;
    for (int i = 0; i < verts.size(); i++) { normals.push_back(0.0f); }

    vector<GLfloat> finalVerts;
    for (int i = 0; i < verts.size() / 3; i += 1) {
        finalVerts.push_back(verts[3 * i + 0]);
        finalVerts.push_back(verts[3 * i + 1]);
        finalVerts.push_back(verts[3 * i + 2]);

        finalVerts.push_back(globalUVs[3 * i + 0]);
        finalVerts.push_back(globalUVs[3 * i + 1]);
        finalVerts.push_back(globalUVs[3 * i + 2]);

        finalVerts.push_back(normals[3 * i + 0]);
        finalVerts.push_back(normals[3 * i + 1]);
        finalVerts.push_back(normals[3 * i + 2]);

        finalVerts.push_back(colorMask[3 * i + 0]);
        finalVerts.push_back(colorMask[3 * i + 1]);
        finalVerts.push_back(colorMask[3 * i + 2]);
    }

    Mesh cubeMesh;
    cubeMesh.createMesh(finalVerts, indices, finalVerts.size(), indices.size());
    return cubeMesh;
}

Block World::createMeshCube(ivec3 blockPos, float scale, Item blockType) {
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
    vector<GLfloat> colorMask;
    for (int i = 0; i < triangle.size(); i++) { colorMask.push_back(1.0f); }

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

        finalVerts.push_back(colorMask[3 * i + 0]);
        finalVerts.push_back(colorMask[3 * i + 1]);
        finalVerts.push_back(colorMask[3 * i + 2]);
    }

    Block returnBlock = Block(blockPos, blockType, finalVerts, indices);
    returnBlock.blockMesh.createMesh(finalVerts, indices, finalVerts.size(), indices.size());
    return returnBlock;
}

void World::createItem(vec3 blockPos, Item blockType, vec3 direction) {
    ivec2 chunkPos = ivec2(floorDiv(blockPos.x, CHUNK_SIZE), floorDiv(blockPos.z, CHUNK_SIZE));
    //chunkData[chunkPos].blockData[blockPos] = BlockData(blockPos, blockType); 
    chunkData[chunkPos].block_data[chunkData[chunkPos].at(blockPos)] = BlockData(blockPos, blockType);
    ivec3 trueDir = floor(blockPos - direction) + vec3(1.0);
    updateChunk(chunkPos, trueDir, blockPos);
    //std::cout << trueDir.x << " " << trueDir.y << " " << trueDir.z << endl;

    //chunkData[chunkPos].blockData[blockPos].blockType.assignLight(pointLights, blockPos);
    chunkData[chunkPos].block_data[chunkData[chunkPos].at(blockPos)].blockType.assignLight(pointLights, blockPos);
}

bool isAir(Item item);

bool shouldEmitFace(vec2 xyChunk, Chunk* cd, int x, int y, int z, int dx, int dy, int dz);

void emitFace(Mesh& m, int baseX, int baseY, int baseZ,
    int face, Item blockType, float x, float y, float z);

void meshChunk(vec2 xyChunk, Chunk* cd, Mesh& out, vec3 direction, vec3 position);

void generateChunkAt(vec2 xyChunk, Chunk* repChunk);

void regenerateChunk(vec2 xyChunk, Chunk& repChunk);

void World::updateChunk(const ivec2& chunkCoord, vec3 direction, vec3 blockPosition) {
    auto it = chunkData.find(chunkCoord);
    if (it == chunkData.end()) return; // Chunk doesn't exist

    Chunk& chunk = it->second;

    //// Clear old mesh data
    chunk.mesh.verts.clear();
    chunk.mesh.inds.clear();

    // Rebuild mesh from current blockData
    meshChunk(chunkCoord, &chunk, chunk.mesh, direction, blockPosition);

    //// Update vertex/index buffers
    chunk.vertices.clear();
    chunk.indices.clear();

    chunk.vertices.insert(chunk.vertices.end(), chunk.mesh.verts.begin(), chunk.mesh.verts.end());

    uint32_t base = 0;
    for (auto idx : chunk.mesh.inds) {
        chunk.indices.push_back(base + idx);
    }

    chunk.indexOffset = static_cast<uint32_t>(chunk.mesh.verts.size() / 12);
    chunk.needUpdate = true;
}

void World::delBlocklook_at() {
    ivec3 blockPos = lookingAtBlock();
    ivec2 chunkPos = ivec2(floorDiv(blockPos.x, CHUNK_SIZE), floorDiv(blockPos.z, CHUNK_SIZE));
    if (blockPos.y <= -404.0f || !recipe.isBreakable(/*world.chunkData[chunkPos].blockData[blockPos].blockType */ world.chunkData[chunkPos].block_data[world.chunkData[chunkPos].at(blockPos)].blockType)) {
        return;
    }
    deleteBlockFromWorld(blockPos);
}

vec3 World::addBlocklook_at(Item blockType) {
    if (recipe.isTool(blockType)) {
        return vec3(-404.0f);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ivec3 blockPos = vec3(0.0f);
    glm::vec3 rayDir;
    glm::vec3 rayOrigin = camera.getCameraPos();
    rayDir = normalize(camera.getCameraFront());

    float maxDistance = 7.0f;
    float stepSize = 0.2f;
    for (float t = 0.0f; t < maxDistance; t += stepSize) {
        glm::vec3 point = rayOrigin + rayDir * t;
        blockPos = glm::floor(point);
        ivec2 chunkPos = ivec2(floorDiv(blockPos.x, CHUNK_SIZE), floorDiv(blockPos.z, CHUNK_SIZE));
        if (blockExistsAt(blockPos) &&
            world.chunkData[chunkPos].block_data[world.chunkData[chunkPos].at(blockPos)].blockType != AIR &&
            //world.chunkData[chunkPos].blockData[blockPos].blockType != AIR &&
            blockType.isPlaceable) {
            createItem(floor(point - rayDir * stepSize), blockType, point);
            return floor(point - rayDir * stepSize);
        }
    }
}