#pragma once
#include "libraries.h"
#include "Chunk.h"

struct Plane {
    glm::vec3 n;
    float d;
}; // plane: n.x * X + n.y * Y + n.z * Z + d = 0

void extractFrustumPlanes(const glm::mat4& VP, Plane planes[6]) {
    const float m00 = VP[0][0], m01 = VP[0][1], m02 = VP[0][2], m03 = VP[0][3];
    const float m10 = VP[1][0], m11 = VP[1][1], m12 = VP[1][2], m13 = VP[1][3];
    const float m20 = VP[2][0], m21 = VP[2][1], m22 = VP[2][2], m23 = VP[2][3];
    const float m30 = VP[3][0], m31 = VP[3][1], m32 = VP[3][2], m33 = VP[3][3];

    // left
    planes[0].n.x = m03 + m00; planes[0].n.y = m13 + m10; planes[0].n.z = m23 + m20; planes[0].d = m33 + m30;
    // right
    planes[1].n.x = m03 - m00; planes[1].n.y = m13 - m10; planes[1].n.z = m23 - m20; planes[1].d = m33 - m30;
    // bottom
    planes[2].n.x = m03 + m01; planes[2].n.y = m13 + m11; planes[2].n.z = m23 + m21; planes[2].d = m33 + m31;
    // top
    planes[3].n.x = m03 - m01; planes[3].n.y = m13 - m11; planes[3].n.z = m23 - m21; planes[3].d = m33 - m31;
    // near
    planes[4].n.x = m03 + m02; planes[4].n.y = m13 + m12; planes[4].n.z = m23 + m22; planes[4].d = m33 + m32;
    // far
    planes[5].n.x = m03 - m02; planes[5].n.y = m13 - m12; planes[5].n.z = m23 - m22; planes[5].d = m33 - m32;

    for (int i = 0; i < 6; ++i) {
        float len = glm::length(planes[i].n);
        planes[i].n /= len;
        planes[i].d /= len;
    }
}

bool aabbIntersectsFrustum(const glm::vec3& min, const glm::vec3& max, const Plane planes[6]) {
    for (int p = 0; p < 6; ++p) {
        // compute positive vertex (vertex most in direction of plane normal)
        glm::vec3 positive;
        positive.x = (planes[p].n.x >= 0.0f) ? max.x : min.x;
        positive.y = (planes[p].n.y >= 0.0f) ? max.y : min.y;
        positive.z = (planes[p].n.z >= 0.0f) ? max.z : min.z;

        // if positive vertex is behind plane, entire box is behind that plane -> culled
        if (glm::dot(planes[p].n, positive) + planes[p].d < 0.0f) return false;
    }
    return true; // intersects or is fully inside
}

//struct Frustum {
//    glm::vec4 planes[6]; // Each plane: ax + by + cz + d = 0
//};
//
//Frustum extractFrustumPlanes(const glm::mat4& vp) {
//    Frustum f;
//
//    f.planes[0] = glm::vec4(vp[0][3] + vp[0][0], vp[1][3] + vp[1][0], vp[2][3] + vp[2][0], vp[3][3] + vp[3][0]); // Left
//    f.planes[1] = glm::vec4(vp[0][3] - vp[0][0], vp[1][3] - vp[1][0], vp[2][3] - vp[2][0], vp[3][3] - vp[3][0]); // Right
//    f.planes[2] = glm::vec4(vp[0][3] + vp[0][1], vp[1][3] + vp[1][1], vp[2][3] + vp[2][1], vp[3][3] + vp[3][1]); // Bottom
//    f.planes[3] = glm::vec4(vp[0][3] - vp[0][1], vp[1][3] - vp[1][1], vp[2][3] - vp[2][1], vp[3][3] - vp[3][1]); // Top
//    f.planes[4] = glm::vec4(vp[0][3] + vp[0][2], vp[1][3] + vp[1][2], vp[2][3] + vp[2][2], vp[3][3] + vp[3][2]); // Near
//    f.planes[5] = glm::vec4(vp[0][3] - vp[0][2], vp[1][3] - vp[1][2], vp[2][3] - vp[2][2], vp[3][3] - vp[3][2]); // Far
//
//    return f;
//}
//
//bool isChunkVisible(const Frustum& f, const glm::vec2& chunkPos) {
//    glm::vec3 min(chunkPos.x * CHUNK_SIZE, 0.0f, chunkPos.y * CHUNK_SIZE);
//    glm::vec3 max((chunkPos.x + 1) * CHUNK_SIZE, CHUNK_SIZE * CHUNK_SIZE, (chunkPos.y + 1) * CHUNK_SIZE);
//
//
//    for (int i = 0; i < 6; ++i) {
//        glm::vec4 plane = f.planes[i];
//        plane /= glm::length(glm::vec3(plane));
//
//        // Find the most positive vertex (farthest in direction of plane normal)
//        glm::vec3 p = glm::vec3(
//            (plane.x < 0) ? min.x : max.x,
//            (plane.y < 0) ? min.y : max.y,
//            (plane.z < 0) ? min.z : max.z
//        );
//
//        // If that vertex is outside the plane, chunk is outside
//        if (plane.x * p.x + plane.y * p.y + plane.z * p.z + plane.w < 0)
//            return false;
//    }
//
//    return true;
//}