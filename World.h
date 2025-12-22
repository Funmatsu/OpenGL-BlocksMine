#pragma once

#include "Chunk.h"
#include "normals.h"

struct Projectile {
    vec3 position;
    vec3 front;
    vec3 velocity, initial_velocity = vec3(0);
    Mesh mesh;
    Item item = OAK_PLANK;
    mat4 model;
    bool shot = 0;
    void shoot(vec3 pos, vec3 direction) {
        shot = 1;
        initial_velocity = normalize(direction)*vec3(5, 20, 5);
        velocity = vec3(0);
        front = direction;
        position = pos + front;
    }
    void shoot(vec3 pos, vec3 direction, Item type) {
        item = type;
        shot = 1;
        position = pos + front;
        initial_velocity = normalize(direction) * vec3(5, 20, 5);
        velocity = vec3(0);
        front = normalize(direction);
        
    }
    void update() {
        velocity += vec3(0, -50, 0) * vec3(0.01);
        position += (initial_velocity + velocity) * vec3(0.01);
        
        //cout << "ballp" << position.x << " " << position.y << " " << position.z << endl;
    }
    void draw() {
        mesh.renderMesh();
    }
};

vector<Projectile> dropped;

float randomFloat(float min, float max) {
    static std::default_random_engine generator(std::random_device{}());
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(generator);
}

bool contains(vector<vec2> vec, vec2 value) {
    return std::find(vec.begin(), vec.end(), value) != vec.end();
}

Camera activeCamera = Camera(vec3(0.0f, CHUNK_SIZE / 2, 0.0f), vec3(0.0f,  0.5f, 0.0f), 0.0f, 0.0f, 0.05f,  0.5f);
Camera firstCamera = Camera(vec3(0.0f, CHUNK_SIZE / 2, 0.0f), vec3(0.0f,  0.5f, 0.0f), 0.0f, 0.0f, 0.05f,  0.5f);
Camera thirdCamera_back = Camera(vec3(0.0f, CHUNK_SIZE / 2, 0.0f), vec3(0.0f,  0.5f, 0.0f), 0.0f, 0.0f, 0.05f,  0.5f);

const int MAX_CHUNK_NUM = 30;

struct ChunkData {
    vector<Chunk> chunks;
    int chunkNum = 0;
    ChunkData() {
        chunks.resize(MAX_CHUNK_NUM * MAX_CHUNK_NUM);
    }
    Chunk& operator[](ivec2 chunkPos){
        return chunks[((chunkPos.x + MAX_CHUNK_NUM) * MAX_CHUNK_NUM/2) + (chunkPos.y + MAX_CHUNK_NUM/2)];
    }
    Chunk& operator[](uint64_t chunkPos) {
        return chunks[uint32_t(((chunkPos >> 32) + MAX_CHUNK_NUM/2) * MAX_CHUNK_NUM) + uint32_t((chunkPos & 0xffffffff) + MAX_CHUNK_NUM/2)];
    }
    bool empty() {
        return chunkNum == 0;
    }
    void insert(ivec2 pos, Chunk* ch) {
        Chunk& chunk = (*this)[pos];
        chunk = *ch;
        chunkNum++;
    }
};

class World {
    public:
    //vector<Chunk> chunks;
    //unordered_map<glm::ivec2, Chunk, ivec2_hash, ivec2_eq> chunkData;
    ska::flat_hash_map<uint64_t, Chunk> chunkData;
    //ChunkData chunkData;

    BlockData getBlockAt(ivec3 blockPos);

    void addChunk(Chunk& newChunk, ivec2 xyChunk);

    void updateChunk(const ivec2& chunkCoord, vec3 direction, vec3 blockPosition);

    Mesh createMeshCube(float x, float y, float z, float scale, Item blockType);

    Mesh createVertsOnlyMesh(ivec3 xyz, float scale, Item blockType);

    Block createMeshCube(ivec3 blockPos, float scale, Item blockType);

    Mesh createMeshCube(vec3 blockPos, float scale, Item blockType);

    Mesh createProjectileMesh(vec3 blockPos, float scale, Item blockType);

    void createItem(vec3 blockPos, Item blockType, vec3 direction);

    void deleteBlockFromWorld(vec3 blockPos);

    void delBlocklook_at();

    vec3 addBlocklook_at(Item blockType);
};

World world;


//.cpp part----------------------------------------------------------------------------------------------------------------------

int floorDiv(float a, float b) {
    return (a >= 0) ? int(a / b) : int((a - b + 1) / b); // the + 1 is for negative multiples : -10/10 = -1, but (-10-10)/10 = -2 x wrong, (-10-10 + 1)/10 = -1.9 = -1 J correct
}

BlockData World::getBlockAt(ivec3 blockPos) {
    if (blockPos.y == -404) return BlockData(vec3(0, -1, 0), AIR);
    ivec2 chunkPos = ivec2(floorDiv(blockPos.x, CHUNK_SIZE), floorDiv(blockPos.z, CHUNK_SIZE));
    return
        //Block(blockPos, world.chunkData[chunkPos].blockData[blockPos].blockType, {}, {}); 
     BlockData(blockPos, world.chunkData[pack(chunkPos)].block_data[world.chunkData[pack(chunkPos)].at(blockPos)].blockType);
}

