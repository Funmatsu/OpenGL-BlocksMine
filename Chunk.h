#pragma once

#include "Block.h"

typedef uint8_t byte;

const int CHUNK_SIZE = 16;
const int CHUNK_HEIGHT = pow(CHUNK_SIZE, 2);  //(CHUNK_SIZE + 2) * (CHUNK_SIZE + 2);
const int CHUNK_VOLUME = pow(CHUNK_SIZE, 4);  //(CHUNK_SIZE + 2) * CHUNK_HEIGHT * (CHUNK_SIZE + 2); // Or just CHUNK_HEIGHT * CHUNK_HEIGHT

//int at(vec3 position) {
//    return position.x * CHUNK_SIZE + (position.y + 1) * CHUNK_HEIGHT + position.z;
//}

//struct ivec2_hash {
//    size_t operator()(const glm::ivec2& v) const noexcept {
//        // widen to unsigned to avoid UB on shifts of negatives
//        uint64_t x = static_cast<uint32_t>(v.x);
//        uint64_t y = static_cast<uint32_t>(v.y);
//        uint64_t h = x * 0x9E3779B185EBCA87ull ^ (y + 0x9E3779B185EBCA87ull + (x << 6) + (x >> 2));
//        // size_t may be 32-bit; fold if needed
//        if constexpr (sizeof(size_t) == 4) {
//            h ^= (h >> 32);
//        }
//        return static_cast<size_t>(h);
//    }
//};
//
//struct ivec2_eq {
//    bool operator()(glm::ivec2 const& a, glm::ivec2 const& b) const noexcept {
//        return a.x == b.x && a.y == b.y;
//    }
//};

uint32_t pack(ivec2 xz) {
    uint32_t x = uint16_t(xz.x), z = uint16_t(xz.y);
    return (x << 16) | z;
}

//uint64_t pack(ivec2 xz) {
//    uint64_t x = uint32_t(xz.x), z = uint32_t(xz.y);
//    return (x << 32) | z;
//}

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
    uint8_t orientation = (1 << 2); // 00 01 00 = 0,1,0  00 11 00 = 0,-1,0
    BlockData() { blockType = AIR; }
    BlockData(Item type) {
        blockType = type;
    }
    BlockData(const BlockData& data) {
        blockType = data.blockType;
    }
};

// use it ? nope :) vectors allocate inside the heap (not the stack like some other :) which makes it better the using array or well a c array or a C++11 array<T, N> hope I got that
// Maps are horrible and a despicable wastage of memory
//std::unordered_map<glm::ivec3, Block, ivec3_hash, ivec3_eq> worldBlocks;
//std::unordered_map<glm::ivec3, blockData, ivec3_hash, ivec3_eq> worldBlocks;

class Chunk {
public:
    Mesh mesh;                                                   
    vector<BlockData> block_data;
    uint32_t coord;
    byte terrainHeight = 0;
    bool needUpdate, unloaded;
    byte neighboursPresent;

    vec2 coords() {
        return vec2(int16_t((coord >> 16) & 0xFFFF), int16_t(coord & 0xFFFF));
    }
    void toCoords(ivec2 chunkCoord) {
        coord = ((uint16_t(chunkCoord.x) & 0xFFFF) << 16) | (uint16_t(chunkCoord.y) & 0xFFFF);
    }
    int at(vec3 position) {
        ivec2 thisCoord = coords();
        return ((position.y) * (CHUNK_HEIGHT) + 
                (position.x - thisCoord.x * (CHUNK_SIZE)) * (CHUNK_SIZE) + 
                (position.z - thisCoord.y * (CHUNK_SIZE)));
    }

    bool inBounds(vec3 position) {
        ivec2 thisCoord = coords();
        return position.x >= (thisCoord.x) * (CHUNK_SIZE) && position.x < (thisCoord.x + 1) * (CHUNK_SIZE) &&
               position.y >= 0 && position.y < CHUNK_HEIGHT &&
               position.z >= (thisCoord.y) * (CHUNK_SIZE) && position.z < (thisCoord.y + 1) * (CHUNK_SIZE);
    }

    Chunk(){
        block_data.resize(CHUNK_VOLUME);
        needUpdate = true; unloaded = false;
    }
    Chunk(const Chunk& chunk) {
        mesh = chunk.mesh;
        coord = chunk.coord;
        needUpdate = chunk.needUpdate;
        block_data = chunk.block_data;
    }
    void operator=(Chunk chunk) {
        mesh = chunk.mesh;
        coord = chunk.coord;
        needUpdate = chunk.needUpdate;
        block_data = chunk.block_data;
    }
    BlockData& operator[](ivec3 pos) {
        int index = at(pos);
        return block_data[index];
    }
};

uint16_t chunkCount = 0;

