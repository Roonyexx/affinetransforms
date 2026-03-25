#include <gtest/gtest.h>

#include <iomanip>
#include <numbers>
#include <sstream>
#include <string>
#include <vector>

#include "math3d.hpp"

using namespace math3d;

constexpr float eps = 1e-4f;

std::string MatrixToString(const std::vector<std::vector<float>>& matrix) {
    std::ostringstream oss;
    for (size_t i = 0; i < matrix.size(); ++i) {
        oss << "[";
        for (size_t j = 0; j < matrix[i].size(); ++j) {
            if (j > 0) {
                oss << ", ";
            }
            oss << std::fixed << std::setprecision(4) << matrix[i][j];
        }
        oss << "]";
        if (i + 1 < matrix.size()) {
            oss << ",";
        }
        oss << "\n";
    }
    return oss.str();
}

void ExpectMatrixEq(const std::vector<std::vector<float>>& first,
                    const std::vector<std::vector<float>>& second,
                    const std::vector<std::vector<float>>& actual,
                    const std::vector<std::vector<float>>& expected,
                    const std::string& context) {
    const std::string matDump {
        "\n" + MatrixToString(first) + "*\n" + MatrixToString(second) + "\nExpected matrix:\n" + MatrixToString(expected) + "\nActual matrix:\n" + MatrixToString(actual)
    };

    ASSERT_EQ(actual.size(), expected.size())
        << context << " row count mismatch: actual = " << actual.size() << ", expected = " << expected.size()
        << matDump;

    for (size_t i = 0; i < expected.size(); ++i) {
        ASSERT_EQ(actual[i].size(), expected[i].size())
            << context << " column count mismatch at row " << i
            << ": actual = " << actual[i].size() << ", expected = " << expected[i].size()
            << matDump;

        for (size_t j = 0; j < expected[i].size(); ++j) {
            EXPECT_NEAR(actual[i][j], expected[i][j], eps)
                << context << " mismatch at [" << i << "][" << j << "]"
                << ": actual = " << actual[i][j] << ", expected = " << expected[i][j]
                << matDump;
        }
    }
}

TEST(MatMul, Computes1x1Multiplication) {
    const std::vector<std::vector<float>> A{{3.5f}};
    const std::vector<std::vector<float>> B{{-2.0f}};

    const auto result { matMul(A, B) };
    ExpectMatrixEq(A, B, result, {{-7.0f}}, "1x1 multiplication must behave like scalar multiplication");
}

TEST(MatMul, ComputesRectangular2x3By3x2Product) {
    const std::vector<std::vector<float>> A{{1, 2, 3}, {4, 5, 6}};
    const std::vector<std::vector<float>> B{{7, 8}, {9, 10}, {11, 12}};

    const auto result { matMul(A, B) };
    ExpectMatrixEq(A, B, result, {{58, 64}, {139, 154}}, "2x3 by 3x2 multiplication produced incorrect values");
}

TEST(MatMul, HandlesNegativeAndFractionalNumbers) {
    const std::vector<std::vector<float>> A{{1.5f, -2.0f}, {-3.0f, 0.25f}};
    const std::vector<std::vector<float>> B{{4.0f, -1.0f}, {2.0f, 8.0f}};

    const auto result { matMul(A, B) };
    ExpectMatrixEq(A, B, result,
                   {{2.0f, -17.5f}, {-11.5f, 5.0f}},
                   "");
}

TEST(MatMul, ZeroMatrixProducesZeroMatrix) {
    const std::vector<std::vector<float>> A{{5, -3}, {9, 4}};
    const std::vector<std::vector<float>> Z{{0, 0, 0}, {0, 0, 0}};

    const auto result { matMul(A, Z) };
    ExpectMatrixEq(A, Z, result, {{0, 0, 0}, {0, 0, 0}}, "Matrix multiplied by zero matrix should be zero");
}

TEST(MatMul, SupportsNonSquare) {
    const std::vector<std::vector<float>> A{{1, 2}};
    const std::vector<std::vector<float>> B{{3}, {4}};

    const auto result { matMul(A, B) };
    ExpectMatrixEq(A, B, result, {{11}}, "1x2 by 2x1 multiplication must produce a 1x1 matrix");
}

TEST(MatMul, NotSameSizeInDiffRow) {
    const std::vector<std::vector<float>> A{{1, 2}, {3, 4}};
    const std::vector<std::vector<float>> B{{5, 6, 7}, {8, 9}};

    EXPECT_THROW(
        {
            try {
                (void)matMul(A, B);
            } catch (const std::runtime_error& e) {
                EXPECT_STREQ("invalid matrix multiplication", e.what())
                    << "Unexpected runtime_error message for dimension mismatch";
                throw;
            }
        },
        std::runtime_error);
}

TEST(MatMul, ThrowsOnIncompatibleDimensions) {
    const std::vector<std::vector<float>> A{{1, 2}, {3, 4}};
    const std::vector<std::vector<float>> B{{1, 2}, {3, 4}, {5, 6}};

    EXPECT_THROW(
        {
            try {
                (void)matMul(A, B);
            } catch (const std::runtime_error& e) {
                EXPECT_STREQ("invalid matrix multiplication", e.what())
                    << "Unexpected runtime_error message for dimension mismatch";
                throw;
            }
        },
        std::runtime_error);
}

TEST(Deg2Rad, ConvertsZeroDegrees) {
    float angle { 0.0f };
    float expected { 0.0f };
    float result { deg2rad(angle) };
    ASSERT_NEAR(result, expected, eps) << "0 degrees should convert to 0 radians";
}

TEST(Deg2Rad, ConvertsOneEightyDegreesToPi) {
    float angle { 180.0f };
    float expected { std::numbers::pi_v<float> };
    float result { deg2rad(angle) };
    ASSERT_NEAR(result, expected, eps) << "180 degrees should convert to pi radians";
}

TEST(Deg2Rad, ConvertsNegativeNinetyDegrees) {
    float angle { -90.0f };
    float expected { -std::numbers::pi_v<float> / 2.0f };
    float result { deg2rad(angle) };
    ASSERT_NEAR(result, expected, eps) << "-90 degrees should convert to -pi/2 radians";
}

TEST(Deg2Rad, ConvertsFullTurn) {
    float angle { 360.0f };
    float expected { 2.0f * std::numbers::pi_v<float> };
    float result { deg2rad(angle) };
    ASSERT_NEAR(result, expected, eps) << "360 degrees should convert to 2*pi radians";
}

TEST(Deg2Rad, IsLinearForScaledAngles) {
    const float angle { 23.5f };
    float expected { 2.0f * deg2rad(angle) };
    float result { deg2rad(2.0f * angle) };
    ASSERT_NEAR(result, expected, eps)
        << "deg2rad(2a) must equal 2*deg2rad(a)";
}