bool blockExistsAt(ivec3 blockPos) {
    if (world.chunkData.empty()) return false;

    ivec2 chunkCoord = ivec2(floorDiv(blockPos.x, CHUNK_SIZE), floorDiv(blockPos.z, CHUNK_SIZE));
    uint64_t chunkIndex = pack(chunkCoord);
    auto chunkIt = world.chunkData.find(chunkIndex);
    if (chunkIt == world.chunkData.end()) return false;
    Chunk& chunk = chunkIt->second;
    //Chunk& chunk = world.chunkData[chunkCoord];
    //if (!chunk.exists) return false;
    int vec_blockPos = chunk.at(blockPos);
    if (vec_blockPos >= CHUNK_VOLUME || vec_blockPos < 0) {
        return false;
    }
    return (chunk.block_data[vec_blockPos].blockType != AIR);
}

vec3 lookingAtBlock() {
    ivec3 blockPos = ivec3(0);
    glm::vec3 rayDir;
    glm::vec3 rayOrigin = firstCamera.getPosition() + 0.5f;
    rayDir = normalize(firstCamera.getFront());

    float maxDistance = 7.0f;
    float stepSize = 0.4f;
    for (float t = 0.0f; t < maxDistance; t += stepSize) {
        glm::vec3 point = rayOrigin + rayDir * t;
        blockPos = glm::floor(point);
        ivec2 chunkPos = ivec2(floorDiv(blockPos.x, CHUNK_SIZE), floorDiv(blockPos.z, CHUNK_SIZE));
        if (rayOrigin.y > 0 && rayOrigin.y < CHUNK_SIZE * CHUNK_SIZE) {
            Chunk& chunk = world.chunkData[pack(chunkPos)];
            int realPos = chunk.at(blockPos);
            if (chunk.block_data[realPos].blockType != AIR)
                return blockPos;
        }
    }
    return ivec3(0);
}

void World::addChunk(Chunk& newChunk, ivec2 xyChunk) {
    //chunkData.insert(xyChunk, newChunk);
    chunkData.emplace(pack(xyChunk), newChunk);
    //chunkData[pack(xyChunk)] = *newChunk;
}

void World::deleteBlockFromWorld(vec3 blockPos) {
    ivec2 chunkPos = ivec2(floorDiv(blockPos.x, CHUNK_SIZE), floorDiv(blockPos.z, CHUNK_SIZE));
    Chunk& chunk = chunkData[pack(chunkPos)];
    Item& blockType = chunk.block_data[chunk.at(blockPos)].blockType;
    if (blockType.isBreakable) {
        dropped.push_back(Projectile());
        dropped.back().shoot(blockPos, vec3(-firstCamera.getFront().x, 0.5, -firstCamera.getFront().z), blockType);
        inventory.assignAvailableSlot(blockType);
        blockType.deassignLight(pointLights, blockPos);
        blockType = AIR;
        
        updateChunk(chunkPos, vec3(0.0f,  0.5f, 0.0f), vec3(0));
    }
}

