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



std::queue<Block> breakResQueue;
std::mutex breakResMutex;

std::queue<vec3> breakReqQueue;
std::mutex breakReqMutex;

std::queue<vec3> placeReqQueue;
std::mutex placeReqMutex;

std::queue<Block> placeResQueue;
std::mutex placeResMutex;

std::atomic<bool> chunkGenRunning = true;
std::atomic<bool> chunkGenRunning2 = true;
std::atomic<bool> chunkUpdateGenRunning = true;
std::atomic<bool> blockBreaking = true;
std::atomic<bool> blockPlacing = true;
std::atomic<bool> stopChunkUpdaters = false;
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
    baseNoise.SetFrequency(0.001f);      // low frequency = broad features
    baseNoise.SetFractalOctaves(3);
    baseNoise.SetFractalLacunarity(2.0f);
    baseNoise.SetFractalGain(0.5f);

    ridgedNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    ridgedNoise.SetFractalType(FastNoiseLite::FractalType_Ridged);
    ridgedNoise.SetFrequency(0.001f);     // higher frequency = more detail
    ridgedNoise.SetFractalOctaves(4);
    ridgedNoise.SetFractalLacunarity(2.0f);
    ridgedNoise.SetFractalGain(0.5f);

    //maskNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    maskNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    maskNoise.SetFrequency(0.01f);      // very low frequency = large biome regions
    maskNoise.SetFractalOctaves(4);
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
            float ridged = norm(ridgedNoise.GetNoise((float)x, (float)z)) * 25.0f;
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
            cloudDensity = cloudNoise.GetNoise((float)x, (float)z);
            if (cloudDensity < -0.25f) {
                ch[ivec3(x, CHUNK_SIZE * CHUNK_SIZE - 1, z)] = BlockData(CLOUD);
            }
            
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

                if (density < -0.2f) {
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
                            //blockData = BlockData(GRASS);
                            for (int i = 1; i <= randomFloat(1, 3); i++) {
                                ivec3 pos(x, y + i, z); BlockData& blockData = ch[pos];
                                blockData = BlockData(GRASS);
                            }
                        }
                        else if (randomNumberForDeco > 2000 && randomNumberForDeco < 2500) {
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
    //repChunk->terrainHeight = maxHeight;
}

bool isAir(Item item) { return item == AIR; }

