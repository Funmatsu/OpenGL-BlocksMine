#pragma once
#include "libraries.h"
#include "Chunk.h"
#include "normals.h"
#include "inventory.h"

//vector<vec2> chunkCoords;
std::unordered_set<ivec2, ivec2_hash> chunkCoords;

std::queue<vec2> chunkRequestQueue;
std::mutex queueMutex;
std::condition_variable queueCV;

std::queue<Chunk> chunkResultQueue;
std::mutex resultMutex;

//std::queue<vec2> chunkRequestQueue;
std::mutex chunkRequestMutex;

//std::queue<Chunk> chunkResultQueue;
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


void generateBlocks(vec2 xyChunk, Chunk& repChunk) {
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

    // Base hills: smooth FBm Perlin
    FastNoiseLite baseNoise;
    baseNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    baseNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    baseNoise.SetFrequency(0.005f);      // low frequency = broad features
    baseNoise.SetFractalOctaves(3);
    baseNoise.SetFractalLacunarity(2.0f);
    baseNoise.SetFractalGain(0.5f);

    // Ridged mountains: sharp features
    FastNoiseLite ridgedNoise;
    ridgedNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    ridgedNoise.SetFractalType(FastNoiseLite::FractalType_Ridged);
    ridgedNoise.SetFrequency(0.01f);     // higher frequency = more detail
    ridgedNoise.SetFractalOctaves(4);
    ridgedNoise.SetFractalLacunarity(2.0f);
    ridgedNoise.SetFractalGain(0.5f);

    // Mask: controls where mountains vs plains appear
    FastNoiseLite maskNoise;
    maskNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    maskNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    maskNoise.SetFrequency(0.002f);      // very low frequency = large biome regions
    maskNoise.SetFractalOctaves(2);
    maskNoise.SetFractalGain(0.5f);

    //FastNoiseLite noise;   
    //noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    //noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    //noise.SetFractalOctaves(6);
    //noise.SetFractalLacunarity(2.0f);
    //noise.SetFractalGain(0.5f);

    FastNoiseLite caveNoise;
    caveNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

    caveNoise.SetFrequency(0.05f);
    FastNoiseLite cloudNoise;
    cloudNoise.SetFrequency(0.045);    

    float cloudDensity = 0;
    float density = 0;

    for (int x = (xyChunk.x) * CHUNK_SIZE - 1; x < (xyChunk.x + 1) * CHUNK_SIZE + 1; x++) {
        for (int z = (xyChunk.y) * CHUNK_SIZE - 1; z < (xyChunk.y + 1) * CHUNK_SIZE + 1; z++) {
            if (x != (xyChunk.x) * CHUNK_SIZE - 1 && x != (xyChunk.x + 1) * CHUNK_SIZE + 1
                && z != (xyChunk.y) * CHUNK_SIZE - 1 && z != (xyChunk.y + 1) * CHUNK_SIZE + 1)
            repChunk.blockData[ivec3(x, -1, z)] = blockData(ivec3(x, -1, z), BEDROCK);
            auto norm = [](float n) { return n * 0.5f + 0.5f; };
            float base = norm(baseNoise.GetNoise((float)x, (float)z)) * 10.0f;
            float ridged = norm(ridgedNoise.GetNoise((float)x, (float)z)) * 30.0f;
            float mask = norm(maskNoise.GetNoise((float)x, (float)z));

            // Blend between base and ridged using the mask
            float blend = smoothstep(0.3f, 0.7f, mask);
            float height = mix(base, ridged, blend);

            //float scaledHeight = height;
            float scaledHeight = height * 2 + 40;
            /*float scaledHeight = ((height + 1.0f) * (CHUNK_SIZE * CHUNK_SIZE));*/
            float treeHeight = randomFloat(3.0, 5.0) + 2, treeDistrib = rand();
            
            for (int y = 0; y < scaledHeight; y++) {
                if (x == (xyChunk.x) * CHUNK_SIZE - 1 || x == (xyChunk.x + 1) * CHUNK_SIZE + 1
                    || z == (xyChunk.y) * CHUNK_SIZE - 1 || z == (xyChunk.y + 1) * CHUNK_SIZE + 1) {
                    repChunk.blockData.insert({ ivec3(x, y, z), blockData(ivec3(x, y, z), AIR) });
                    continue;
                }
                if (y >= scaledHeight - 1) {
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

                cloudDensity = cloudNoise.GetNoise((float)x, (float)y, (float)z);
                density = caveNoise.GetNoise((float)x, (float)y, (float)z);

                if (density < -0.4f) {
                    continue;
                }
                repChunk.blockData.insert({ ivec3(x, y, z), blockData(ivec3(x, y, z), blockType) });
                
                //"Decorations"

                if (cloudDensity < -0.85f) {
                    repChunk.blockData[ivec3(x, CHUNK_SIZE * CHUNK_SIZE + 5 * CHUNK_SIZE - 1, z)] = blockData(ivec3(x, CHUNK_SIZE * CHUNK_SIZE + 5 * CHUNK_SIZE - 1, z), CLOUD);
                }

                if (blockType == GRASS_BLOCK) {
                    // Making the trees and leaves
                    if (x >= (xyChunk.x + 1) * CHUNK_SIZE || x <= (xyChunk.x) * CHUNK_SIZE ||
                        z >= (xyChunk.y + 1) * CHUNK_SIZE || z <= (xyChunk.y) * CHUNK_SIZE) { goto notree; }
                    if (treeDistrib > 0.0 && treeDistrib <= 150) {

                        for (int i = y + 1; i < y + treeHeight; i++) {
                            repChunk.blockData[ivec3(x, i, z)] = blockData(ivec3(x, i, z), OAK_WOOD);
                        }
                        glm::ivec3 center(x, y + treeHeight + 1, z);

                        int radius = 3; // adjust for size

                        for (int dx = -radius; dx <= radius; ++dx) {
                            for (int dy = -radius; dy <= radius; ++dy) {
                                for (int dz = -radius; dz <= radius; ++dz) {
                                    glm::ivec3 pos = center + glm::ivec3(dx, dy, dz);
                                    float dist = glm::length(glm::vec3(dx, dy, dz));

                                    if (dist <= radius + randomFloat(-0.3f, 0.3f)) {
                                        if (repChunk.blockData.find(pos) == repChunk.blockData.end())
                                        repChunk.blockData[ivec3(pos.x, pos.y, pos.z)] = blockData(ivec3(pos.x, pos.y, pos.z), OAK_LEAVES);
                                    }
                                }
                            }
                        }
                    }                    
                }
                notree: // No trees should be created if they are near the chunks border
                float randomNumberForGrass = rand(), randomNumberForPoppy = rand(), randomNumberForOrchid = rand();
                if (blockType == GRASS_BLOCK && blockType != OAK_WOOD) {
                    if (randomNumberForGrass < 2000.0) {
                        repChunk.blockData.insert(make_pair(ivec3(x, scaledHeight + 1, z), blockData(ivec3(x, scaledHeight + 1, z), GRASS)));
                        break;
                    }

                    if (randomNumberForPoppy < 500.0) {
                        repChunk.blockData.insert(make_pair(ivec3(x, scaledHeight + 1, z), blockData(ivec3(x, scaledHeight + 1, z), POPPY)));
                        break;
                    }

                    if (randomNumberForOrchid < 200.0) {
                        repChunk.blockData.insert(make_pair(ivec3(x, scaledHeight + 1, z), blockData(ivec3(x, scaledHeight + 1, z), BLUE_ORCHID)));
                        break;
                    }
                }
            }
        }
    }
}

bool isAir(Item item) { return item == AIR; }

bool shouldEmitFace(vec2 xyChunk, Chunk& cd, int x, int y, int z, int dx, int dy, int dz) {
    //return 1;
    int nx = x + dx, ny = y + dy, nz = z + dz;
    ivec3 checkPos = ivec3(nx, ny, nz), blockPos = ivec3(x, y, z);
    //if (cd.blockData[checkPos].blockType == OAK_WOOD) return true;
    if (((nx <= xyChunk.x * CHUNK_SIZE - 1 || nz <= xyChunk.y * CHUNK_SIZE - 1)              ||
          nx >= (xyChunk.x + 1) * CHUNK_SIZE + 1 || nz >= (xyChunk.y + 1) * CHUNK_SIZE + 1) && cd.blockData[checkPos].blockType != OAK_LEAVES) return false;
    //if (nx < xyChunk.x * CHUNK_SIZE || ny < -1 || nz < xyChunk.y * CHUNK_SIZE || nx >= (xyChunk.x + 1) * CHUNK_SIZE || ny >= CHUNK_SIZE * CHUNK_SIZE + 10 * CHUNK_SIZE || nz >= (xyChunk.y + 1) * CHUNK_SIZE) return true;
    return isAir(cd.blockData[checkPos].blockType)          ||
           cd.blockData[checkPos].blockType.isFlat          ||
           cd.blockData[checkPos].blockType == OAK_LEAVES   ||
           cd.blockData[ivec3(x, y, z)].blockType == TORCH  ;
}

void emitFace(Mesh& m, int baseX, int baseY, int baseZ,
    int face, Item blockType, float x, float y, float z) {
    // face: 0=-X,1=+X,2=-Z,3=+Z,4=+Z,5=-Z
    // Define 4 positions and normal per face
    static const glm::vec3 normals[6] = {
        {1, 0, 0}, {-1, 0, 0}, {0, 0, 1}, {0, 0, -1}, {0, 1, 0}, { 0, -1, 0 }
    };
    glm::vec3 n = normals[face];

    glm::vec3 v[4];
    float torchdiff = -0.045;
    if (blockType == TORCH) {
        switch (face) {
            case 0: v[0] = { x + 0.4 - torchdiff,y + 0,z + 0 }; v[1] = { x + 0.4 - torchdiff,y + 1,z + 0 }; v[2] = { x + 0.4 - torchdiff,y + 1,z + 1 }; v[3] = { x + 0.4 - torchdiff,y + 0,z + 1 }; break; // -X
            case 1: v[0] = { x + 0.615 + torchdiff,y + 0,z + 0 }; v[1] = { x + 0.615 + torchdiff,y + 0,z + 1 }; v[2] = { x + 0.615 + torchdiff,y + 1,z + 1 }; v[3] = { x + 0.615 + torchdiff,y + 1,z + 0 }; break; // +X
            case 2: v[0] = { x + 0,y + 0,z + 0.4 - torchdiff }; v[1] = { x + 1,y + 0,z + 0.4 - torchdiff }; v[2] = { x + 1,y + 1,z + 0.4 - torchdiff }; v[3] = { x + 0,y + 1,z + 0.4 - torchdiff }; break; // -Z
            case 3: v[0] = { x + 0,y + 0,z + 0.615 + torchdiff }; v[1] = { x + 0,y + 1,z + 0.615 + torchdiff }; v[2] = { x + 1,y + 1,z + 0.615 + torchdiff }; v[3] = { x + 1,y + 0,z + 0.615 + torchdiff }; break; // +Z
            case 4: v[0] = { x + 0,y + 0,z + 1 }; v[1] = { x + 1,y + 0,z + 0 }; v[2] = { x + 1,y + 0,z + 1 }; v[3] = { x + 0,y + 0,z + 1 }; break; // -Y
            case 5: v[0] = { x + 0,y + 1,z + 0 }; v[1] = { x + 1,y + 1,z + 0 }; v[2] = { x + 1,y + 1,z + 1 }; v[3] = { x + 0,y + 1,z + 1 }; break; // +Y
        }
    }
    else if (!blockType.isFlat){
        switch (face) {
            case 0: v[0] = { x + 0,y + 0,z + 0 }; v[1] = { x + 0,y + 1,z + 0 }; v[2] = { x + 0,y + 1,z + 1 }; v[3] = { x + 0,y + 0,z + 1 }; break; // -X
            case 1: v[0] = { x + 1,y + 0,z + 0 }; v[1] = { x + 1,y + 0,z + 1 }; v[2] = { x + 1,y + 1,z + 1 }; v[3] = { x + 1,y + 1,z + 0 }; break; // +X
            case 2: v[0] = { x + 0,y + 0,z + 0 }; v[1] = { x + 1,y + 0,z + 0 }; v[2] = { x + 1,y + 1,z + 0 }; v[3] = { x + 0,y + 1,z + 0 }; break; // -Z
            case 3: v[0] = { x + 0,y + 0,z + 1 }; v[1] = { x + 0,y + 1,z + 1 }; v[2] = { x + 1,y + 1,z + 1 }; v[3] = { x + 1,y + 0,z + 1 }; break; // +Z
            case 4: v[0] = { x + 0,y + 0,z + 0 }; v[1] = { x + 0,y + 0,z + 1 }; v[2] = { x + 1,y + 0,z + 1 }; v[3] = { x + 1,y + 0,z + 0 }; break; // -Y
            case 5: v[0] = { x + 0,y + 1,z + 0 }; v[1] = { x + 1,y + 1,z + 0 }; v[2] = { x + 1,y + 1,z + 1 }; v[3] = { x + 0,y + 1,z + 1 }; break; // +Y
        }
    }
    else {
        switch (face) {
            case 0: v[0] = { x + 0,y + 0,z + 0 }; v[1] = { x + 0,y + 1,z + 0 }; v[2] = { x + 1,y + 1,z + 1 }; v[3] = { x + 1,y + 0,z + 1 }; break; // -X disgonal
            case 1: v[0] = { x + 0,y + 0,z + 1 }; v[1] = { x + 0,y + 1,z + 1 }; v[2] = { x + 1,y + 1,z + 0 }; v[3] = { x + 1,y + 0,z + 0 }; break; // +X diagonal
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

    float clipX = 0.02f, clipY = 0.98f;
    if (blockType == OAK_WOOD && face != 5 && face != 4) {
        clipX = 0.05f, clipY = 0.95f;
    }

    int offsetX = 0, offsetY = 0;       
    if (face == 4) { offsetX = xoffsetBottom; offsetY = yoffsetBottom; }
    else if (face == 5) { offsetX = xoffsetTop;  offsetY = yoffsetTop; }
    switch (face) {
        case 1:  
        case 2:       if (!blockType.isFlat || blockType == TORCH) {
                            uv.push_back(vec3((clipX + xoffset + offsetX) / xdimens, (clipX + yoffset + offsetY) / ydimens, transparency));
                            uv.push_back(vec3((clipY + xoffset + offsetX) / xdimens, (clipX + yoffset + offsetY) / ydimens, transparency));
                            uv.push_back(vec3((clipY + xoffset + offsetX) / xdimens, (clipY + yoffset + offsetY) / ydimens, transparency));
                            uv.push_back(vec3((clipX + xoffset + offsetX) / xdimens, (clipY + yoffset + offsetY) / ydimens, transparency));
                      }
                      break;
        default :     uv.push_back(vec3((clipX + xoffset + offsetX) / xdimens, (clipX + yoffset + offsetY) / ydimens, transparency));
                      uv.push_back(vec3((clipX + xoffset + offsetX) / xdimens, (clipY + yoffset + offsetY) / ydimens, transparency));
                      uv.push_back(vec3((clipY + xoffset + offsetX) / xdimens, (clipY + yoffset + offsetY) / ydimens, transparency));
                      uv.push_back(vec3((clipY + xoffset + offsetX) / xdimens, (clipX + yoffset + offsetY) / ydimens, transparency)); 
                      break;
    }
    uv.push_back(vec3((clipX + xoffset + offsetX) / xdimens, (clipX + yoffset + offsetY) / ydimens, transparency));
    uv.push_back(vec3((clipX + xoffset + offsetX) / xdimens, (clipY + yoffset + offsetY) / ydimens, transparency));
    uv.push_back(vec3((clipY + xoffset + offsetX) / xdimens, (clipY + yoffset + offsetY) / ydimens, transparency));
    uv.push_back(vec3((clipY + xoffset + offsetX) / xdimens, (clipX + yoffset + offsetY) / ydimens, transparency));

    uint32_t base = (uint32_t)(m.verts.size() / 9);
    for (int i = 0; i < 4; i++) {
        m.verts.push_back(v[i].x);
        m.verts.push_back(v[i].y);
        m.verts.push_back(v[i].z);
        m.verts.push_back(uv[i].x);
        m.verts.push_back(uv[i].y);
        m.verts.push_back(uv[i].z);
        m.verts.push_back(n.x);
        m.verts.push_back(n.y);
        m.verts.push_back(n.z);
    }
    // Two triangles (0,1,2) (2,3,0)
    m.inds.push_back(base + 0); m.inds.push_back(base + 1); m.inds.push_back(base + 2);
    m.inds.push_back(base + 2); m.inds.push_back(base + 3); m.inds.push_back(base + 0);
}

void meshChunk(vec2 xyChunk, Chunk& cd, Mesh& out) {
    out.verts.clear(); out.inds.clear();
    out.verts.reserve(CHUNK_SIZE * (CHUNK_SIZE * CHUNK_SIZE) * CHUNK_SIZE * 24 * 8); // heuristic
    out.inds.reserve(CHUNK_SIZE * (CHUNK_SIZE * CHUNK_SIZE) * CHUNK_SIZE * 6 * 6);

    for (int x = (xyChunk.x) * CHUNK_SIZE - 1; x < (xyChunk.x + 1) * CHUNK_SIZE + 1; ++x)
        for (int y = -1; y < (CHUNK_SIZE * CHUNK_SIZE) + 5 * CHUNK_SIZE; ++y)
            for (int z = (xyChunk.y) * CHUNK_SIZE - 1; z < (xyChunk.y + 1) * CHUNK_SIZE + 1; ++z) {
                Item blockType = cd.blockData[ivec3(x, y, z)].blockType;
                if (isAir(blockType)) continue;

                float fx = float(x), fy = float(y), fz = float(z);
                if (!blockType.isFlat || blockType == TORCH)
                {
                    if (shouldEmitFace(xyChunk, cd, x, y, z, -1, 0, 0)) emitFace(out, x, y, z, 0, blockType, fx, fy, fz);
                    if (shouldEmitFace(xyChunk, cd, x, y, z, +1, 0, 0)) emitFace(out, x, y, z, 1, blockType, fx, fy, fz);
                    if (shouldEmitFace(xyChunk, cd, x, y, z, 0, 0, -1)) emitFace(out, x, y, z, 2, blockType, fx, fy, fz);
                    if (shouldEmitFace(xyChunk, cd, x, y, z, 0, 0, +1)) emitFace(out, x, y, z, 3, blockType, fx, fy, fz);
                    if (shouldEmitFace(xyChunk, cd, x, y, z, 0, -1, 0)) emitFace(out, x, y, z, 4, blockType, fx, fy, fz);
                    if (shouldEmitFace(xyChunk, cd, x, y, z, 0, +1, 0)) emitFace(out, x, y, z, 5, blockType, fx, fy, fz);
                }
                else {                    
                    emitFace(out, x, y, z, 0, blockType, fx, fy, fz);
                    emitFace(out, x, y, z, 1, blockType, fx, fy, fz);                    
                }
            }
}

void generateChunkAt(vec2 xyChunk, Chunk* repChunk) {
    //auto start = std::chrono::high_resolution_clock::now();

    generateBlocks(xyChunk, *repChunk);

    Mesh& m = repChunk->mesh;
    meshChunk(xyChunk, *repChunk, m);
    // meshChunk(cd, m);                 // simple visible faces
    // or:
    // greedyMeshChunk(cd, m);           // build masks and merge per orientation 

    repChunk->vertices.insert(repChunk->vertices.end(), m.verts.begin(), m.verts.end());
    uint32_t base = (uint32_t)(repChunk->indexOffset);
    for (auto idx : repChunk->mesh.inds) repChunk->indices.push_back(base + idx);
    repChunk->indexOffset += (uint32_t)(m.verts.size() / 9);

    //auto end = std::chrono::high_resolution_clock::now();
    //std::cout << repChunk.mesh.verts.size() << " : Elapsed: " << std::chrono::duration<double>(end - start).count() << " s\n";
}

//std::thread chunkGenThread([&]() {
//    while (chunkGenRunning) {
//        vec2 coord;
//        {
//            std::lock_guard<std::mutex> lock(chunkRequestMutex);
//            if (!chunkRequestQueue.empty()) {
//                coord = chunkRequestQueue.front();
//                chunkRequestQueue.pop();
//            }
//            else {
//                continue;
//            }
//        }
//
//        Chunk newChunk;
//        {
//            //std::lock_guard<std::mutex> lock(chunkRequestMutex);
//            generateChunkAt(coord, newChunk);
//            newChunk.coords = coord;
//            newChunk.needUpdate = true;
//        }
//
//        //cout << newChunk.blockNum << endl;
//        {
//            std::lock_guard<std::mutex> lock(chunkResultMutex);
//            chunkResultQueue.push(std::move(newChunk));
//        }
//    }
//    });

//std::thread chunkGenThread2([&]() {
//    while (chunkGenRunning2) {
//        vec2 coord;
//        {
//            std::lock_guard<std::mutex> lock(chunkRequestMutex2);
//            if (!chunkRequestQueue2.empty()) {
//                coord = chunkRequestQueue2.front();
//                chunkRequestQueue2.pop();
//            }
//            else {
//                continue;
//            }
//        }
//
//        Chunk newChunk;
//        {
//            std::lock_guard<std::mutex> lock(chunkRequestMutex2);
//            generateChunkAt(coord, newChunk);
//        }
//
//        newChunk.coords = coord;
//        newChunk.needUpdate = true;
//        //cout << newChunk.blockNum << endl;
//        {
//            std::lock_guard<std::mutex> lock(chunkResultMutex2);
//            chunkResultQueue2.push(std::move(newChunk));
//        }
//    }
//    });
//
//std::thread chunkGenThread3([&]() {
//    while (chunkGenRunning3) {
//        vec2 coord;
//        {
//            std::lock_guard<std::mutex> lock(chunkRequestMutex3);
//            if (!chunkRequestQueue3.empty()) {
//                coord = chunkRequestQueue3.front();
//                chunkRequestQueue3.pop();
//            }
//            else {
//                continue;
//            }
//        }
//
//        Chunk newChunk;
//        generateChunkAt(coord, newChunk);
//
//        newChunk.coords = chunkCoords.back();
//        newChunk.needUpdate = true;
//        {
//            std::lock_guard<std::mutex> lock(chunkResultMutex3);
//            chunkResultQueue3.push(std::move(newChunk));
//        }
//    }
//    });

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


void chunkWorker() {
    while (chunkGenRunning) {
        vec2 coord; 
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCV.wait(lock, [] { return !chunkRequestQueue.empty(); });
            coord = chunkRequestQueue.front();
            chunkRequestQueue.pop();
        }

        Chunk* newChunk = new Chunk();
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            generateChunkAt(coord, newChunk);
            newChunk->coords = coord;
            newChunk->needUpdate = true;
        }

        {
            std::lock_guard<std::mutex> lock(resultMutex);
            chunkResultQueue.push(*newChunk);
        }
    }
}