Mesh World::createVertsOnlyMesh(ivec3 xyz, float scale, Item blockType) {
    if (blockType == AIR) {
        return Mesh();
    }

    std::vector<unsigned int> indices;
    for (int e = 0; e < 6 * 4; e++) {
        int base = e * 4;
        indices.push_back(base + 0); indices.push_back(base + 1); indices.push_back(base + 2);
        indices.push_back(base + 2); indices.push_back(base + 3); indices.push_back(base + 0);
        indices.push_back(base + 0); indices.push_back(base + 3); indices.push_back(base + 2);
        indices.push_back(base + 2); indices.push_back(base + 1); indices.push_back(base + 0);
    }

    float t = 0.0075f; 

    vector<GLfloat> vertices = {
        xyz.x + -scale + -0.5f    , -t + xyz.y + -scale + -0.5f   , xyz.z + -scale + -0.5f    ,   xyz.x + -scale + -0.5f       ,   xyz.y + t + -scale + -0.5f    , xyz.z + -scale + -0.5f            ,     xyz.x + scale +  0.5f+ t      ,    xyz.y + t + -scale + -0.5f     ,      xyz.z + -scale + -0.5f        ,     xyz.x + scale +  0.5f+ t       ,    xyz.y - t + -scale + -0.5f    ,    xyz.z + -scale + -0.5f        ,
        xyz.x + -scale + -0.5f    , -t + xyz.y + scale +  0.5f    , xyz.z + -scale + -0.5f    ,   xyz.x + -scale + -0.5f       ,   xyz.y + t + scale +  0.5f     , xyz.z + -scale + -0.5f            ,     xyz.x + scale +  0.5f+ t      ,    xyz.y + t + scale +  0.5f      ,     xyz.z + -scale + -0.5f        ,     xyz.x + scale +  0.5f+ t       ,    xyz.y - t + scale +  0.5f     ,   xyz.z + -scale + -0.5f        ,
        xyz.x + scale +  0.5f     ,  xyz.y + -scale + -0.5f       , xyz.z + -scale + -0.5f    ,   xyz.x + scale +  0.5f + t    ,   xyz.y + -scale + -0.5f        , xyz.z + -scale + -0.5f            ,     xyz.x + t + scale +  0.5f     ,    xyz.y + scale +  0.5f          ,     xyz.z + -scale + -0.5f        ,     xyz.x + scale +  0.5f          ,    xyz.y + scale +  0.5f         ,   xyz.z + -scale + -0.5f        ,
        xyz.x + -scale + -0.5f    ,  xyz.y + -scale + -0.5f       , xyz.z + -scale + -0.5f    ,   xyz.x + -scale + -0.5f + t   ,   xyz.y + -scale + -0.5f        , xyz.z + -scale + -0.5f            ,     xyz.x + t + -scale + -0.5f    ,     xyz.y + scale +  0.5f          ,     xyz.z + -scale + -0.5f        ,     xyz.x + -scale + -0.5f         ,     xyz.y + scale +  0.5f         ,   xyz.z + -scale + -0.5f        ,

        xyz.x + -scale + -0.5f    , -t + xyz.y + -scale + -0.5f   , xyz.z + scale +  0.5f     ,   xyz.x + -scale + -0.5f       ,   xyz.y + t + -scale + -0.5f    , xyz.z + scale +  0.5f             ,    xyz.x + scale +  0.5f + t      ,   xyz.y + t + -scale + -0.5f      ,     xyz.z + scale +  0.5f          ,   xyz.x + scale +  0.5f + t       ,   xyz.y - t + -scale + -0.5f     ,   xyz.z + scale +  0.5f          ,
        xyz.x + -scale + -0.5f    , -t + xyz.y + scale +  0.5f    , xyz.z + scale +  0.5f     ,   xyz.x + -scale + -0.5f       ,   xyz.y + t + scale +  0.5f     , xyz.z + scale +  0.5f             ,    xyz.x + scale +  0.5f + t      ,   xyz.y + t + scale +  0.5f       ,    xyz.z + scale +  0.5f          ,   xyz.x + scale +  0.5f + t       ,   xyz.y - t + scale +  0.5f      ,  xyz.z + scale +  0.5f          ,
        xyz.x + -scale + -0.5f    ,  xyz.y + -scale + -0.5f       , xyz.z + scale +  0.5f     ,   xyz.x + -scale + -0.5f + t   ,   xyz.y + -scale + -0.5f        , xyz.z + scale +  0.5f             ,    xyz.x + t + -scale + -0.5f     ,    xyz.y + scale +  0.5f           ,    xyz.z + scale +  0.5f          ,   xyz.x  + -scale + -0.5f         ,    xyz.y + scale +  0.5f          ,  xyz.z + scale +  0.5f          ,
        xyz.x + scale +  0.5f     ,  xyz.y + -scale + -0.5f       , xyz.z + scale +  0.5f     ,   xyz.x + scale +  0.5f + t    ,   xyz.y + -scale + -0.5f        , xyz.z + scale +  0.5f             ,    xyz.x + t + scale +  0.5f      ,   xyz.y + scale +  0.5f           ,    xyz.z + scale +  0.5f          ,   xyz.x  + scale +  0.5f          ,   xyz.y + scale +  0.5f          ,  xyz.z + scale +  0.5f          ,

        xyz.x + -scale + -0.5f    ,  -t + xyz.y + -scale + -0.5f  , xyz.z + -scale + -0.5f    ,   xyz.x + -scale + -0.5f       ,   xyz.y + t + -scale + -0.5f    , xyz.z + -scale + -0.5f            ,     xyz.x + -scale + -0.5f        ,     xyz.y + t + -scale + -0.5f     ,      xyz.z + scale +  0.5f + t     ,    xyz.x + -scale + -0.5f         ,     xyz.y - t + -scale + -0.5f    ,    xyz.z + scale +  0.5f + t     ,
        xyz.x + -scale + -0.5f    ,  -t + xyz.y + scale +  0.5f   , xyz.z + -scale + -0.5f    ,   xyz.x + -scale + -0.5f       ,   xyz.y + t + scale +  0.5f     , xyz.z + -scale + -0.5f            ,     xyz.x + -scale + -0.5f        ,     xyz.y + t + scale +  0.5f      ,     xyz.z + scale +  0.5f + t     ,    xyz.x + -scale + -0.5f         ,     xyz.y - t + scale +  0.5f     ,   xyz.z + scale +  0.5f + t     ,
        xyz.x + -scale + -0.5f    ,  xyz.y + -scale + -0.5f       , xyz.z + scale +  0.5f     ,   xyz.x + -scale + -0.5f       ,   xyz.y + -scale + -0.5f        , xyz.z + scale +  0.5f + t         ,    xyz.x + -scale + -0.5f         ,    xyz.y + scale +  0.5f           ,    xyz.z + t + scale +  0.5f      ,   xyz.x + -scale + -0.5f          ,    xyz.y + scale +  0.5f          ,  xyz.z  + scale +  0.5f         ,
        xyz.x + -scale + -0.5f    ,  xyz.y + -scale + -0.5f       , xyz.z + -scale + -0.5f    ,   xyz.x + -scale + -0.5f       ,   xyz.y + -scale + -0.5f        , xyz.z + -scale + -0.5f + t        ,     xyz.x + -scale + -0.5f        ,     xyz.y + scale +  0.5f          ,     xyz.z + t + -scale + -0.5f    ,     xyz.x + -scale + -0.5f         ,     xyz.y + scale +  0.5f         ,   xyz.z + -scale + -0.5f        ,
       
        xyz.x + scale +  0.5f     ,  -t + xyz.y + -scale + -0.5f  , xyz.z + -scale + -0.5f    ,   xyz.x + scale +  0.5f        ,   xyz.y + t + -scale + -0.5f    , xyz.z + -scale + -0.5f            ,     xyz.x + scale +  0.5f         ,    xyz.y + t + -scale + -0.5f     ,      xyz.z + scale +  0.5f + t     ,    xyz.x + scale +  0.5f          ,    xyz.y - t + -scale + -0.5f    ,    xyz.z + scale +  0.5f + t     ,
        xyz.x + scale +  0.5f     ,  -t + xyz.y + scale +  0.5f   , xyz.z + -scale + -0.5f    ,   xyz.x + scale +  0.5f        ,   xyz.y + t + scale +  0.5f     , xyz.z + -scale + -0.5f            ,     xyz.x + scale +  0.5f         ,    xyz.y + t + scale +  0.5f      ,     xyz.z + scale +  0.5f + t     ,    xyz.x + scale +  0.5f          ,    xyz.y - t + scale +  0.5f     ,   xyz.z + scale +  0.5f + t     ,
        xyz.x + scale +  0.5f     ,  xyz.y + -scale + -0.5f       , xyz.z + -scale + -0.5f    ,   xyz.x + scale +  0.5f        ,   xyz.y + -scale + -0.5f        , xyz.z + -scale + -0.5f + t        ,     xyz.x + scale +  0.5f         ,    xyz.y + scale +  0.5f          ,     xyz.z + t + -scale + -0.5f    ,     xyz.x + scale +  0.5f          ,    xyz.y + scale +  0.5f         ,   xyz.z + -scale + -0.5f        ,
        xyz.x + scale +  0.5f     ,  xyz.y + -scale + -0.5f       , xyz.z + scale +  0.5f     ,   xyz.x + scale +  0.5f        ,   xyz.y + -scale + -0.5f        , xyz.z + scale +  0.5f + t         ,    xyz.x + scale +  0.5f          ,   xyz.y + scale +  0.5f           ,    xyz.z + t + scale +  0.5f      ,   xyz.x + scale +  0.5f           ,   xyz.y + scale +  0.5f          ,  xyz.z + scale +  0.5f          ,
      
        xyz.x + -scale + -0.5f    ,  xyz.y + -scale + -0.5f       , xyz.z + -scale + -0.5f    ,   xyz.x + -scale + -0.5f + t   ,   xyz.y + -scale + -0.5f        , xyz.z + -scale + -0.5f            ,     xyz.x + -scale + -0.5f + t    ,     xyz.y + -scale + -0.5f         ,      xyz.z + scale +  0.5f         ,    xyz.x + -scale + -0.5f         ,     xyz.y + -scale + -0.5f        ,    xyz.z + scale +  0.5f         ,
        xyz.x + scale +  0.5f     ,  xyz.y + -scale + -0.5f       , xyz.z + -scale + -0.5f    ,   xyz.x + scale +  0.5f + t    ,   xyz.y + -scale + -0.5f        , xyz.z + -scale + -0.5f            ,     xyz.x + scale +  0.5f + t     ,    xyz.y + -scale + -0.5f         ,      xyz.z + scale +  0.5f         ,    xyz.x + scale +  0.5f          ,    xyz.y + -scale + -0.5f        ,    xyz.z + scale +  0.5f         ,
        xyz.x + -scale + -0.5f    ,  xyz.y + -scale + -0.5f       , xyz.z + -scale + -0.5f    ,   xyz.x + -scale + -0.5f       ,   xyz.y + -scale + -0.5f        , xyz.z + -scale + -0.5f + t        ,     xyz.x + scale +  0.5f         ,    xyz.y + -scale + -0.5f         ,      xyz.z + -scale + -0.5f + t    ,     xyz.x + scale +  0.5f          ,    xyz.y + -scale + -0.5f        ,    xyz.z + -scale + -0.5f        ,
        xyz.x + -scale + -0.5f    ,  xyz.y + -scale + -0.5f       , xyz.z + scale +  0.5f     ,   xyz.x + -scale + -0.5f       ,   xyz.y + -scale + -0.5f        , xyz.z + scale +  0.5f + t         ,    xyz.x + scale +  0.5f          ,   xyz.y + -scale + -0.5f          ,     xyz.z + scale +  0.5f + t      ,   xyz.x + scale +  0.5f           ,   xyz.y + -scale + -0.5f         ,   xyz.z + scale +  0.5f          ,
       
        xyz.x + -scale + -0.5f    ,  xyz.y + scale +  0.5f        , xyz.z + -scale + -0.5f    ,   xyz.x + -scale + -0.5f + t   ,   xyz.y + scale +  0.5f         , xyz.z + -scale + -0.5f            ,     xyz.x + -scale + -0.5f + t    ,     xyz.y + scale +  0.5f          ,     xyz.z + scale +  0.5f         ,    xyz.x + -scale + -0.5f         ,     xyz.y + scale +  0.5f         ,   xyz.z + scale +  0.5f         ,
        xyz.x + scale +  0.5f     ,  xyz.y + scale +  0.5f        , xyz.z + -scale + -0.5f    ,   xyz.x + scale +  0.5f + t    ,   xyz.y + scale +  0.5f         , xyz.z + -scale + -0.5f            ,     xyz.x + scale +  0.5f + t     ,    xyz.y + scale +  0.5f          ,     xyz.z + scale +  0.5f         ,    xyz.x + scale +  0.5f          ,    xyz.y + scale +  0.5f         ,   xyz.z + scale +  0.5f         ,
        xyz.x + -scale + -0.5f    ,  xyz.y + scale +  0.5f        , xyz.z + -scale + -0.5f    ,   xyz.x + -scale + -0.5f       ,   xyz.y + scale +  0.5f         , xyz.z + -scale + -0.5f + t        ,     xyz.x + scale +  0.5f         ,    xyz.y + scale +  0.5f          ,     xyz.z + -scale + -0.5f + t    ,     xyz.x + scale +  0.5f          ,    xyz.y + scale +  0.5f         ,   xyz.z + -scale + -0.5f        ,
        xyz.x + -scale + -0.5f    ,  xyz.y + scale +  0.5f        , xyz.z + scale +  0.5f     ,   xyz.x + -scale + -0.5f       ,   xyz.y + scale +  0.5f         , xyz.z + scale +  0.5f + t         ,    xyz.x + scale +  0.5f          ,   xyz.y + scale +  0.5f           ,    xyz.z + scale +  0.5f + t      ,   xyz.x + scale +  0.5f           ,   xyz.y + scale +  0.5f          ,  xyz.z + scale +  0.5f          ,
    };
    vector<GLfloat> globalUVs;
    for (int i = 0; i < vertices.size(); i++) { globalUVs.push_back(0.0f); }

    vector<GLfloat> colorMask;
    for (int i = 0; i < vertices.size(); i++) { colorMask.push_back( 0.5f); }

    vector<GLfloat> normals;
    for (int i = 0; i < vertices.size(); i++) { normals.push_back(0.0f); }

    vector<GLfloat> finalvertices;
    for (int i = 0; i < vertices.size() / 3; i += 1) {
        finalvertices.push_back(vertices[3 * i + 0]);
        finalvertices.push_back(vertices[3 * i + 1]);
        finalvertices.push_back(vertices[3 * i + 2]);

        finalvertices.push_back(globalUVs[3 * i + 0]);
        finalvertices.push_back(globalUVs[3 * i + 1]);
        finalvertices.push_back(globalUVs[3 * i + 2]);

        finalvertices.push_back(normals[3 * i + 0]);
        finalvertices.push_back(normals[3 * i + 1]);
        finalvertices.push_back(normals[3 * i + 2]);

        finalvertices.push_back(colorMask[3 * i + 0]);
        finalvertices.push_back(colorMask[3 * i + 1]);
        finalvertices.push_back(colorMask[3 * i + 2]);
    }

    Mesh cubeMesh;
    cubeMesh.createMesh(finalvertices, indices, finalvertices.size(), indices.size());
    return cubeMesh;
}

