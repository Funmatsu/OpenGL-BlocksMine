#pragma once
#include "Chunk.h"
#include "normals.h"

//vector<vec2> chunkCoords;

uint32_t to(ivec2 vec) {
    uint16_t x = (uint16_t)(int16_t)vec.x, y = (uint16_t)(int16_t)vec.y;
    return (x << 16) ^ y;
}

std::queue<vec2> chunkRequestQueue;
std::mutex queueMutex;
std::condition_variable queueCV;

std::queue<unique_ptr<Chunk>> chunkResultQueue;
std::mutex resultMutex;

//std::queue<vec2> chunkRequestQueue;
std::mutex chunkRequestMutex;

//std::queue<Chunk> chunkResultQueue;
std::mutex chunkResultMutex;

std::queue<vec2> chunkRequestQueue2;
std::mutex chunkRequestMutex2;

std::queue<Chunk> chunkResultQueue2;
std::mutex chunkResultMutex2;

std::queue<Chunk> chunkUpdateRequestQueue;
std::mutex chunkUpdateRequestMutex;

std::queue<bool> chunkUpdateResultQueue;
std::mutex chunkUpdateResultMutex;

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
std::atomic<bool> chunkUpdateGenRunning = true;
std::atomic<bool> blockBreaking = true;
std::atomic<bool> blockPlacing = true;
bool blockBreakingOut = false;
bool blockPlacingOut = false;

// Base hills: smooth FBm Perlin
FastNoiseLite baseNoise;
// Ridged mountains: sharp features
FastNoiseLite ridgedNoise;
// Mask: controls where mountains vs plains appear
FastNoiseLite maskNoise;
FastNoiseLite caveNoise;
FastNoiseLite cloudNoise;

//cloudNoise.SetFrequency(0.045);  
void initChunksNoise() {
    baseNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    baseNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    baseNoise.SetFrequency(0.005f);      // low frequency = broad features
    baseNoise.SetFractalOctaves(3);
    baseNoise.SetFractalLacunarity(2.0f);
    baseNoise.SetFractalGain(0.5f);

    ridgedNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    ridgedNoise.SetFractalType(FastNoiseLite::FractalType_Ridged);
    ridgedNoise.SetFrequency(0.005f);     // higher frequency = more detail
    ridgedNoise.SetFractalOctaves(4);
    ridgedNoise.SetFractalLacunarity(2.0f);
    ridgedNoise.SetFractalGain(0.5f);

    //maskNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    maskNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    maskNoise.SetFrequency(0.02f);      // very low frequency = large biome regions
    maskNoise.SetFractalOctaves(2);
    maskNoise.SetFractalGain(0.5f);

    //FastNoiseLite noise;   
    //noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    //noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    //noise.SetFractalOctaves(6);
    //noise.SetFractalLacunarity(2.0f);
    //noise.SetFractalGain(0.5f);

    caveNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

    caveNoise.SetFrequency(0.05f);
    cloudNoise.SetFrequency(0.045);  

}

bool isAir(Item item);

