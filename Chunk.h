#pragma once

#include "Block.h"

int CHUNK_SIZE = 5;

struct ivec3_hash {
    size_t operator()(glm::ivec3 const& v) const noexcept {
        uint64_t ux = static_cast<uint32_t>(v.x);
        uint64_t uy = static_cast<uint32_t>(v.y);
        uint64_t uz = static_cast<uint32_t>(v.z);
        uint64_t h = ux;
        h ^= (uy + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2));
        h ^= (uz + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2));
        return static_cast<size_t>(h);
    }
};
struct ivec3_eq {
    bool operator()(glm::ivec3 const& a, glm::ivec3 const& b) const noexcept {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }
};

struct blockData {
    Item blockType = AIR;
    ivec3 position;
    blockData(){}
    blockData(ivec3 pos, Item type) {
        blockType = type;
        position = pos;
    }
    blockData(const blockData& data) {
        blockType = data.blockType;
        position = data.position;
    }
};
// use it
std::mutex worldBlocksMutex;
std::unordered_map<glm::ivec3, Block, ivec3_hash, ivec3_eq> worldBlocks;

class Chunk {
public:
    Mesh mesh;
    vector<float> vertices;
    vector<unsigned int> indices;
    vector<unsigned int> logicalIndices;
    unsigned int blockIds = 0;
    unsigned int indexOffset = 0;
    bool needUpdate = false;
    vec2 coords;
    vector<Block> blocks;
    unordered_map<glm::ivec3, blockData, ivec3_hash, ivec3_eq> blockData;
    //vector<Item> blocks;
    unordered_map<glm::ivec3, Face, ivec3_hash, ivec3_eq> faces;

    void addBlock(Block block) {
        {
            std::lock_guard<std::mutex> lk(worldBlocksMutex);
            blocks.push_back(block);
            glm::ivec3 key = block.position;

            //worldBlocks[key] = block;
            worldBlocks.insert(make_pair(key, block));
            needUpdate = true;
        }
    }

    Chunk(){}
    Chunk(const Chunk& chunk) {
        mesh = chunk.mesh;
        vertices = move(chunk.vertices);
        indices = move(chunk.indices);
        blockIds = chunk.blockIds;
        indexOffset = chunk.indexOffset;
        needUpdate = true;
        coords = chunk.coords;
        blocks = move(chunk.blocks);
        faces = chunk.faces;
    }
};