Mesh World::createMeshCube(vec3 blockPos, float scale, Item blockType) {
    float UVs[7] = { 1, 0, 0, 0, 0, 0, 1 };
    if (!blockType.isFlat)
        getUVs(blockType, UVs);
    float xoffset = UVs[0],
        yoffset = UVs[1],
        xoffsetTop = UVs[2],
        yoffsetTop = UVs[3],
        xoffsetBottom = UVs[4],
        yoffsetBottom = UVs[5],
        transparency =  1.0f;

    float clipX = 0.0f, clipY = 1.0f;

    vector<unsigned int> indices;
    for (int e = 0; e < 6; e++) {
        int base = e * 4;
        //indices.push_back(base + 0); indices.push_back(base + 1); indices.push_back(base + 2);
        //indices.push_back(base + 2); indices.push_back(base + 3); indices.push_back(base + 0);
        indices.push_back(base + 0); indices.push_back(base + 3); indices.push_back(base + 2);
        indices.push_back(base + 2); indices.push_back(base + 1); indices.push_back(base + 0);
    }

    vector<GLfloat> globalUVs;

    for (int i = 0; i < 6; i++) {
        int offsetX = 0, offsetY = 0;
        if (i == 2) { offsetX = 1; offsetY = 1; }
        else if (i == 3) { offsetX = 1;  offsetY = 1; }
        globalUVs.push_back(clipX); globalUVs.push_back(clipX); globalUVs.push_back(transparency);
        globalUVs.push_back(clipX); globalUVs.push_back(clipY); globalUVs.push_back(transparency);
        globalUVs.push_back(clipY); globalUVs.push_back(clipY); globalUVs.push_back(transparency);
        globalUVs.push_back(clipY); globalUVs.push_back(clipX); globalUVs.push_back(transparency);
    }

    vector<GLfloat> triangle = {
            -0.50f + blockPos.x + 0.1f * -scale,   -0.50f + blockPos.y + 0.1f * -scale,    -0.50f + blockPos.z + 0.1f * -scale,
            -0.50f + blockPos.x + 0.1f * -scale,   -0.50f + blockPos.y + 0.1f * -scale,     0.50f + blockPos.z + 0.1f * scale,
            -0.50f + blockPos.x + 0.1f * -scale,   0.50f + blockPos.y + 0.1f * scale,      0.50f + blockPos.z + 0.1f * scale,
            -0.50f + blockPos.x + 0.1f * -scale,   0.50f + blockPos.y + 0.1f * scale,     -0.50f + blockPos.z + 0.1f * -scale,
                                                                                           
            0.50f + blockPos.x + 0.1f * scale,     -0.50f + blockPos.y + 0.1f * -scale,    -0.50f + blockPos.z + 0.1f * -scale,
            0.50f + blockPos.x + 0.1f * scale,     0.50f + blockPos.y + 0.1f * scale,      -0.50f + blockPos.z + 0.1f * -scale,
            0.50f + blockPos.x + 0.1f * scale,     0.50f + blockPos.y + 0.1f * scale,      0.50f + blockPos.z + 0.1f * scale,
            0.50f + blockPos.x + 0.1f * scale,     -0.50f + blockPos.y + 0.1f * -scale,    0.50f + blockPos.z + 0.1f * scale,
                                                                                           
            -0.50f + blockPos.x + 0.1f * -scale,   -0.50f + blockPos.y + 0.1f * -scale,    -0.50f + blockPos.z + 0.1f * -scale,
            -0.50f + blockPos.x + 0.1f * -scale,    0.50f + blockPos.y + 0.1f * scale,     -0.50f + blockPos.z + 0.1f * -scale,
            0.50f + blockPos.x + 0.1f * scale,     0.50f + blockPos.y + 0.1f * scale,      -0.50f + blockPos.z + 0.1f * -scale, 
            0.50f + blockPos.x + 0.1f * scale,    -0.50f + blockPos.y + 0.1f * -scale,     -0.50f + blockPos.z + 0.1f * -scale,
                                                                                           
            -0.50f + blockPos.x + 0.1f * -scale,   -0.50f + blockPos.y + 0.1f * -scale,    0.50f + blockPos.z + 0.1f * scale,
            0.50f + blockPos.x + 0.1f * scale,     -0.50f + blockPos.y + 0.1f * -scale,    0.50f + blockPos.z + 0.1f * scale,
            0.50f + blockPos.x + 0.1f * scale,     0.50f + blockPos.y + 0.1f * scale,      0.50f + blockPos.z + 0.1f * scale,
            -0.50f + blockPos.x + 0.1f * -scale,   0.50f + blockPos.y + 0.1f * scale,      0.50f + blockPos.z + 0.1f * scale,
                                                                                           
            -0.50f + blockPos.x + 0.1f * -scale,   -0.50f + blockPos.y + 0.1f * -scale,    -0.50f + blockPos.z + 0.1f * -scale,
            0.50f + blockPos.x + 0.1f * scale,     -0.50f + blockPos.y + 0.1f * -scale,    -0.50f + blockPos.z + 0.1f * -scale,
            0.50f + blockPos.x + 0.1f * scale,     -0.50f + blockPos.y + 0.1f * -scale,    0.50f + blockPos.z + 0.1f * scale,
            -0.50f + blockPos.x + 0.1f * -scale,   -0.50f + blockPos.y + 0.1f * -scale,    0.50f + blockPos.z + 0.1f * scale,
                                                                                           
            -0.50f + blockPos.x + 0.1f * -scale,   0.50f + blockPos.y + 0.1f * scale,      -0.50f + blockPos.z + 0.1f * -scale,
            -0.50f + blockPos.x + 0.1f * -scale,    0.50f + blockPos.y + 0.1f * scale,      0.50f + blockPos.z + 0.1f * scale,
            0.50f + blockPos.x + 0.1f * scale,     0.50f + blockPos.y + 0.1f * scale,      0.50f + blockPos.z + 0.1f * scale,
            0.50f + blockPos.x + 0.1f * scale,    0.50f + blockPos.y + 0.1f * scale,      -0.50f + blockPos.z + 0.1f * -scale
    };

    vector<GLfloat> normals = long_normals;
    vector<GLfloat> colorMask;
    for (int i = 0; i < triangle.size(); i++) { colorMask.push_back(1.0f); }

    vector<GLfloat> finalvertices;
    for (int i = 0; i < triangle.size(); i += 3) {
        finalvertices.push_back(triangle[i + 0]);
        finalvertices.push_back(triangle[i + 1]);
        finalvertices.push_back(triangle[i + 2]);

        finalvertices.push_back(globalUVs[i + 0]);
        finalvertices.push_back(globalUVs[i + 1]);
        finalvertices.push_back(globalUVs[i + 2]);

        finalvertices.push_back(normals[i + 0]);
        finalvertices.push_back(normals[i + 1]);
        finalvertices.push_back(normals[i + 2]);

        finalvertices.push_back(colorMask[i + 0]);
        finalvertices.push_back(colorMask[i + 1]);
        finalvertices.push_back(colorMask[i + 2]);
    }

    Mesh returnMesh;
    returnMesh.createMesh(finalvertices, indices, finalvertices.size(), indices.size());
    return returnMesh;
}

