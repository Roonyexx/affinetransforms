#include "math3d.hpp"

#include <cmath>
#include <numbers>
#include <stdexcept>

namespace math3d {

Vertex make_vertex(float x, float y, float z) {
    return Vertex{x, y, z, 1.0f};
}

Vertex mulMatVec(const std::vector<std::vector<float>>& m, const Vertex& v) {
    Vertex r;
    r.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w;
    r.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w;
    r.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w;
    r.w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w;
    return r;
}

std::vector<std::vector<float>> matMul(const std::vector<std::vector<float>>& A, 
                                       const std::vector<std::vector<float>>& B) {
    if (A[0].size() != B.size()) 
        throw std::runtime_error("invalid matrix multiplication");
    

    const size_t r = A.size();
    const size_t c = B[0].size();
    const size_t k = B.size();

    std::vector<std::vector<float>> R(r, std::vector<float>(c, 0.0f));
    for (size_t i = 0; i < r; i++) {
        if (A[i].size() != k) throw std::runtime_error("invalid matrix multiplication");

        for (size_t j = 0; j < c; j++) {
            for (size_t t = 0; t < k; t++) {
                if (B[t].size() != c || j >= B[t].size())
                    throw std::runtime_error("invalid matrix multiplication");
                R[i][j] += A[i][t] * B[t][j];
            }
        }
    }

    return R;
}

std::vector<std::vector<float>> translate(float dx, float dy, float dz) {
    return {{1, 0, 0, dx}, {0, 1, 0, dy}, {0, 0, 1, dz}, {0, 0, 0, 1}};
}

std::vector<std::vector<float>> scaleMat(float sx, float sy, float sz) {
    return {{sx, 0, 0, 0}, {0, sy, 0, 0}, {0, 0, sz, 0}, {0, 0, 0, 1}};
}

std::vector<std::vector<float>> rotX(float a) {
    return {{1, 0, 0, 0}, {0, static_cast<float>(cos(a)), static_cast<float>(sin(a)), 0}, {0, static_cast<float>(-sin(a)), static_cast<float>(cos(a)), 0}, {0, 0, 0, 1}};
}

std::vector<std::vector<float>> rotY(float a) {
    return {{static_cast<float>(cos(a)), 0, static_cast<float>(-sin(a)), 0}, {0, 1, 0, 0}, {static_cast<float>(sin(a)), 0, static_cast<float>(cos(a)), 0}, {0, 0, 0, 1}};
}

std::vector<std::vector<float>> rotZ(float a) {
    return {{static_cast<float>(cos(a)), static_cast<float>(sin(a)), 0, 0}, {static_cast<float>(-sin(a)), static_cast<float>(cos(a)), 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
}

std::vector<std::vector<float>> reflect(bool rx, bool ry, bool rz) {
    const float fx = rx ? -1.0f : 1.0f;
    const float fy = ry ? -1.0f : 1.0f;
    const float fz = rz ? -1.0f : 1.0f;
    return {{fx, 0, 0, 0}, {0, fy, 0, 0}, {0, 0, fz, 0}, {0, 0, 0, 1}};
}

std::vector<std::vector<float>> ortho(float left, float right, float bottom, float top, float n, float f) {
    const float tx = -(right + left) / (right - left);
    const float ty = -(top + bottom) / (top - bottom);
    const float tz = -(f + n) / (f - n);

    return {
        {2.0f / (right - left), 0, 0, tx},
        {0, 2.0f / (top - bottom), 0, ty},
        {0, 0, -2.0f / (f - n), tz},
        {0, 0, 0, 1}
    };
}

float dot(Vertex v1, Vertex v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float deg2rad(float d) {
    return d * std::numbers::pi_v<float> / 180.0f;
}

}
