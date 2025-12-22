#pragma once

#include "Block.h"

const int CHUNK_SIZE = 16;
const int CHUNK_HEIGHT = CHUNK_SIZE * CHUNK_SIZE + 4 * CHUNK_SIZE + 4;
const int CHUNK_VOLUME = (CHUNK_SIZE + 2) * CHUNK_HEIGHT * (CHUNK_SIZE + 2);

//int at(vec3 position) {
//    return position.x * CHUNK_SIZE + (position.y + 1) * CHUNK_HEIGHT + position.z;
//}

struct ivec2_hash {
    size_t operator()(const glm::ivec2& v) const noexcept {
        // widen to unsigned to avoid UB on shifts of negatives
        uint64_t x = static_cast<uint32_t>(v.x);
        uint64_t y = static_cast<uint32_t>(v.y);
        uint64_t h = x * 0x9E3779B185EBCA87ull ^ (y + 0x9E3779B185EBCA87ull + (x << 6) + (x >> 2));
        // size_t may be 32-bit; fold if needed
        if constexpr (sizeof(size_t) == 4) {
            h ^= (h >> 32);
        }
        return static_cast<size_t>(h);
    }
};

struct ivec2_eq {
    bool operator()(glm::ivec2 const& a, glm::ivec2 const& b) const noexcept {
        return a.x == b.x && a.y == b.y;
    }
};

uint64_t pack(ivec2 xz) {
    uint64_t x = uint32_t(xz.x), z = uint32_t(xz.y);
    return (x << 32) | z;
}

//struct ivec3_hash {
//    size_t operator()(glm::ivec3 const& v) const noexcept {
//        uint64_t ux = static_cast<uint32_t>(v.x);
//        uint64_t uy = static_cast<uint32_t>(v.y);
//        uint64_t uz = static_cast<uint32_t>(v.z);
//        uint64_t h = ux;
//        h ^= (uy + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2));
//        h ^= (uz + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2));
//        return static_cast<size_t>(h);
//    }
//};
//
//struct ivec3_eq {
//    bool operator()(glm::ivec3 const& a, glm::ivec3 const& b) const noexcept {
//        return a.x == b.x && a.y == b.y && a.z == b.z;
//    }
//};

struct BlockData {
    Item blockType = AIR;
    ivec3 position;
    ivec3 orientation = ivec3(0, 1, 0);
    int state = 0;
    BlockData(){}
    BlockData(ivec3 pos, Item type) {
        blockType = type;
        position = pos;
    }
    BlockData(const BlockData& data) {
        blockType = data.blockType;
        position = data.position;
    }
};
// use it ? nope :) vectors allocate inside the heap (not the stack like some other :) which makes it better the using array or well a c array or a C++11 array<T, N> hope I got that
// Maps are horrible and a despicable wastage of memory
//std::unordered_map<glm::ivec3, Block, ivec3_hash, ivec3_eq> worldBlocks;
//std::unordered_map<glm::ivec3, blockData, ivec3_hash, ivec3_eq> worldBlocks;

class Chunk {
public:
    Mesh mesh;
    bool needUpdate = true;
    bool unloaded = false;
    vec2 coords;
    vector<BlockData> block_data;
    //BlockData* block_data = new BlockData[CHUNK_VOLUME];
    int at(vec3 position) {
        return ((position.y + 1) * (CHUNK_HEIGHT) + 
                (position.x - coords.x * (CHUNK_SIZE)) * (CHUNK_SIZE + 2) + 
                (position.z - coords.y * (CHUNK_SIZE)));
    }

    Chunk(){
        block_data.resize(CHUNK_VOLUME);
    }
    Chunk(const Chunk& chunk) {
        mesh = chunk.mesh;
        coords = chunk.coords;
        unloaded = chunk.unloaded;
        block_data = chunk.block_data;
    }
    void operator=(Chunk chunk) {
        mesh = chunk.mesh;
        coords = chunk.coords;
        unloaded = chunk.unloaded;
        block_data = chunk.block_data;
    }
};