Mesh World::createProjectileMesh(vec3 blockPos, float scale, Item blockType) {
    float UVs[7] = { 1, 0, 0, 0, 0, 0, 1 };
    if (!blockType.isFlat)
        getUVs(blockType, UVs);
    float xoffset = UVs[0],
        yoffset = UVs[1],
        xoffsetTop = UVs[2],
        yoffsetTop = UVs[3],
        xoffsetBottom = UVs[4],
        yoffsetBottom = UVs[5],
        transparency = 1.0f;

    float clipX = 0.0f, clipY = 1.0f;

    vector<unsigned int> indices;
    for (int e = 0; e < 6; e++) {
        int base = e * 4;
        //indices.push_back(base + 0); indices.push_back(base + 1); indices.push_back(base + 2);
        //indices.push_back(base + 2); indices.push_back(base + 3); indices.push_back(base + 0);
        indices.push_back(base + 0); indices.push_back(base + 3); indices.push_back(base + 2);
        indices.push_back(base + 2); indices.push_back(base + 1); indices.push_back(base + 0);
    }

    vector<GLfloat> globalUVs;

    for (int i = 0; i < 6; i++) {
        int offsetX = 0, offsetY = 0;
        if (i == 4) { offsetX = xoffsetBottom; offsetY = yoffsetBottom; }
        else if (i == 5) { offsetX = xoffsetTop;  offsetY = yoffsetTop; }
        if (i % 3 == 0) {
            globalUVs.push_back((clipX + xoffset + offsetX) / xdimens); globalUVs.push_back((clipX + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
            globalUVs.push_back((clipY + xoffset + offsetX) / xdimens); globalUVs.push_back((clipX + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
            globalUVs.push_back((clipY + xoffset + offsetX) / xdimens); globalUVs.push_back((clipY + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
            globalUVs.push_back((clipX + xoffset + offsetX) / xdimens); globalUVs.push_back((clipY + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
        }
        else {
            globalUVs.push_back((clipX + xoffset + offsetX) / xdimens); globalUVs.push_back((clipX + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
            globalUVs.push_back((clipX + xoffset + offsetX) / xdimens); globalUVs.push_back((clipY + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
            globalUVs.push_back((clipY + xoffset + offsetX) / xdimens); globalUVs.push_back((clipY + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
            globalUVs.push_back((clipY + xoffset + offsetX) / xdimens); globalUVs.push_back((clipX + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
        }
    }

    vector<GLfloat> triangle = {
            -0.50f + blockPos.x + 0.1f * -scale,   -0.50f + blockPos.y + 0.1f * -scale,    -0.50f + blockPos.z + 0.1f * -scale,
            -0.50f + blockPos.x + 0.1f * -scale,   -0.50f + blockPos.y + 0.1f * -scale,     0.50f + blockPos.z + 0.1f * scale,
            -0.50f + blockPos.x + 0.1f * -scale,   0.50f + blockPos.y + 0.1f * scale,      0.50f + blockPos.z + 0.1f * scale,
            -0.50f + blockPos.x + 0.1f * -scale,   0.50f + blockPos.y + 0.1f * scale,     -0.50f + blockPos.z + 0.1f * -scale,

            0.50f + blockPos.x + 0.1f * scale,     -0.50f + blockPos.y + 0.1f * -scale,    -0.50f + blockPos.z + 0.1f * -scale,
            0.50f + blockPos.x + 0.1f * scale,     0.50f + blockPos.y + 0.1f * scale,      -0.50f + blockPos.z + 0.1f * -scale,
            0.50f + blockPos.x + 0.1f * scale,     0.50f + blockPos.y + 0.1f * scale,      0.50f + blockPos.z + 0.1f * scale,
            0.50f + blockPos.x + 0.1f * scale,     -0.50f + blockPos.y + 0.1f * -scale,    0.50f + blockPos.z + 0.1f * scale,

            -0.50f + blockPos.x + 0.1f * -scale,   -0.50f + blockPos.y + 0.1f * -scale,    -0.50f + blockPos.z + 0.1f * -scale,
            -0.50f + blockPos.x + 0.1f * -scale,    0.50f + blockPos.y + 0.1f * scale,     -0.50f + blockPos.z + 0.1f * -scale,
            0.50f + blockPos.x + 0.1f * scale,     0.50f + blockPos.y + 0.1f * scale,      -0.50f + blockPos.z + 0.1f * -scale,
            0.50f + blockPos.x + 0.1f * scale,    -0.50f + blockPos.y + 0.1f * -scale,     -0.50f + blockPos.z + 0.1f * -scale,

            -0.50f + blockPos.x + 0.1f * -scale,   -0.50f + blockPos.y + 0.1f * -scale,    0.50f + blockPos.z + 0.1f * scale,
            0.50f + blockPos.x + 0.1f * scale,    -0.50f + blockPos.y + 0.1f * -scale,    0.50f + blockPos.z + 0.1f * scale,
            0.50f + blockPos.x + 0.1f * scale,     0.50f + blockPos.y + 0.1f * scale,      0.50f + blockPos.z + 0.1f * scale,
            -0.50f + blockPos.x + 0.1f * -scale,     0.50f + blockPos.y + 0.1f * scale,      0.50f + blockPos.z + 0.1f * scale,

            -0.50f + blockPos.x + 0.1f * -scale,   -0.50f + blockPos.y + 0.1f * -scale,    -0.50f + blockPos.z + 0.1f * -scale,
            0.50f + blockPos.x + 0.1f * scale,     -0.50f + blockPos.y + 0.1f * -scale,    -0.50f + blockPos.z + 0.1f * -scale,
            0.50f + blockPos.x + 0.1f * scale,     -0.50f + blockPos.y + 0.1f * -scale,    0.50f + blockPos.z + 0.1f * scale,
            -0.50f + blockPos.x + 0.1f * -scale,   -0.50f + blockPos.y + 0.1f * -scale,    0.50f + blockPos.z + 0.1f * scale,

            -0.50f + blockPos.x + 0.1f * -scale,   0.50f + blockPos.y + 0.1f * scale,      -0.50f + blockPos.z + 0.1f * -scale,
            -0.50f + blockPos.x + 0.1f * -scale,    0.50f + blockPos.y + 0.1f * scale,      0.50f + blockPos.z + 0.1f * scale,
            0.50f + blockPos.x + 0.1f * scale,     0.50f + blockPos.y + 0.1f * scale,      0.50f + blockPos.z + 0.1f * scale,
            0.50f + blockPos.x + 0.1f * scale,    0.50f + blockPos.y + 0.1f * scale,      -0.50f + blockPos.z + 0.1f * -scale
    };

    vector<GLfloat> normals = long_normals;
    vector<GLfloat> colorMask;
    for (int i = 0; i < triangle.size(); i++) { colorMask.push_back( 1.0f); }

    vector<GLfloat> finalvertices;
    for (int i = 0; i < triangle.size(); i += 3) {
        finalvertices.push_back(triangle[i + 0]);
        finalvertices.push_back(triangle[i + 1]);
        finalvertices.push_back(triangle[i + 2]);

        finalvertices.push_back(globalUVs[i + 0]);
        finalvertices.push_back(globalUVs[i + 1]);
        finalvertices.push_back(globalUVs[i + 2]);

        finalvertices.push_back(normals[i + 0]);
        finalvertices.push_back(normals[i + 1]);
        finalvertices.push_back(normals[i + 2]);

        finalvertices.push_back(colorMask[i + 0]);
        finalvertices.push_back(colorMask[i + 1]);
        finalvertices.push_back(colorMask[i + 2]);
    }

    Mesh returnMesh;
    returnMesh.createMesh(finalvertices, indices, finalvertices.size(), indices.size());
    return returnMesh;
}

void World::createItem(vec3 blockPos, Item blockType, vec3 direction) {
    ivec2 chunkPos = ivec2(floorDiv(blockPos.x, CHUNK_SIZE), floorDiv(blockPos.z, CHUNK_SIZE));
    //chunkData[chunkPos].blockData[blockPos] = BlockData(blockPos, blockType); 
    chunkData[pack(chunkPos)].block_data[chunkData[pack(chunkPos)].at(blockPos)] = BlockData(blockPos, blockType);
    ivec3 trueDir = floor(blockPos - direction) + vec3(1.0);
    updateChunk(chunkPos, trueDir, blockPos);
    //std::cout << trueDir.x << " " << trueDir.y << " " << trueDir.z << endl;

    //chunkData[pack(chunkPos)].blockData[blockPos].blockType.assignLight(pointLights, blockPos);
    chunkData[pack(chunkPos)].block_data[chunkData[pack(chunkPos)].at(blockPos)].blockType.assignLight(pointLights, blockPos);
}

bool isAir(Item item);

bool shouldEmitFace(vec2 xyChunk, Chunk* cd, int x, int y, int z, int dx, int dy, int dz);

void emitFace(Mesh& m, int baseX, int baseY, int baseZ,
    int face, Item blockType, float x, float y, float z);

void meshChunk(vec2 xyChunk, Chunk* cd, Mesh& out, vec3 direction, vec3 position);

void generateChunkAt(vec2 xyChunk, Chunk* repChunk);

void World::updateChunk(const ivec2& chunkCoord, vec3 direction, vec3 blockPosition) {
    //auto start = std::chrono::high_resolution_clock::now();

    auto it = chunkData.find(pack(chunkCoord));
    if (it == chunkData.end()) return; // Chunk doesn't exist

    Chunk& chunk = it->second;

    //// Clear old mesh data
    chunk.mesh.vertices.clear();
    chunk.mesh.indices.clear();

    // Rebuild mesh from current blockData
    meshChunk(chunkCoord, &chunk, chunk.mesh, direction, blockPosition);
    chunk.needUpdate = true;

    //auto end = std::chrono::high_resolution_clock::now();
    //std::cout << chunk.mesh.vertices.size() << " updates : Elapsed: " << std::chrono::duration<double>(end - start).count() << " s\n";
}

void World::delBlocklook_at() {
    ivec3 blockPos = lookingAtBlock();
    ivec2 chunkPos = ivec2(floorDiv(blockPos.x, CHUNK_SIZE), floorDiv(blockPos.z, CHUNK_SIZE));
    if (blockPos.y <= -404.0f || !recipe.isBreakable(/*world.chunkData[chunkPos].blockData[blockPos].blockType */ world.chunkData[pack(chunkPos)].block_data[world.chunkData[pack(chunkPos)].at(blockPos)].blockType)) {
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
    glm::vec3 rayOrigin = firstCamera.getPosition() + 0.5f;
    rayDir = normalize(firstCamera.getFront());

    float maxDistance = 7.0f;
    float stepSize = 0.2f;
    for (float t = 0.0f; t < maxDistance; t += stepSize) {
        glm::vec3 point = rayOrigin + rayDir * t;
        blockPos = glm::floor(point);
        ivec2 chunkPos = ivec2(floorDiv(blockPos.x, CHUNK_SIZE), floorDiv(blockPos.z, CHUNK_SIZE));
        Chunk& chunk = world.chunkData[pack(chunkPos)];
        int index = chunk.at(blockPos);
        if (blockExistsAt(blockPos) &&
            chunk.block_data[index].blockType != AIR &&
            blockType.isPlaceable) {
            createItem(floor(point - rayDir * stepSize), blockType, point);
            return floor(point - rayDir * stepSize);
        }
    }
}