bool shouldEmitFace(vec2 xyChunk, Chunk* cd, Item currentBlockType, int x, int y, int z, int dx, int dy, int dz) {
    if (currentBlockType.isFlat() || currentBlockType == OAK_PLANK) return 1;
    int nx = x + dx, ny = y + dy, nz = z + dz;
    ivec3 checkPos = ivec3(nx, ny, nz);
    if (!cd->inBounds(checkPos) && ny >= -1 && ny < CHUNK_HEIGHT) {
        if (ny == -1) return 1;
        uint32_t pos = pack(ivec2(floorDiv(checkPos.x, CHUNK_SIZE), floorDiv(checkPos.z, CHUNK_SIZE)));
        if (!world.chunkData.count(pos)) return 0;
        else {
            BlockData& bData = (*world.chunkData[pos])[checkPos];
            return (isAir(bData.blockType)) || bData.blockType.isFlat();
        }
    }
    else if (ny < 0 || ny >= CHUNK_HEIGHT) { return 0; }
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
    vec3 cMask = { tintr, tintg, tintb };

    // face: 0=-X,1=+X,2=-Z,3=+Z,4=+Z,5=-Z
    // Define 4 positions and normal per face
    static const glm::vec3 normals[6] = {
        {1, 0, 0}, {-1, 0, 0}, {0, 0, 1}, {0, 0, -1}, {0, 1, 0}, { 0, -1, 0 }
    };

    auto absl = [](int n) { return n >= 0 ? n : -n; };
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

    uint32_t uintUVs = ((uint8_t(y) << 24)) | ((xdimens << 16) & 0xFFFFFF) | ((uint8_t)(yoffset + offsetY) << 8) | ((uint8_t)(xoffset + offsetX)); // Packaging floats into one integer
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

    uint32_t norm_color = ((byte(n.x < 0 ? 1 : 0) & 0x1) << 5) | ((byte(absl(n.x)) & 0x1) << 4)
        | ((byte(n.y < 0 ? 1 : 0) & 0x1) << 3) | ((byte(absl(n.y)) & 0x1) << 2)
        | ((byte(n.z < 0 ? 1 : 0) & 0x1) << 1) | ((byte(absl(n.z)) & 0x1) << 0)
        | ((byte(cMask.x * 100) & 0x7F) << 20)
        | ((byte(cMask.y * 100) & 0x7F) << 13)
        | ((byte(cMask.z * 100) & 0x7F) <<  6);
    float normcolor;
    memcpy(&normcolor, &norm_color, sizeof(float));

    uint32_t base = (uint32_t)(m.vertices.size() / 7);
    for (int i = 0; i < 4; i++) {
        m.vertices.push_back(v[i].x);
        m.vertices.push_back(v[i].y);
        m.vertices.push_back(v[i].z);
        m.vertices.push_back(uv[i].x);
        m.vertices.push_back(uv[i].y);
        m.vertices.push_back(uv[i].z);
        m.vertices.push_back(normcolor);
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
bool shouldEmitFace(vec2 xyChunk, Chunk* cd, Item currentBlockType, ivec3 checkPos) {
    if (currentBlockType.isFlat() || currentBlockType == OAK_LEAVES) return 1;
    if (!cd->inBounds(checkPos)) {
        if (checkPos.y == -1) return 1;
        else if (checkPos.y >= 0 && checkPos.y < CHUNK_HEIGHT) {
            uint32_t pos = pack(ivec2(floorDiv(checkPos.x, CHUNK_SIZE), floorDiv(checkPos.z, CHUNK_SIZE)));
            if (!world.chunkData.count(pos)) return 0;
            else {
                BlockData& bData = (*world.chunkData[pos])[checkPos];
                return (isAir(bData.blockType)) || bData.blockType.isFlat();
            }
        }
        else return 0;
    }
    Item& blockType = (*cd)[checkPos].blockType;
    if (blockType == OAK_LEAVES) return 1;
    return isAir(blockType) ||
        blockType.isFlat() ||
        currentBlockType == TORCH;
}

bool buildMask(Chunk* ch, int planeDirVal, int faceDir, vec2& xyChunk, int W, int H, vector<uint8_t>& mask, ivec3 normal) {
    mask.assign(W * H, 0);
    int dir = faceDir / 2; bool allAir = true;
    int minX = xyChunk.x * CHUNK_SIZE, minZ = xyChunk.y * CHUNK_SIZE;
        //(faceDir == 0) ? ivec3(-1, 0, 0) : (faceDir == 1) ? ivec3( 1, 0, 0) : (faceDir == 2) ? ivec3( 0, 0,-1) : (faceDir == 3) ? ivec3( 0, 0, 1) : (faceDir == 4) ? ivec3(0,-1, 0) : ivec3( 0, 1, 0);
    for (int l = 0; l < H; l++) {
        for (int b = 0; b < W; b++) {
            int idx = b + l * W;
            uint8_t& item = mask[idx];
            ivec3 pos = (dir == 0) ? ivec3(planeDirVal, l, minZ + b) : (dir == 1) ? ivec3(minX + b, l, planeDirVal) : ivec3(minX + b, planeDirVal, minZ + l);
            Item blockType = AIR;
            if (ch->inBounds(pos)) {
                blockType = (*ch)[pos].blockType;
                if (isAir(blockType)) { continue; }
            }        
            if (shouldEmitFace(xyChunk, ch, blockType, ivec3(pos.x + normal.x, pos.y + normal.y, pos.z + normal.z))) item = blockType.id;
            if (item != mask[0] || (item != 0)) allAir = 0;
        }
    }
    return allAir;
}

void emitFace(Mesh& m, int face, uint8_t blockType, ivec3 blockPos, ivec3 dims, ivec3 normals) {
    float tintr = 1.0f, tintg = 1.0f, tintb = 1.0f;
    if (blockType == GRASS_BLOCK.id && face != 4) {
        tintr = 0.2f, tintg = 1.0f, tintb = 0.2f;
    }
    else if (blockType == GRASS.id) {
        tintr = 0.2f, tintg = 1.25f, tintb = 0.15f;
    }
    else if (blockType == OAK_LEAVES.id) {
        tintr = 0.2f, tintg = 1.0f, tintb = 0.2f;
    }
    //vec3 rgb = { tintr, tintg, tintb };
    vec3 cMask = { tintr, tintg, tintb };

    glm::vec3 n = normals;
    auto absl = [](int n) { return n >= 0 ? n : -n; };

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

    uint32_t uintUVs = ((uint8_t)(blockPos.y) << 24) | ((uint8_t)(xdimens) << 16) | ((uint8_t)(yoffset + offsetY) << 8) | (uint8_t)(xoffset + offsetX); // Packaging floats into one integer
    float startUvs;
    memcpy(&startUvs, &uintUVs, sizeof(float));
    uint32_t norm_color = ((byte(n.x < 0 ? 1 : 0) & 0x1) << 5) | ((byte(absl(n.x)) & 0x1) << 4) 
                        | ((byte(n.y < 0 ? 1 : 0) & 0x1) << 3) | ((byte(absl(n.y)) & 0x1) << 2) 
                        | ((byte(n.z < 0 ? 1 : 0) & 0x1) << 1) | ((byte(absl(n.z)) & 0x1) << 0)
                        | ((byte(cMask.x * 100) & 0x7F) << 20) 
                        | ((byte(cMask.y * 100) & 0x7F) << 13) 
                        | ((byte(cMask.z * 100) & 0x7F) << 6 );
    float normcolor;
    memcpy(&normcolor, &norm_color, sizeof(float));

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

    uint32_t base = (uint32_t)(m.vertices.size() / 7);
    for (int i = 0; i < 4; i++) {
        m.vertices.push_back(v[i].x);
        m.vertices.push_back(v[i].y);
        m.vertices.push_back(v[i].z);
        m.vertices.push_back(uv[i].x);
        m.vertices.push_back(uv[i].y);
        m.vertices.push_back(uv[i].z);
        m.vertices.push_back(normcolor);
    }
    // Two triangles (0,1,2) (2,3,0)
    m.indices.push_back(base + 1); m.indices.push_back(base + 2); m.indices.push_back(base + 3);
    m.indices.push_back(base + 3); m.indices.push_back(base + 0); m.indices.push_back(base + 1);
}

void greedyMerge(vector<uint8_t>& mask, Mesh& m, vec2& xyChunk, int planeDirVal, int W, int H, int faceDir, ivec3 normals) {
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
            emitFace(m, faceDir, type, blockPos, dims, normals);

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
    //uint8_t terrainHeight = ch->terrainHeight;

    for (int x = (xyChunk.x) * CHUNK_SIZE - 1; x < (xyChunk.x + 1) * CHUNK_SIZE; ++x) {
        buildMask(ch, x + 0, 0, xyChunk, CHUNK_SIZE, CHUNK_HEIGHT, mask, ivec3(-1, 0, 0));
        greedyMerge(mask, m, xyChunk, x + 0, CHUNK_SIZE, CHUNK_HEIGHT, 0, ivec3(1, 0, 0));
        

        buildMask(ch, x + 1, 1, xyChunk, CHUNK_SIZE, CHUNK_HEIGHT, mask, ivec3( 1, 0, 0));
        greedyMerge(mask, m, xyChunk, x + 1, CHUNK_SIZE, CHUNK_HEIGHT, 1,ivec3(-1, 0, 0));
    }

    for (int z = (xyChunk.y) * CHUNK_SIZE - 1; z < (xyChunk.y + 1) * CHUNK_SIZE; ++z) {
        buildMask(ch, z + 0, 2, xyChunk, CHUNK_SIZE, CHUNK_HEIGHT, mask, ivec3( 0, 0,-1));
        greedyMerge(mask, m, xyChunk, z + 0, CHUNK_SIZE, CHUNK_HEIGHT, 2, ivec3(0, 0, 1));

        buildMask(ch, z + 1, 3, xyChunk, CHUNK_SIZE, CHUNK_HEIGHT, mask, ivec3( 0, 0, 1));
        greedyMerge(mask, m, xyChunk, z + 1, CHUNK_SIZE, CHUNK_HEIGHT, 3,ivec3(0, 0, -1));
    }

    for (int y = 0-1; y < CHUNK_HEIGHT; ++y) { //CHUNK_SIZE*(CHUNK_SIZE + 2) + 5
        if (!buildMask(ch, y + 0, 4, xyChunk, CHUNK_SIZE, CHUNK_SIZE, mask, ivec3(0,-1, 0)));
            greedyMerge(mask, m, xyChunk, y + 0, CHUNK_SIZE, CHUNK_SIZE, 4, ivec3(0, 1, 0));

        if (!buildMask(ch, y + 1, 5, xyChunk, CHUNK_SIZE, CHUNK_SIZE, mask, ivec3(0, 1, 0)));
            greedyMerge(mask, m, xyChunk, y + 1, CHUNK_SIZE, CHUNK_SIZE, 5, ivec3(0,-1, 0));
    }
}

void generateChunkAt(vec2 xyChunk, Chunk* repChunk) {
    //auto start = std::chrono::high_resolution_clock::now();

    generateBlocks(xyChunk, repChunk);
    //meshChunk(xyChunk, repChunk, repChunk->mesh);

    //auto end = std::chrono::high_resolution_clock::now();
    //cout << repChunk->mesh->vertices.size() << "count : " << chunkCount << " : Elapsed: " << std::chrono::duration<double>(end - start).count() << " s\n";
}

std::thread blockBreakThread([&]() {
    while (blockBreaking) {
            if (blockBreakingOut) {
            std::lock_guard<std::mutex> lock(breakReqMutex);
            Block block = world.delBlocklook_at();
            //this_thread::sleep_for(chrono::milliseconds(100));
            ivec2 chunkPos = ivec2(floorDiv(block.position.x, CHUNK_SIZE), floorDiv(block.position.z, CHUNK_SIZE));
            world.updateChunk(chunkPos, vec3(0), vec3(0));
            blockBreakingOut = false;
            breakResQueue.push(block);
        }
    }
    });

std::thread blockPlaceThread([&]() {
    while (blockPlacing) {
        if (blockPlacingOut) {
            std::lock_guard<std::mutex> lock(placeReqMutex);
            Block block = world.addBlocklook_at(inventory.mainInventorySlots[3][slot].item);
            //this_thread::sleep_for(chrono::milliseconds(100));
            blockBreakingOut = false;
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
            std::lock_guard<std::mutex> lock(resultMutex);
            newChunk->toCoords(coord);
            generateChunkAt(coord, newChunk.get());
            world.addChunk(newChunk, newChunk->coord);
            //chunkResultQueue.push(move(newChunk));
        }
    }
}////////////

void updateChunkJob() {
    while (!stopChunkUpdaters) {
        vec2 chPos;
        {
            unique_lock<mutex> lock(chunkUpdateRequestMutex);
            chunkUpdateCV.wait(lock, [] { return !chunkUpdateRequestQueue.empty() || stopChunkUpdaters; });
            chPos = chunkUpdateRequestQueue.front();
            chunkUpdateRequestQueue.pop();
        }

        unique_ptr<Chunk>& ch = world.chunkData.at(pack(chPos));
        if (!ch) continue;

        world.updateChunk(chPos, vec3(0), vec3(0));
    }
}/////////////

//std::thread chunkUpdateThread([&]() {
//    while (chunkUpdateGenRunning) {
//        {
//            if (!chunkUpdateRequestQueue.empty()) {
//                std::lock_guard<std::mutex> lock(chunkUpdateRequestMutex);
//                vec2 chunkToUpdate = (chunkUpdateRequestQueue.front());
//                world.updateChunk(chunkToUpdate, vec3(0), vec3(0));
//                chunkUpdateRequestQueue.pop();
//            }
//        }
//    }
//});