//#pragma once
//
//#include "Block.h"
//
//const int CHUNK_SIZE = 16;
//const int CHUNK_HEIGHT = pow(CHUNK_SIZE, 2);  //(CHUNK_SIZE + 2) * (CHUNK_SIZE + 2);
//const int CHUNK_VOLUME = pow(CHUNK_SIZE, 4);  //(CHUNK_SIZE + 2) * CHUNK_HEIGHT * (CHUNK_SIZE + 2); // Or just CHUNK_HEIGHT * CHUNK_HEIGHT
//
////int at(vec3 position) {
////    return position.x * CHUNK_SIZE + (position.y + 1) * CHUNK_HEIGHT + position.z;
////}
//
//struct ivec2_hash {
//    size_t operator()(const glm::ivec2& v) const noexcept {
//        // widen to unsigned to avoid UB on shifts of negatives
//        uint64_t x = static_cast<uint32_t>(v.x);
//        uint64_t y = static_cast<uint32_t>(v.y);
//        uint64_t h = x * 0x9E3779B185EBCA87ull ^ (y + 0x9E3779B185EBCA87ull + (x << 6) + (x >> 2));
//        // size_t may be 32-bit; fold if needed
//        if constexpr (sizeof(size_t) == 4) {
//            h ^= (h >> 32);
//        }
//        return static_cast<size_t>(h);
//    }
//};
//
//struct ivec2_eq {
//    bool operator()(glm::ivec2 const& a, glm::ivec2 const& b) const noexcept {
//        return a.x == b.x && a.y == b.y;
//    }
//};
//
//uint64_t pack(ivec2 xz) {
//    uint64_t x = uint32_t(xz.x), z = uint32_t(xz.y);
//    return (x << 32) | z;
//}
//
////struct ivec3_hash {
////    size_t operator()(glm::ivec3 const& v) const noexcept {
////        uint64_t ux = static_cast<uint32_t>(v.x);
////        uint64_t uy = static_cast<uint32_t>(v.y);
////        uint64_t uz = static_cast<uint32_t>(v.z);
////        uint64_t h = ux;
////        h ^= (uy + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2));
////        h ^= (uz + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2));
////        return static_cast<size_t>(h);
////    }
////};
////
////struct ivec3_eq {
////    bool operator()(glm::ivec3 const& a, glm::ivec3 const& b) const noexcept {
////        return a.x == b.x && a.y == b.y && a.z == b.z;
////    }
////};
//
//struct BlockData {
//    Item blockType = AIR;
//    uint8_t orientation = (1 << 2); // 00 01 00 = 0,1,0  00 11 00 = 0,-1,0
//    BlockData() {}
//    BlockData(ivec3 pos, Item type) {
//        blockType = type;
//    }
//    BlockData(const BlockData& data) {
//        blockType = data.blockType;
//    }
//};
//
//// use it ? nope :) vectors allocate inside the heap (not the stack like some other :) which makes it better the using array or well a c array or a C++11 array<T, N> hope I got that
//// Maps are horrible and a despicable wastage of memory
////std::unordered_map<glm::ivec3, Block, ivec3_hash, ivec3_eq> worldBlocks;
////std::unordered_map<glm::ivec3, blockData, ivec3_hash, ivec3_eq> worldBlocks;
//
//class Chunk {
//public:
//    Mesh mesh;
//    vec2 coords;
//    vector<BlockData> block_data;
//    uint8_t updateFlags; // << 1 : needUpdate, << 0 : unloaded
//    uint8_t terrainHeight = 0;
//
//    int at(vec3 position) {
//        return ((position.y) * (CHUNK_HEIGHT)+
//            (position.x - coords.x * (CHUNK_SIZE)) * (CHUNK_SIZE)+
//            (position.z - coords.y * (CHUNK_SIZE)));
//    }
//
//    bool inBounds(vec3 position) {
//        return position.x >= (coords.x) * (CHUNK_SIZE) && position.x < (coords.x + 1) * (CHUNK_SIZE) &&
//            position.y >= 0 && position.y < CHUNK_HEIGHT &&
//            position.z >= (coords.y) * (CHUNK_SIZE) && position.z < (coords.y + 1) * (CHUNK_SIZE);
//    }
//
//    Chunk() {
//        block_data.resize(CHUNK_VOLUME);
//        updateFlags = (1 << 1);
//    }
//    Chunk(const Chunk& chunk) {
//        mesh = chunk.mesh;
//        coords = chunk.coords;
//        updateFlags = chunk.updateFlags;
//        block_data = chunk.block_data;
//    }
//    void operator=(Chunk chunk) {
//        mesh = chunk.mesh;
//        coords = chunk.coords;
//        updateFlags = chunk.updateFlags;
//        block_data = chunk.block_data;
//    }
//
//    bool unloaded() {
//        return bool(updateFlags & 1);
//    }
//
//    bool needUpdate() {
//        return ((updateFlags >> 1) & 1);
//    }
//    void needUpdate(uint8_t update) {
//        updateFlags |= (update << 1) & 2;
//    }
//    void unloaded(uint8_t update) {
//        updateFlags |= (update & 1);
//    }
//    BlockData& operator[](ivec3 pos) {
//        return block_data[at(pos)];
//    }
//};
//
//uint16_t chunkCount = 0;