cmake_minimum_required(VERSION 3.10)
project(affineTransformations)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)
enable_testing()

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GTest CONFIG REQUIRED)

add_library(math3d STATIC
  src/math3d.cpp
)

target_include_directories(math3d PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(${PROJECT_NAME}
  src/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  math3d
  glfw
  glad::glad
  imgui::imgui
  OpenGL::GL
)

add_executable(unit_tests
  tests/math3d_tests.cpp
)

target_link_libraries(unit_tests PRIVATE
  math3d
  GTest::gtest
  GTest::gtest_main
)

add_test(NAME unit_tests COMMAND unit_tests)
