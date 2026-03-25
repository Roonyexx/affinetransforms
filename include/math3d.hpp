#pragma once

#include <vector>

namespace math3d {

struct Vertex {
    float x{};
    float y{};
    float z{};
    float w{1.0f};
};

Vertex make_vertex(float x, float y, float z);
Vertex mulMatVec(const std::vector<std::vector<float>>& m, const Vertex& v);
std::vector<std::vector<float>> matMul(const std::vector<std::vector<float>>& A, const std::vector<std::vector<float>>& B);

std::vector<std::vector<float>> translate(float dx, float dy, float dz);
std::vector<std::vector<float>> scaleMat(float sx, float sy, float sz);
std::vector<std::vector<float>> rotX(float a);
std::vector<std::vector<float>> rotY(float a);
std::vector<std::vector<float>> rotZ(float a);
std::vector<std::vector<float>> reflect(bool rx, bool ry, bool rz);
std::vector<std::vector<float>> ortho(float left, float right, float bottom, float top, float n, float f);

float dot(Vertex v1, Vertex v2);
float deg2rad(float d);

}