void generateBlocks(vec2 xyChunk, Chunk* repChunk) {
    //auto start = std::chrono::high_resolution_clock::now();
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

    float cloudDensity = 0;
    float density = 0;
    int maxHeight = 0;
    Chunk& ch = *repChunk;

    for (int x = (xyChunk.x) * CHUNK_SIZE; x < (xyChunk.x + 1) * CHUNK_SIZE; x++) {
        for (int z = (xyChunk.y) * CHUNK_SIZE; z < (xyChunk.y + 1) * CHUNK_SIZE; z++) {
                    (*repChunk)[ivec3(x, 0, z)] = BlockData(BEDROCK);
            auto norm    = [](float n) { return (n + 1) * 0.5f; }; //defining a function inline
            float base   = norm(baseNoise.GetNoise((float)x, (float)z)) * 10.0f;
            float ridged = norm(ridgedNoise.GetNoise((float)x, (float)z)) * 10.0f;
            float mask   = norm(maskNoise.GetNoise((float)x, (float)z));

            // Blend between base and ridged using the mask
            float blend = smoothstep(0.0f, 1.0f, mask);
            float height = mix(base, ridged, blend);
            float treeHeight = randomFloat(3.0, 5.0) + 2, treeDistrib = rand();
            float randomNumberForDeco = rand();
            
            //float scaledHeight = height;
            float scaledHeight = height * 4 + CHUNK_SIZE * 5;
            maxHeight = (scaledHeight > maxHeight) ? scaledHeight + treeHeight + 5 : maxHeight;
            //float scaledHeight = ((height + 1.0f) * (CHUNK_SIZE * CHUNK_SIZE));
            //cloudDensity = cloudNoise.GetNoise((float)x, (float)z);
            //if (cloudDensity < -0.25f) {
            //    ch[ivec3(x, CHUNK_SIZE * CHUNK_SIZE + 2 * CHUNK_SIZE - 1, z)] = BlockData(ivec3(x, CHUNK_SIZE * CHUNK_SIZE + 2 * CHUNK_SIZE - 1, z), CLOUD);
            //}
            
            for (int y = 1; y < scaledHeight; y++) {
                if (y >= scaledHeight - 1) {
                    blockType = GRASS_BLOCK;
                }
                else if (y > scaledHeight - 4 && y < scaledHeight - 1) {
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
                else continue;
                density = caveNoise.GetNoise((float)x, (float)y, (float)z);

                if (density < -0.5f) {
                    continue;
                }
                ch[ivec3(x, y, z)] = BlockData(blockType);

                int radius = 3;  //adjust for size

                //Making the trees and leaves
                if (x >= (xyChunk.x + 1) * CHUNK_SIZE - radius || x <= (xyChunk.x) * CHUNK_SIZE + radius ||
                    z >= (xyChunk.y + 1) * CHUNK_SIZE - radius || z <= (xyChunk.y) * CHUNK_SIZE + radius) {
                    goto notree;
                }
                if (blockType == GRASS_BLOCK) {
                    if (treeDistrib > 0.0 && treeDistrib <= 75) {

                        for (int i = y + 1; i < y + treeHeight; i++) {
                            ch[ivec3(x, i, z)] = BlockData(OAK_WOOD);
                        }
                        glm::ivec3 center(x, y + treeHeight + 1, z);

                        for (int dx = -radius; dx <= radius; ++dx) {
                            for (int dy = -radius; dy <= radius; ++dy) {
                                for (int dz = -radius; dz <= radius; ++dz) {
                                    glm::ivec3 pos = center + ivec3(dx, dy, dz);
                                    float dist = glm::length(vec3(dx, dy, dz));

                                    if (dist <= radius + randomFloat(-0.3f, 0.3f)) {
                                        if (isAir(ch[pos].blockType)) {
                                            ch[pos] = BlockData(OAK_LEAVES);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                notree:;  //No trees should be created if they are near the chunks border

                if (blockType == GRASS_BLOCK && blockType != OAK_WOOD) {
                    ivec3 pos(x, y + 1, z); BlockData& blockData = ch[pos];
                    if (isAir(blockData.blockType)) {
                        if (randomNumberForDeco < 2000) {
                            for (int i = 1; i <= randomFloat(1, 3); i++) {
                                ivec3 pos(x, y + i, z); BlockData& blockData = ch[pos];
                                blockData = BlockData(GRASS);
                            }
                        }
                        else {
                            if (randomNumberForDeco > 2000 && randomNumberForDeco < 2500) {
                                blockData = BlockData(POPPY);
                            }
                            else if (randomNumberForDeco > 2500 && randomNumberForDeco < 2800) {
                                blockData = BlockData(BLUE_ORCHID);
                            }
                        }
                    }

                }
            }
        }
    }
    repChunk->terrainHeight = maxHeight;
}

bool isAir(Item item) { return item == AIR; }

bool shouldEmitFace(vec2 xyChunk, Chunk* cd, Item currentBlockType, int x, int y, int z, int dx, int dy, int dz) {
    if (currentBlockType.isFlat() || currentBlockType == OAK_PLANK) return 1;
    int nx = x + dx, ny = y + dy, nz = z + dz;
    ivec3 checkPos = ivec3(nx, ny, nz);
    if (!cd->inBounds(checkPos) && y != 0) {
        uint32_t pos = pack(ivec2(floorDiv(checkPos.x, CHUNK_SIZE), floorDiv(checkPos.z, CHUNK_SIZE)));
        if (!world.chunkData.count(pos)) return 0;
        else {
            BlockData& bData = (*world.chunkData[pos])[checkPos];
            return (isAir(bData.blockType)) || bData.blockType.isFlat();
        }
    }
    else if (y == 0) { return 0; }
    BlockData& blockData = (*cd)[checkPos]; 
    Item& blockType = blockData.blockType;
    if (blockType == OAK_LEAVES) return 1;
    return isAir(blockType)           ||
           blockType.isFlat()         ||
           currentBlockType == TORCH;
}

void emitFace(Mesh& m, int face, Item blockType, float x, float y, float z, vec3 direction) {
    float tintr = 1.0f, tintg = 1.0f, tintb = 1.0f;
    if (blockType == GRASS_BLOCK && face != 4) {
        tintr = 0.2f, tintg = 1.0f, tintb = 0.2f;
    }
    else if (blockType == GRASS) {
        tintr = 0.2f, tintg = 1.45f, tintb = 0.15f;
    }
    else if (blockType == OAK_LEAVES) {
        tintr = 0.2f, tintg = 1.0f, tintb = 0.2f;
    }
    //vec3 rgb = { tintr, tintg, tintb };
    vec3 cMask[4] = { { tintr, tintg, tintb }, { tintr, tintg, tintb }, { tintr, tintg, tintb }, { tintr, tintg, tintb } };

    // face: 0=-X,1=+X,2=-Z,3=+Z,4=+Z,5=-Z
    // Define 4 positions and normal per face
    static const glm::vec3 normals[6] = {
        {1, 0, 0}, {-1, 0, 0}, {0, 0, 1}, {0, 0, -1}, {0, 1, 0}, { 0, -1, 0 }
    };
    glm::vec3 n = normals[face];

    const int bottomDir = 4 - 4 * (-abs(direction.y) + 1) + (abs(direction.y)) * (0.5 * (-direction.y + 1)) + (-abs(direction.y) + 1) * (0.5 * (-direction.x + 1)) * (-direction.z * direction.z + 1) + (-abs(direction.y) + 1) * (-direction.x * direction.x + 1) * (2 + 0.5 * (-direction.z + 1));
    const int topDir    = 5 - 4 * (-abs(direction.y) + 1) + (abs(direction.y)) * (0.5 * (direction.y + 1) - 1) + (-abs(direction.y) + 1) * (0.5 * (direction.x + 1) - 1) * (-direction.z * direction.z + 1) + (-abs(direction.y) + 1) * (-direction.x * direction.x + 1) * (1 + 0.5 * (direction.z + 1));

    glm::vec3 v[4];
    if (blockType == TORCH) {
        switch (face) {
            case 0: /* left face */   v[0] = { x + 0.4375, y + 0.0, z + 0.4375 };      v[1] = { x + 0.4375, y + 0.5625, z + 0.4375 };   v[2] = { x + 0.4375, y + 0.5625, z + 0.5625 };   v[3] = { x + 0.4375, y + 0.0, z + 0.5625 };    break;
            case 1: /* right face */  v[0] = { x + 0.5625, y + 0.0, z + 0.4375 };      v[1] = { x + 0.5625, y + 0.0, z + 0.5625 };      v[2] = { x + 0.5625, y + 0.5625, z + 0.5625 };   v[3] = { x + 0.5625, y + 0.5625, z + 0.4375 }; break;
            case 2: /* front face */  v[0] = { x + 0.4375, y + 0.0, z + 0.4375 };      v[1] = { x + 0.5625, y + 0.0, z + 0.4375 };      v[2] = { x + 0.5625, y + 0.5625, z + 0.4375 };   v[3] = { x + 0.4375, y + 0.5625, z + 0.4375 }; break;
            case 3: /* back face */   v[0] = { x + 0.4375, y + 0.0, z + 0.5625 };      v[1] = { x + 0.4375, y + 0.5625, z + 0.5625 };   v[2] = { x + 0.5625, y + 0.5625, z + 0.5625 };   v[3] = { x + 0.5625, y + 0.0, z + 0.5625 };    break;
            case 4: /* bottom face */ v[0] = { x + 0.0   , y + 0.0, z + 0.0 };         v[1] = { x + 0.0, y + 0.0, z + 0.0 };            v[2] = { x + 0.0, y + 0.0, z + 0.0 };            v[3] = { x + 0.0, y + 0.0, z + 0.0 };          break;
            case 5: /* top face */    v[0] = { x + 0.4375, y + 0.5625, z + 0.4375 };   v[1] = { x + 0.5625, y + 0.5625, z + 0.4375 };   v[2] = { x + 0.5625, y + 0.5625, z + 0.5625 };   v[3] = { x + 0.4375, y + 0.5625, z + 0.5625 }; break;
        }
    }
    else if (!blockType.isFlat()) {
        switch (face) {
            case 0: v[0] = { x + -0.5f,y + -0.5f,z + -0.5f };   v[1] = { x + -0.5f,y +  0.5f,z + -0.5f }; v[2] = { x + -0.5f,y +  0.5f,z +  0.5f }; v[3] = { x + -0.5f,y + -0.5f,z +  0.5f }; break; // -X
            case 1: v[0] = { x +  0.5f,y + -0.5f,z + -0.5f };   v[1] = { x +  0.5f,y + -0.5f,z +  0.5f }; v[2] = { x +  0.5f,y +  0.5f,z +  0.5f }; v[3] = { x +  0.5f,y +  0.5f,z + -0.5f }; break; // +X
            case 2: v[0] = { x + -0.5f,y + -0.5f,z + -0.5f };   v[1] = { x +  0.5f,y + -0.5f,z + -0.5f }; v[2] = { x +  0.5f,y +  0.5f,z + -0.5f }; v[3] = { x + -0.5f,y +  0.5f,z + -0.5f }; break; // -Z
            case 3: v[0] = { x + -0.5f,y + -0.5f,z +  0.5f };   v[1] = { x + -0.5f,y +  0.5f,z +  0.5f }; v[2] = { x +  0.5f,y +  0.5f,z +  0.5f }; v[3] = { x +  0.5f,y + -0.5f,z +  0.5f }; break; // +Z
            case 4: v[0] = { x + -0.5f,y + -0.5f,z + -0.5f };   v[1] = { x + -0.5f,y + -0.5f,z +  0.5f }; v[2] = { x +  0.5f,y + -0.5f,z +  0.5f }; v[3] = { x +  0.5f,y + -0.5f,z + -0.5f }; break; // -Y
            case 5: v[0] = { x + -0.5f,y +  0.5f,z + -0.5f };   v[1] = { x +  0.5f,y +  0.5f,z + -0.5f }; v[2] = { x +  0.5f,y +  0.5f,z +  0.5f }; v[3] = { x + -0.5f,y +  0.5f,z +  0.5f }; break; // +Y
        }
    }
    else {
        switch (face) {
            case 0: v[0] = { x + -0.5f,y + -0.5f,z + -0.5f };   v[1] = { x + -0.5f,y +  0.5f,z + -0.5f }; v[2] = { x +  0.5f,y +  0.5f,z +  0.5f }; v[3] = { x +  0.5f,y + -0.5f,z +  0.5f };  break; // -X disgonal
            case 1: v[0] = { x + -0.5f,y + -0.5f,z +  0.5f };   v[1] = { x + -0.5f,y +  0.5f,z +  0.5f }; v[2] = { x +  0.5f,y +  0.5f,z + -0.5f }; v[3] = { x +  0.5f,y + -0.5f,z + -0.5f };  break; // +X diagonal
            case 2: v[0] = { x +  0.5f,y + -0.5f,z +  0.5f };   v[1] = { x +  0.5f,y +  0.5f,z +  0.5f }; v[2] = { x + -0.5f,y +  0.5f,z + -0.5f }; v[3] = { x + -0.5f,y + -0.5f,z + -0.5f };  break; // -X disgonal
            case 3: v[0] = { x +  0.5f,y + -0.5f,z + -0.5f };   v[1] = { x +  0.5f,y +  0.5f,z + -0.5f }; v[2] = { x + -0.5f,y +  0.5f,z +  0.5f }; v[3] = { x + -0.5f,y + -0.5f,z +  0.5f };  break; // +X diagonal
        }
    }

    // Simple tile UV (replace with atlas lookup per block/face)
    vector<glm::vec3> uv;

    float UVs[7];
    getUVs(blockType, UVs);
    float xoffset = UVs[0],
        yoffset = UVs[1],
        xoffsetTop = UVs[2],
        yoffsetTop = UVs[3],
        xoffsetBottom = UVs[4],
        yoffsetBottom = UVs[5],
        transparency = UVs[6];

    float clipX = 0.0f, clipY = 1.0f;
    int offsetX = 0, offsetY = 0;

    if (blockType == TORCH) { clipX = 0.4375 - 0.5; clipY = 0.5625 - 0.5; }

    if (face == bottomDir) { offsetX = xoffsetBottom; offsetY = yoffsetBottom; }
    else if (face == topDir) { offsetX = xoffsetTop;  offsetY = yoffsetTop; }

    uint32_t uintUVs = ((xdimens << 16) & 0xFFFFFF) | ((uint8_t)(yoffset + offsetY) << 8) | ((uint8_t)(xoffset + offsetX)); // Packaging floats into one integer
    float startUvs;
    memcpy(&startUvs, &uintUVs, sizeof(float));

    auto feed = [](vector<vec3>& uv, float l, float h, float packed, int dir, float w = 0) { // l is for low and h is for high; Transmits texture coordinates to vector.
        if (!dir) { // cw feed
            uv.push_back(vec3(l, l, packed));
            uv.push_back(vec3(l, h, packed));
            uv.push_back(vec3(h, h, packed));
            uv.push_back(vec3(h, l, packed));
        }
        else if(dir == 1){ // ccw feed
            uv.push_back(vec3(l, l, packed));
            uv.push_back(vec3(h, l, packed));
            uv.push_back(vec3(h, h, packed));
            uv.push_back(vec3(l, h, packed));
        }
        else if (dir == 2) {
            uv.push_back(vec3(l, h, packed));
            uv.push_back(vec3(l, l, packed));
            uv.push_back(vec3(h, l, packed));
            uv.push_back(vec3(h, h, packed));
        }
        else if (dir == 3) {
            uv.push_back(vec3(h, l, packed));
            uv.push_back(vec3(l, l, packed));
            uv.push_back(vec3(l, h, packed));
            uv.push_back(vec3(h, h, packed));
        }
        else if(dir == 4){
            uv.push_back(vec3(l, l, packed));
            uv.push_back(vec3(h, l, packed));
            uv.push_back(vec3(h, w, packed));
            uv.push_back(vec3(l, w, packed));
        }
        else if (dir == 5) {
            uv.push_back(vec3(l, l, packed));
            uv.push_back(vec3(l, w, packed));
            uv.push_back(vec3(h, w, packed));
            uv.push_back(vec3(h, l, packed));
        }
    };

    switch (face) {
        case 1:  
        case 2: 
        case 5: if (!blockType.isFlat() || blockType == TORCH) {
                    if      (abs(direction.x) && !blockType.isFlat() || abs(direction.z) && !blockType.isFlat()) { feed(uv, 0.0f, 1.0f, startUvs, 0); }
                    else if (blockType == TORCH) { feed(uv, 0.0f, 1.0f, startUvs, 4, 0.625f); }
                    else { feed(uv, 0.0f, 1.0f, startUvs, 1); }
                }
                else { // for grass and other flats
                    feed(uv, 1.0f, 0.0f, startUvs, 2);
                }
                break;
        default:
                if (blockType == TORCH && face != 5 && blockType != GRASS) { feed(uv, 0.0f, 1.0f, startUvs, 5, 0.625f); }
                else {
                    if (abs(direction.x) && !blockType.isFlat()) {
                        if (face == 5) { feed(uv, 1.0f, 0.0f, startUvs, 0); }
                        else { feed(uv, 1.0f, 0.0f, startUvs, 3); }
                    }
                    else if (abs(direction.z) && !blockType.isFlat()) {
                        if      (face == 5) { feed(uv, 0.0f, 1.0f, startUvs, 3); }
                        else if (face == 4) { feed(uv, 0.0f, 1.0f, startUvs, 0); }
                        else { feed(uv, 1.0f, 0.0f, startUvs, 3); }
                    }
                    else { feed(uv, 0.0f, 1.0f, startUvs, 0); }
                }
                break;
    }

    uint32_t base = (uint32_t)(m.vertices.size() / 12);
    for (int i = 0; i < 4; i++) {
        m.vertices.push_back(v[i].x);
        m.vertices.push_back(v[i].y);
        m.vertices.push_back(v[i].z);
        m.vertices.push_back(uv[i].x);
        m.vertices.push_back(uv[i].y);
        m.vertices.push_back(uv[i].z);
        m.vertices.push_back(n.x);
        m.vertices.push_back(n.y);
        m.vertices.push_back(n.z);
        m.vertices.push_back(cMask[i].x);
        m.vertices.push_back(cMask[i].y);
        m.vertices.push_back(cMask[i].z);
    }
    // Two triangles (0,1,2) (2,3,0)
    m.indices.push_back(base + 1); m.indices.push_back(base + 2); m.indices.push_back(base + 3);
    m.indices.push_back(base + 3); m.indices.push_back(base + 0); m.indices.push_back(base + 1);
}

void meshChunk(vec2 xyChunk, Chunk* cd, Mesh& out, vec3 direction, ivec3 position) {
    //out.vertices.clear(); out.indices.clear();
    //out.vertices.reserve(CHUNK_SIZE * (CHUNK_SIZE * CHUNK_SIZE)); // heuristic
    //out.indices.reserve(CHUNK_SIZE * (CHUNK_SIZE * CHUNK_SIZE));

    for (int x = (xyChunk.x) * CHUNK_SIZE; x < (xyChunk.x + 1) * CHUNK_SIZE; ++x)
        for (int y = 0; y < (CHUNK_SIZE * CHUNK_SIZE); ++y) // + 2 * CHUNK_SIZE
            for (int z = (xyChunk.y) * CHUNK_SIZE; z < (xyChunk.y + 1) * CHUNK_SIZE; ++z) {
                ivec3 blockPos = ivec3(x, y, z);
                BlockData& block_data = (*cd)[blockPos];
                Item& blockType = block_data.blockType;
                if (isAir(block_data.blockType)) continue;
                auto absl = [](int n) { return n >= 0 ? n : -n; };
                auto sign_shift = [](uint8_t num, int shift) { return ((num << shift & 1) == 1 ? -1 : 1); };
                vec3 blockOrientation = vec3(sign_shift(block_data.orientation, 5) * (block_data.orientation >> 4) & 1,
                                             sign_shift(block_data.orientation, 3) * (block_data.orientation >> 2) & 1,
                                             sign_shift(block_data.orientation, 1) * (block_data.orientation >> 0) & 1);
                if (direction != vec3(0, 1, 0) && position == blockPos) {
                    block_data.orientation = ((direction.x < 0 ? 1 : 0) << 5) | ((absl(direction.x)) << 4) |
                                             ((direction.x < 0 ? 1 : 0) << 3) | ((absl(direction.y)) << 2) |
                                             ((direction.x < 0 ? 1 : 0) << 1) | ((absl(direction.z)) << 0);
                    blockOrientation = direction;              
                }

                if (!blockType.isFlat() || blockType == TORCH)
                {
                    if (shouldEmitFace(xyChunk, cd, blockType, x, y, z, -1, 0, 0)) emitFace(out, 0, blockType, x, y, z, blockOrientation);
                    if (shouldEmitFace(xyChunk, cd, blockType, x, y, z, +1, 0, 0)) emitFace(out, 1, blockType, x, y, z, blockOrientation);
                    if (shouldEmitFace(xyChunk, cd, blockType, x, y, z, 0, 0, -1)) emitFace(out, 2, blockType, x, y, z, blockOrientation);
                    if (shouldEmitFace(xyChunk, cd, blockType, x, y, z, 0, 0, +1)) emitFace(out, 3, blockType, x, y, z, blockOrientation);
                    if (shouldEmitFace(xyChunk, cd, blockType, x, y, z, 0, -1, 0)) emitFace(out, 4, blockType, x, y, z, blockOrientation);
                    if (shouldEmitFace(xyChunk, cd, blockType, x, y, z, 0, +1, 0)) emitFace(out, 5, blockType, x, y, z, blockOrientation);
                }
                else {                    
                    for (int i = 0; i < 4; i++) {
                        emitFace(out, i, blockType, x, y, z, direction);
                    }
                }
            }
}

//void generateChunkAt(vec2 xyChunk, Chunk* repChunk) {
//    auto start = std::chrono::high_resolution_clock::now();
//
//    generateBlocks(xyChunk, repChunk);
//    meshChunk(xyChunk, repChunk, repChunk->mesh, vec3(0, 1, 0), vec3(0, -1, 0));
//
//    auto end = std::chrono::high_resolution_clock::now();
//    cout << repChunk->mesh.vertices.size() << " : Elapsed: " << std::chrono::duration<double>(end - start).count() << " s\n";
//}

/// <summary>
/// This will begin an attempt for a fast greedy meshing algorithm
/// </summary>

bool buildMask(Chunk* ch, int planeDirVal, int faceDir, vec2& xyChunk, int W, int H, vector<uint8_t>& mask) {
    mask.assign(W * H, 0);
    int dir = faceDir / 2; bool allAir = true;
    ivec3 normal = (faceDir == 0) ? ivec3(-1, 0, 0) : (faceDir == 1) ? ivec3( 1, 0, 0) : (faceDir == 2) ? ivec3( 0, 0,-1) : (faceDir == 3) ? ivec3( 0, 0, 1) : (faceDir == 4) ? ivec3(0,-1, 0) : ivec3( 0, 1, 0);
    for (int l = 0; l < H; l++) {
        for (int b = 0; b < W; b++) {
            ivec3 pos = (dir == 0) ? ivec3(planeDirVal, l, xyChunk.y * CHUNK_SIZE + b) : (dir == 1) ? ivec3(xyChunk.x * CHUNK_SIZE + b, l, planeDirVal) : ivec3(xyChunk.x * CHUNK_SIZE + b, planeDirVal, xyChunk.y * CHUNK_SIZE + l);
            Item blockType = AIR;
            if (ch->inBounds(pos)) {
                blockType = (*ch)[pos].blockType;
                if (isAir(blockType)) { continue; }
            }        
            if (shouldEmitFace(xyChunk, ch, blockType, pos.x, pos.y, pos.z, normal.x, normal.y, normal.z)) mask[b + l * W] = blockType.id;
            if (mask[b + l * W] != mask[0] || (mask[b + l * W] != 0)) allAir = 0;
        }
    }
    return allAir;
}

void emitFace(Mesh& m, int face, uint8_t blockType, ivec3 blockPos, ivec3 dims) {
    float tintr = 1.0f, tintg = 1.0f, tintb = 1.0f;
    if (blockType == GRASS_BLOCK.id && face != 4) {
        tintr = 0.2f, tintg = 1.0f, tintb = 0.2f;
    }
    else if (blockType == GRASS.id) {
        tintr = 0.2f, tintg = 1.45f, tintb = 0.15f;
    }
    else if (blockType == OAK_LEAVES.id) {
        tintr = 0.2f, tintg = 1.0f, tintb = 0.2f;
    }
    //vec3 rgb = { tintr, tintg, tintb };
    vec3 cMask[4] = { { tintr, tintg, tintb }, { tintr, tintg, tintb }, { tintr, tintg, tintb }, { tintr, tintg, tintb } };

    // face: 0=-X,1=+X,2=-Z,3=+Z,4=+Z,5=-Z
    // Define 4 positions and normal per face
    static const glm::vec3 normals[6] = {
        {1, 0, 0}, {-1, 0, 0}, {0, 0, 1}, {0, 0, -1}, {0, 1, 0}, { 0, -1, 0 }
    };
    glm::vec3 n = normals[face];

    const int bottomDir = 4;
    const int topDir = 5;

    glm::vec3 v[4];
    bool flat = item(blockType).isFlat();

    if (!flat) {
        switch (face) {
            case 0: v[0] = { blockPos.x + -0.5f         , blockPos.y + -0.5f         , blockPos.z + -0.5f          };    v[1] = { blockPos.x + -0.5f         ,blockPos.y + -0.5f + dims.y,blockPos.z + -0.5f          };  v[2] = { blockPos.x + -0.5f         ,blockPos.y + -0.5f + dims.y,blockPos.z + -0.5f + dims.z };  v[3] = { blockPos.x + -0.5f         ,blockPos.y + -0.5f         ,blockPos.z + -0.5f + dims.z }; break; // -X
            case 1: v[0] = { blockPos.x + -0.5f + dims.x, blockPos.y + -0.5f         , blockPos.z + -0.5f          };    v[1] = { blockPos.x + -0.5f + dims.x,blockPos.y + -0.5f         ,blockPos.z + -0.5f + dims.z };  v[2] = { blockPos.x + -0.5f + dims.x,blockPos.y + -0.5f + dims.y,blockPos.z + -0.5f + dims.z };  v[3] = { blockPos.x + -0.5f + dims.x,blockPos.y + -0.5f + dims.y,blockPos.z + -0.5f          }; break; // +X
            case 2: v[0] = { blockPos.x + -0.5f         , blockPos.y + -0.5f         , blockPos.z + -0.5f          };    v[1] = { blockPos.x + -0.5f + dims.x,blockPos.y + -0.5f         ,blockPos.z + -0.5f          };  v[2] = { blockPos.x + -0.5f + dims.x,blockPos.y + -0.5f + dims.y,blockPos.z + -0.5f          };  v[3] = { blockPos.x + -0.5f         ,blockPos.y + -0.5f + dims.y,blockPos.z + -0.5f          }; break; // -Z
            case 3: v[0] = { blockPos.x + -0.5f         , blockPos.y + -0.5f         , blockPos.z + -0.5f + dims.z };    v[1] = { blockPos.x + -0.5f         ,blockPos.y + -0.5f + dims.y,blockPos.z + -0.5f + dims.z };  v[2] = { blockPos.x + -0.5f + dims.x,blockPos.y + -0.5f + dims.y,blockPos.z + -0.5f + dims.z };  v[3] = { blockPos.x + -0.5f + dims.x,blockPos.y + -0.5f         ,blockPos.z + -0.5f + dims.z }; break; // +Z
            case 4: v[0] = { blockPos.x + -0.5f         , blockPos.y + -0.5f         , blockPos.z + -0.5f          };    v[1] = { blockPos.x + -0.5f         ,blockPos.y + -0.5f         ,blockPos.z + -0.5f + dims.z };  v[2] = { blockPos.x + -0.5f + dims.x,blockPos.y + -0.5f         ,blockPos.z + -0.5f + dims.z };  v[3] = { blockPos.x + -0.5f + dims.x,blockPos.y + -0.5f         ,blockPos.z + -0.5f          }; break; // -Y
            case 5: v[0] = { blockPos.x + -0.5f         , blockPos.y + -0.5f + dims.y, blockPos.z + -0.5f          };    v[1] = { blockPos.x + -0.5f + dims.x,blockPos.y + -0.5f + dims.y,blockPos.z + -0.5f          };  v[2] = { blockPos.x + -0.5f + dims.x,blockPos.y + -0.5f + dims.y,blockPos.z + -0.5f + dims.z };  v[3] = { blockPos.x + -0.5f         ,blockPos.y + -0.5f + dims.y,blockPos.z + -0.5f + dims.z }; break; // +Y
        }
    }
    else {
        switch (face) {
            case 0: v[0] = { blockPos.x + -0.5f,blockPos.y + -0.5f,blockPos.z + -0.5f };   v[1] = { blockPos.x + -0.5f,blockPos.y + -0.5f + dims.y,blockPos.z + -0.5f }; v[2] = { blockPos.x +  0.5f,blockPos.y + -0.5f + dims.y,blockPos.z +  0.5f }; v[3] = { blockPos.x +  0.5f,blockPos.y + -0.5f,blockPos.z +  0.5f };  break; // -X disgonal
            case 1: v[0] = { blockPos.x + -0.5f,blockPos.y + -0.5f,blockPos.z +  0.5f };   v[1] = { blockPos.x + -0.5f,blockPos.y + -0.5f + dims.y,blockPos.z +  0.5f }; v[2] = { blockPos.x +  0.5f,blockPos.y + -0.5f + dims.y,blockPos.z + -0.5f }; v[3] = { blockPos.x +  0.5f,blockPos.y + -0.5f,blockPos.z + -0.5f };  break; // +X diagonal
            case 2: v[0] = { blockPos.x +  0.5f,blockPos.y + -0.5f,blockPos.z +  0.5f };   v[1] = { blockPos.x +  0.5f,blockPos.y + -0.5f + dims.y,blockPos.z +  0.5f }; v[2] = { blockPos.x + -0.5f,blockPos.y + -0.5f + dims.y,blockPos.z + -0.5f }; v[3] = { blockPos.x + -0.5f,blockPos.y + -0.5f,blockPos.z + -0.5f };  break; // -X disgonal
            case 3: v[0] = { blockPos.x +  0.5f,blockPos.y + -0.5f,blockPos.z + -0.5f };   v[1] = { blockPos.x +  0.5f,blockPos.y + -0.5f + dims.y,blockPos.z + -0.5f }; v[2] = { blockPos.x + -0.5f,blockPos.y + -0.5f + dims.y,blockPos.z +  0.5f }; v[3] = { blockPos.x + -0.5f,blockPos.y + -0.5f,blockPos.z +  0.5f };  break; // +X diagonal
        }
    }


    // Simple tile UV (replace with atlas lookup per block/face)
    vector<glm::vec3> uv;

    float UVs[7];
    getUVs(blockType, UVs);
    float xoffset = UVs[0],
        yoffset = UVs[1],
        xoffsetTop = UVs[2],
        yoffsetTop = UVs[3],
        xoffsetBottom = UVs[4],
        yoffsetBottom = UVs[5],
        transparency = UVs[6];
    int dir = face / 2;
    float clipX = 0.0f, clipY = 1.0f;
    float clipXX = 0.0f, clipXY = !flat ? ((dir == 0) ? dims.z : dims.x) : 1.0f,
          clipYX = 0.0f, clipYY = !flat ? ((dir == 2) ? dims.z : dims.y) : 1.0f;
    int offsetX = 0, offsetY = 0;

    //if (blockType == TORCH) { clipX = 0.4375; clipY = 0.5625; }

    if (face == bottomDir) { offsetX = xoffsetBottom; offsetY = yoffsetBottom; }
    else if (face == topDir) { offsetX = xoffsetTop;  offsetY = yoffsetTop; }

    uint32_t uintUVs = ((uint8_t)(xdimens) << 16) | ((uint8_t)(yoffset + offsetY) << 8) | (uint8_t)(xoffset + offsetX); // Packaging floats into one integer
    float startUvs;
    memcpy(&startUvs, &uintUVs, sizeof(float));

    auto feed = [](vector<vec3>& uv, float w, float h, float packed, int direction) { // direction = 0 ? clockwise : counterclockwise
        if (!direction) { //feedCW
            uv.push_back(vec3(0, 0, packed));
            uv.push_back(vec3(0, h, packed));
            uv.push_back(vec3(w, h, packed));
            uv.push_back(vec3(w, 0, packed));
        }
        else { // feedCCW
            uv.push_back(vec3(0, 0, packed));
            uv.push_back(vec3(w, 0, packed));
            uv.push_back(vec3(w, h, packed));
            uv.push_back(vec3(0, h, packed));
        }
    };
    
    switch (face) {
    case 0:
    case 3:
    case 4:
        feed(uv, clipXY, clipYY, startUvs, 0);
        break;
    default:
        if (flat) { feed(uv, clipXY, clipYY, startUvs, 0); }
        else { feed(uv, clipXY, clipYY, startUvs, 1); };
        break;
    }

    uint32_t base = (uint32_t)(m.vertices.size() / 12);
    for (int i = 0; i < 4; i++) {
        m.vertices.push_back(v[i].x);
        m.vertices.push_back(v[i].y);
        m.vertices.push_back(v[i].z);
        m.vertices.push_back(uv[i].x);
        m.vertices.push_back(uv[i].y);
        m.vertices.push_back(uv[i].z);
        m.vertices.push_back(n.x);
        m.vertices.push_back(n.y);
        m.vertices.push_back(n.z);
        m.vertices.push_back(cMask[i].x);
        m.vertices.push_back(cMask[i].y);
        m.vertices.push_back(cMask[i].z);
    }
    // Two triangles (0,1,2) (2,3,0)
    m.indices.push_back(base + 1); m.indices.push_back(base + 2); m.indices.push_back(base + 3);
    m.indices.push_back(base + 3); m.indices.push_back(base + 0); m.indices.push_back(base + 1);
}

void greedyMerge(vector<uint8_t>& mask, Mesh& m, vec2& xyChunk, int planeDirVal, int W, int H, int faceDir) {
    int dir = faceDir / 2;
    for (int l = 0; l < H; l++) {
        for (int b = 0; b < W;) {
            int idx = b + l * W, type = mask[idx];
            if (type == 0) {
                b++; continue;
            } 

            int w = 1, h = 1;
            while (b + w < W && mask[idx + w] == type) { w++; }
            while (l + h < H) {
                for (int d = 0; d < w; d++) {
                    if (mask[b + d + (l + h) * W] != type) goto not_type;
                }
                h++;
            }
        not_type:
            ivec3 dims = (dir == 0) ? vec3(1, h, w) : (dir == 1) ? vec3(w, h, 1) : vec3(w, 1, h);
            ivec3 blockPos = (dir == 0) ? ivec3(planeDirVal, l, xyChunk.y * CHUNK_SIZE + b) : (dir == 1) ? ivec3(xyChunk.x * CHUNK_SIZE + b, l, planeDirVal) : ivec3(xyChunk.x * CHUNK_SIZE + b, planeDirVal, xyChunk.y * CHUNK_SIZE + l);
            emitFace(m, faceDir, type, blockPos, dims);

            for (int i = 0; i < h; i++) {
                for (int j = 0; j < w; j++) {
                    mask[b+j + (l+i) * W] = 0;
                }
            }
            b += w;
        }
    }
}
void meshChunk(vec2 xyChunk, Chunk* ch, Mesh& m) {
    vector<uint8_t> mask;
    mask.reserve(CHUNK_SIZE * CHUNK_HEIGHT);
    uint8_t terrainHeight = ch->terrainHeight;

    for (int x = (xyChunk.x) * CHUNK_SIZE - 1; x < (xyChunk.x + 1) * CHUNK_SIZE; ++x) {
        buildMask(ch, x + 0, 0, xyChunk, CHUNK_SIZE, CHUNK_HEIGHT, mask);
        greedyMerge(mask, m, xyChunk, x + 0, CHUNK_SIZE, CHUNK_HEIGHT, 0);
        

        buildMask(ch, x + 1, 1, xyChunk, CHUNK_SIZE, CHUNK_HEIGHT, mask);
        greedyMerge(mask, m, xyChunk, x + 1, CHUNK_SIZE, CHUNK_HEIGHT, 1);
    }

    for (int z = (xyChunk.y) * CHUNK_SIZE - 1; z < (xyChunk.y + 1) * CHUNK_SIZE; ++z) {
        buildMask(ch, z + 0, 2, xyChunk, CHUNK_SIZE, CHUNK_HEIGHT, mask);
        greedyMerge(mask, m, xyChunk, z + 0, CHUNK_SIZE, CHUNK_HEIGHT, 2);

        buildMask(ch, z + 1, 3, xyChunk, CHUNK_SIZE, CHUNK_HEIGHT, mask);
        greedyMerge(mask, m, xyChunk, z + 1, CHUNK_SIZE, CHUNK_HEIGHT, 3);
    }

    for (int y = 0; y < terrainHeight; ++y) { //CHUNK_SIZE*(CHUNK_SIZE + 2) + 5
        if (!buildMask(ch, y + 0, 4, xyChunk, CHUNK_SIZE, CHUNK_SIZE, mask));
            greedyMerge(mask, m, xyChunk, y + 0, CHUNK_SIZE, CHUNK_SIZE, 4);

        if (!buildMask(ch, y + 1, 5, xyChunk, CHUNK_SIZE, CHUNK_SIZE, mask));
            greedyMerge(mask, m, xyChunk, y + 1, CHUNK_SIZE, CHUNK_SIZE, 5);
    }
}

void generateChunkAt(vec2 xyChunk, Chunk* repChunk) {
    //auto start = std::chrono::high_resolution_clock::now();

    generateBlocks(xyChunk, repChunk);
    //meshChunk(xyChunk, repChunk, repChunk->mesh);

    //auto end = std::chrono::high_resolution_clock::now();
    //cout << repChunk->mesh.vertices.size() << "count : " << chunkCount << " : Elapsed: " << std::chrono::duration<double>(end - start).count() << " s\n";
}
//void buildMask_PosX(vec2 xyChunk,
//    Chunk* cd,
//    int x,
//    int yMin, int yMax,
//    int zMin, int zMax,
//    std::vector<MaskCell>& mask,
//    int W, int H)
//{
//    // W = number of z cells, H = number of y cells
//    mask.assign(W * H, AIR);
//
//    for (int y = yMin; y <= yMax; ++y) {
//        for (int z = zMin; z <= zMax; ++z) {
//
//            // local indices in the mask
//            int iy = y - yMin;
//            int iz = z - zMin;
//            int idx = iz + iy * W;
//
//            ivec3 pos(x, y, z);
//
// //            BlockData& bd = cd->block_data[cd->at(pos)];
//            Item blockType = bd.blockType;
//            if (isAir(blockType)) {
//                mask[idx] = AIR;
//                continue;
//            }
//
//            // Reuse your current visibility logic:
//            // dx = +1, dy = 0, dz = 0
//            if (shouldEmitFace(xyChunk, cd, x, y, z, +1, 0, 0)) {
//                mask[idx] = blockType;
//            }
//            else {
//                mask[idx] = AIR;
//            }
//        }
//    }
//}
//Here:
//
//W = zMax - zMin + 1
//
//H = yMax - yMin + 1
//
//4. Emitting a single greedy quad on + X
//This is a simplified version that :
//
//Uses side UVs only.
//
//Uses a neutral color mask(1, 1, 1).
//
//Uses a simple rectangular UV mapping scaled by w and h.
//
//We can later patch it to perfectly match your atlas / tint rules.
//
//cpp
//void emitGreedyQuad_PosX(Mesh & m,
//    Item blockType,
//    int x, int y, int z,
//    int w, int h,
//    int yMin, int zMin)
//{
//    // world-space base coords
//    int y0 = yMin + y;
//    int z0 = zMin + z;
//
//    // 4 corners of the quad (face at x + 0.5)
//    glm::vec3 v0 = { x + 0.5f, (float)y0,         (float)z0 };
//    glm::vec3 v1 = { x + 0.5f, (float)(y0 + h),   (float)z0 };
//    glm::vec3 v2 = { x + 0.5f, (float)(y0 + h),   (float)(z0 + w) };
//    glm::vec3 v3 = { x + 0.5f, (float)y0,         (float)(z0 + w) };
//
//    glm::vec3 n = { 1.0f, 0.0f, 0.0f }; // +X
//
//    // --- UVs: simple version, scaled by w,h ---
//    // You can later plug in getUVs(blockType, ...) and pick the side tile.
//    glm::vec2 uv0 = { 0.0f, 0.0f };
//    glm::vec2 uv1 = { 0.0f, (float)h };
//    glm::vec2 uv2 = { (float)w, (float)h };
//    glm::vec2 uv3 = { (float)w, 0.0f };
//
//    glm::vec3 cMask = { 1.0f, 1.0f, 1.0f }; // neutral tint
//    float transparency = 1.0f; // you can derive from blockType later
//
//    uint32_t base = (uint32_t)(m.vertices.size() / 12);
//
//    auto push = [&](const glm::vec3& v, const glm::vec2& uv) {
//        m.vertices.push_back(v.x);
//        m.vertices.push_back(v.y);
//        m.vertices.push_back(v.z);
//
//        m.vertices.push_back(uv.x);
//        m.vertices.push_back(uv.y);
//        m.vertices.push_back(transparency);
//
//        m.vertices.push_back(n.x);
//        m.vertices.push_back(n.y);
//        m.vertices.push_back(n.z);
//
//        m.vertices.push_back(cMask.x);
//        m.vertices.push_back(cMask.y);
//        m.vertices.push_back(cMask.z);
//        };
//
//    push(v0, uv0);
//    push(v1, uv1);
//    push(v2, uv2);
//    push(v3, uv3);
//
//    // two triangles (0,1,2) (2,3,0)
//    m.indices.push_back(base + 0);
//    m.indices.push_back(base + 1);
//    m.indices.push_back(base + 2);
//
//    m.indices.push_back(base + 2);
//    m.indices.push_back(base + 3);
//    m.indices.push_back(base + 0);
//}
//5. Greedy merge on the + X mask for a given x
//cpp
//void greedyMerge_PosX(Chunk * cd,
//    Mesh & out,
//    std::vector<MaskCell>&mask,
//    int W, int H,
//    int x,
//    int yMin, int zMin)
//{
//    // Work on a copy so we can zero out merged regions
//    std::vector<MaskCell> tmp = mask;
//
//    for (int iy = 0; iy < H; ++iy) {
//        for (int iz = 0; iz < W; ) {
//
//            MaskCell cell = tmp[iz + iy * W];
//            if (cell == AIR) {
//                ++iz;
//                continue;
//            }
//
//            Item blockType = cell;
//
//            // 1) find width (in z)
//            int w = 1;
//            while (iz + w < W && tmp[iz + w + iy * W] == blockType)
//                ++w;
//
//            // 2) find height (in y)
//            int h = 1;
//            bool done = false;
//            while (iy + h < H && !done) {
//                for (int k = 0; k < w; ++k) {
//                    if (tmp[(iz + k) + (iy + h) * W] != blockType) {
//                        done = true;
//                        break;
//                    }
//                }
//                if (!done) ++h;
//            }
//
//            // 3) emit one quad for this rectangle
//            emitGreedyQuad_PosX(out, blockType, x, iy, iz, w, h, yMin, zMin);
//
//            // 4) zero out merged region
//            for (int dy = 0; dy < h; ++dy) {
//                for (int dx = 0; dx < w; ++dx) {
//                    tmp[(iz + dx) + (iy + dy) * W] = AIR;
//                }
//            }
//
//            iz += w;
//        }
//    }
//}
//6. Top - level greedy meshing for + X(only)
//Now we assemble it into a new mesher.Later we’ll add the other 5 directions.
//
//cpp
//void meshChunkGreedy_PosX(vec2 xyChunk, Chunk * cd, Mesh & out)
//{
//    // You can tune these to exactly match your previous y-range.
//    int yMin = 0;
//    int yMax = CHUNK_SIZE * CHUNK_SIZE + 2 * CHUNK_SIZE + 5; // as in your old loop
//    int zMin = xyChunk.y * CHUNK_SIZE;
//    int zMax = (xyChunk.y + 1) * CHUNK_SIZE - 1;
//
//    int xMin = xyChunk.x * CHUNK_SIZE;
//    int xMax = (xyChunk.x + 1) * CHUNK_SIZE - 1;
//
//    int W = zMax - zMin + 1;  // z dimension
//    int H = yMax - yMin + 1;  // y dimension
//
//    std::vector<MaskCell> mask;
//    mask.reserve(W * H);
//
//    for (int x = xMin; x <= xMax; ++x) {
//        buildMask_PosX(xyChunk, cd, x, yMin, yMax, zMin, zMax, mask, W, H);
//        greedyMerge_PosX(cd, out, mask, W, H, x, yMin, zMin);
//    }
//}
//Then your chunk generation becomes, for testing:
//
//cpp
//void generateChunkAt(vec2 xyChunk, Chunk * repChunk) {
//    auto start = std::chrono::high_resolution_clock::now();
//    generateBlocks(xyChunk, repChunk);
//
//    Mesh& m = repChunk->mesh;
//    m.vertices.clear();
//    m.indices.clear();
//
//    meshChunkGreedy_PosX(xyChunk, repChunk, m); // currently only +X
//
//    auto end = std::chrono::high_resolution_clock::now();
//    std::cout << repChunk->mesh.vertices.size()
//        << " : Elapsed: "
//        << std::chrono::duration<double>(end - start).count()
//        << " s\n";
//}
//For now you’ll see :
//
//Only + X faces rendered.
//
//Vertex count massively reduced compared to your old mesher(for those faces).
//
//Time per chunk much lower just from that direction.
//
//Once you’re happy with + X, we:
//
//Add - X, +Y, -Y, +Z, -Z variants with the same pattern.
//
//Replace the naive meshChunk entirely.
//
//Then fold back in full UV / tint / orientation logic into emitGreedyQuad_ * .
//
//If you paste this in and run it, what I’d love to see from you next is :
//
//The new vertices.size() and time with only + X greedy meshing.
//
//A screenshot / description of how the world looks(only + X faces visible).
//
//Then we’ll clone this for the remaining 5 directions and really crush your meshing time.
//
//ok.Just for testing, give the other 5 functions and the updated generateChunkAt function
//See my thinking
//Good, let’s wire up a full 6‑direction greedy mesher you can just drop in and test.
//
//I’ll:
//
//Give you all 6 directions(±X, ±Y, ±Z)
//
//Keep UVs / tints simple for now(just to validate correctness & perf)
//
//Plug it into an updated generateChunkAt
//
//You can then time it and visually verify, and we can refine UV / tint / orientation after.
//
//1. Shared types and helper
//Add near the top of your meshing file :
//
//cpp
//using MaskCell = Item; // or use ItemID / uint16_t if you prefer
//
//template<typename EmitFn>
//void greedyMerge2D(std::vector<MaskCell>&mask,
//    int W, int H,
//    EmitFn emit)
//{
//    std::vector<MaskCell> tmp = mask; // work copy so we can zero out merged regions
//
//    for (int v = 0; v < H; ++v) {
//        for (int u = 0; u < W; ) {
//
//            MaskCell cell = tmp[u + v * W];
//            if (cell == AIR) {
//                ++u;
//                continue;
//            }
//
//            Item blockType = cell;
//
//            // 1) find width in u
//            int w = 1;
//            while (u + w < W && tmp[u + w + v * W] == blockType)
//                ++w;
//
//            // 2) find height in v
//            int h = 1;
//            bool done = false;
//            while (v + h < H && !done) {
//                for (int k = 0; k < w; ++k) {
//                    if (tmp[(u + k) + (v + h) * W] != blockType) {
//                        done = true;
//                        break;
//                    }
//                }
//                if (!done) ++h;
//            }
//
//            // 3) emit one quad
//            emit(u, v, w, h, blockType);
//
//            // 4) zero out merged region
//            for (int dv = 0; dv < h; ++dv)
//                for (int du = 0; du < w; ++du)
//                    tmp[(u + du) + (v + dv) * W] = AIR;
//
//            u += w;
//        }
//    }
//}
//2. Emit functions for each direction
//These are simplified : neutral color, simple UVs scaled by w / h.
//
//+ X
//cpp
//void emitGreedyQuad_PosX(Mesh & m,
//    Item blockType,
//    int x, int u, int v, int w, int h,
//    int yMin, int zMin)
//{
//    int y0 = yMin + v;
//    int z0 = zMin + u;
//
//    glm::vec3 n = { 1.0f, 0.0f, 0.0f };
//
//    glm::vec3 v0 = { x + 0.5f, (float)y0,        (float)z0 };
//    glm::vec3 v1 = { x + 0.5f, (float)(y0 + h),  (float)z0 };
//    glm::vec3 v2 = { x + 0.5f, (float)(y0 + h),  (float)(z0 + w) };
//    glm::vec3 v3 = { x + 0.5f, (float)y0,        (float)(z0 + w) };
//
//    glm::vec2 uv0 = { 0.0f, 0.0f };
//    glm::vec2 uv1 = { 0.0f, (float)h };
//    glm::vec2 uv2 = { (float)w, (float)h };
//    glm::vec2 uv3 = { (float)w, 0.0f };
//
//    glm::vec3 cMask = { 1.0f, 1.0f, 1.0f };
//    float transparency = 1.0f;
//
//    uint32_t base = (uint32_t)(m.vertices.size() / 12);
//
//    auto push = [&](const glm::vec3& v, const glm::vec2& uv) {
//        m.vertices.push_back(v.x);
//        m.vertices.push_back(v.y);
//        m.vertices.push_back(v.z);
//
//        m.vertices.push_back(uv.x);
//        m.vertices.push_back(uv.y);
//        m.vertices.push_back(transparency);
//
//        m.vertices.push_back(n.x);
//        m.vertices.push_back(n.y);
//        m.vertices.push_back(n.z);
//
//        m.vertices.push_back(cMask.x);
//        m.vertices.push_back(cMask.y);
//        m.vertices.push_back(cMask.z);
//        };
//
//    push(v0, uv0);
//    push(v1, uv1);
//    push(v2, uv2);
//    push(v3, uv3);
//
//    m.indices.push_back(base + 0);
//    m.indices.push_back(base + 1);
//    m.indices.push_back(base + 2);
//    m.indices.push_back(base + 2);
//    m.indices.push_back(base + 3);
//    m.indices.push_back(base + 0);
//}
//void meshChunkGreedy(vec2 xyChunk, Chunk* cd, Mesh& out)
//{
//    int yMin = 0;
//    int yMax = CHUNK_SIZE * CHUNK_SIZE + 2 * CHUNK_SIZE + 5;
//
//    int xMin = xyChunk.x * CHUNK_SIZE;
//    int xMax = (xyChunk.x + 1) * CHUNK_SIZE - 1;
//
//    int zMin = xyChunk.y * CHUNK_SIZE;
//    int zMax = (xyChunk.y + 1) * CHUNK_SIZE - 1;
//
//    std::vector<MaskCell> mask;
//
//    // --- +X and -X (mask: z × y) ---
//    {
//        int W = zMax - zMin + 1;
//        int H = yMax - yMin + 1;
//        mask.reserve(W * H);
//
//        for (int x = xMin; x <= xMax; ++x) {
//            // +X
//            buildMask_PosX(xyChunk, cd, x, yMin, yMax, zMin, zMax, mask, W, H);
//            greedyMerge2D(mask, W, H, [&](int u, int v, int w, int h, Item blockType) {
//                emitGreedyQuad_PosX(out, blockType, x, u, v, w, h, yMin, zMin);
//                });
//
//            // -X
//            buildMask_NegX(xyChunk, cd, x, yMin, yMax, zMin, zMax, mask, W, H);
//            greedyMerge2D(mask, W, H, [&](int u, int v, int w, int h, Item blockType) {
//                emitGreedyQuad_NegX(out, blockType, x, u, v, w, h, yMin, zMin);
//                });
//        }
//    }
//
//    // --- +Y and -Y (mask: x × z) ---
//    {
//        int W = xMax - xMin + 1;
//        int H = zMax - zMin + 1;
//        mask.reserve(W * H);
//
//        for (int y = yMin; y <= yMax; ++y) {
//            // +Y
//            buildMask_PosY(xyChunk, cd, y, xMin, xMax, zMin, zMax, mask, W, H);
//            greedyMerge2D(mask, W, H, [&](int u, int v, int w, int h, Item blockType) {
//                emitGreedyQuad_PosY(out, blockType, y, u, v, w, h, xMin, zMin);
//                });
//
//            // -Y
//            buildMask_NegY(xyChunk, cd, y, xMin, xMax, zMin, zMax, mask, W, H);
//            greedyMerge2D(mask, W, H, [&](int u, int v, int w, int h, Item blockType) {
//                emitGreedyQuad_NegY(out, blockType, y, u, v, w, h, xMin, zMin);
//                });
//        }
//    }
//
//    // --- +Z and -Z (mask: x × y) ---
//    {
//        int W = xMax - xMin + 1;
//        int H = yMax - yMin + 1;
//        mask.reserve(W * H);
//
//        for (int z = zMin; z <= zMax; ++z) {
//            // +Z
//            buildMask_PosZ(xyChunk, cd, z, xMin, xMax, yMin, yMax, mask, W, H);
//            greedyMerge2D(mask, W, H, [&](int u, int v, int w, int h, Item blockType) {
//                emitGreedyQuad_PosZ(out, blockType, z, u, v, w, h, xMin, yMin);
//                });
//
//            // -Z
//            buildMask_NegZ(xyChunk, cd, z, xMin, xMax, yMin, yMax, mask, W, H);
//            greedyMerge2D(mask, W, H, [&](int u, int v, int w, int h, Item blockType) {
//                emitGreedyQuad_NegZ(out, blockType, z, u, v, w, h, xMin, yMin);
//                });
//        }
//    }
//}
//void generateChunkAt(vec2 xyChunk, Chunk* repChunk) {
//    auto start = std::chrono::high_resolution_clock::now();
//    generateBlocks(xyChunk, repChunk);
//
//    Mesh& m = repChunk->mesh;
//    m.vertices.clear();
//    m.indices.clear();
//
//    meshChunkGreedy(xyChunk, repChunk, m);
//
//    auto end = std::chrono::high_resolution_clock::now();
//    std::cout << repChunk->mesh.vertices.size()
//        << " : Elapsed: "
//        << std::chrono::duration<double>(end - start).count()
//        << " s\n";
//}


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
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            std::lock_guard<std::mutex> lock(breakReqMutex);
            world.delBlocklook_at();
            blockBreakingOut = false;
            if (!breakResQueue.empty())
                breakResQueue.pop();
        }
    }
    });

std::thread blockPlaceThread([&]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
            world.addBlocklook_at(inventory.mainInventorySlots[3][slot].item);
            blockPlacingOut = false;
            if (!placeResQueue.empty())
                placeResQueue.pop();
        }
    }
    });


void chunkWorker() {
    while (chunkGenRunning) {
        ivec2 coord; 
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCV.wait(lock, [] { return !chunkRequestQueue.empty(); });
            coord = chunkRequestQueue.front();
            chunkRequestQueue.pop();
        }

        unique_ptr<Chunk> newChunk = make_unique<Chunk>();
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            newChunk->toCoords(coord);
            generateChunkAt(coord, newChunk.get());
            chunkResultQueue.push(move(newChunk));
        }
    }
}////////////

std::thread chunkUpdateThread([&]() {
    while (chunkUpdateGenRunning) {
        {
            if (!chunkUpdateRequestQueue.empty()) {
                std::lock_guard<std::mutex> lock(chunkUpdateRequestMutex);
                Chunk& chunkToUpdate = (chunkUpdateRequestQueue.front());
                world.updateChunk(chunkToUpdate.coords(), vec3(0), vec3(0));
                chunkUpdateRequestQueue.pop();
            }
        }
    }
});