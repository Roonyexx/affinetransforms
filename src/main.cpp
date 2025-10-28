#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <vector>
#include <numbers>
#include <cmath>

struct Vertex
{
    float x{}, y{}, z{}, w{1.0f};
};

Vertex multiplyVectorMatrix(const Vertex& v, const std::vector<std::vector<float>>& m)
{
    Vertex result;
    result.x = v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2] + v.w * m[0][3];
    result.y = v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2] + v.w * m[1][3];
    result.z = v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2] + v.w * m[2][3];
    result.w = v.x * m[3][0] + v.y * m[3][1] + v.z * m[3][2] + v.w * m[3][3];
    return result;
}

class Object
{
private:
    struct Edge
    {
        int v1, v2;
    };
    struct Reflection
    {
        bool x{ false }, y{ false }, z{ false };
    };

    
    std::vector<Vertex> originalVertices;
    std::vector<Vertex> vertices;
    std::vector<Edge> edges;
    std::vector<std::vector<float>> transformMatrix;
    std::vector<std::vector<float>> viewMatrix;
    std::vector<std::vector<float>> projectionMatrix;
    
    public:
    Vertex center;
    Vertex scale;
    Vertex rotate;
    Reflection reflection;
    
    Object() : transformMatrix(4, std::vector<float>(4, 0.0f)), 
    viewMatrix(4, std::vector<float>(4, 0.0f)),
    projectionMatrix(4, std::vector<float>(4, 0.0f)),
    scale { 1, 1, 1 }
    {
        for (size_t i{}; i < 4; i++)
        {
            transformMatrix[i][i] = 1.0f;
            viewMatrix[i][i] = 1.0f;
            projectionMatrix[i][i] = 1.0f;
        }
    }
    
    void transformVertexes()
    {
        for (size_t i = 0; i < originalVertices.size(); i++)
        {
            Vertex src;
            src.x = originalVertices[i].x;
            src.y = originalVertices[i].y;
            src.z = originalVertices[i].z;
            src.w = 1.0f;

            Vertex temp = multiplyVectorMatrix(src, transformMatrix);
            temp = multiplyVectorMatrix(temp, viewMatrix);
            Vertex result = multiplyVectorMatrix(temp, projectionMatrix);
            
            vertices[i].x = result.x;
            vertices[i].y = result.y;
            vertices[i].z = result.z;
        }
    }

    void draw()
    {
        glBegin(GL_LINES);
        glColor3f(1.0f, 1.0f, 1.0f);

        for (const auto& edge : edges)
        {
            const Vertex& v1 = vertices[edge.v1];
            const Vertex& v2 = vertices[edge.v2];

            glVertex3f(v1.x, v1.y, v1.z);
            glVertex3f(v2.x, v2.y, v2.z);
        }
        glEnd();
    }

    void setTransformMatrix(const std::vector<std::vector<float>>& transformMatrix)
    {
        if (transformMatrix.size() != 4 || transformMatrix[0].size() != 4)
            throw std::runtime_error("invalid arg");

        for (size_t i = 0; i < 4; i++)
            this->transformMatrix[i] = transformMatrix[i];

        transformVertexes();
    }

    void setViewMatrix(const std::vector<std::vector<float>>& viewMatrix)
    {
        if (viewMatrix.size() != 4 || viewMatrix[0].size() != 4)
            throw std::runtime_error("invalid arg");

        for (size_t i = 0; i < 4; i++)
            this->viewMatrix[i] = viewMatrix[i];
    }

    void setProjectionMatrix(const std::vector<std::vector<float>>& projectionMatrix)
    {
        if (projectionMatrix.size() != 4 || projectionMatrix[0].size() != 4)
            throw std::runtime_error("invalid arg");

        for (size_t i = 0; i < 4; i++)
            this->projectionMatrix[i] = projectionMatrix[i];
    }

    friend Object initLetterK();
    friend Object initAxes();
};

Object initLetterK()
{
    Object obj;
    obj.originalVertices = {
        { -0.2f, 0.2f, 0.0f }, //a
        { -0.2f, -0.2f, 0.0f }, //b
        { -0.1f, -0.2f, 0.0f }, //c
        { -0.1f, -0.05f, 0.0f }, //d
        { 0.05f, -0.2f, 0.0f }, //e
        { 0.15f, -0.2f, 0.0f }, //f
        { -0.05f, 0.0f, 0.0f }, //g
        { 0.1f, 0.2f, 0.0f },  //h
        { 0.0f, 0.2f, 0.0f }, //i
        { -0.1f, 0.075f, 0.0f }, //j
        { -0.1f, 0.2f, 0.0f },  //k

        { -0.2f, 0.2f, -0.1f }, //a1 11
        { -0.2f, -0.2f, -0.1f }, //b1
        { -0.1f, -0.2f, -0.1f }, //c1
        { -0.1f, -0.05f, -0.1f }, //d1
        { 0.05f, -0.2f, -0.1f }, //e1
        { 0.15f, -0.2f, -0.1f }, //f1
        { -0.05f, 0.0f, -0.1f }, //g1
        { 0.1f, 0.2f, -0.1f },  //h1
        { 0.0f, 0.2f, -0.1f }, //i1
        { -0.1f, 0.075f, -0.1f }, //j1
        { -0.1f, 0.2f, -0.1f }  //k1
    };

    obj.edges = {
        { 0, 1 },
        { 1, 2 },
        { 2, 3 },
        { 3, 4 },
        { 4, 5 },
        { 5, 6 },
        { 6, 7 },
        { 7, 8 },
        { 8, 9 },
        { 9, 10 },
        { 10, 0 },

        {11, 12},
        {12, 13},
        {13, 14},
        {14, 15},
        {15, 16},
        {16, 17},
        {17, 18},
        {18, 19},
        {19, 20},
        {20, 21},
        {21, 11},

        {0,11},
        {1,12},
        {2,13},
        {3,14},
        {4,15},
        {5,16},
        {6,17},
        {7,18},
        {8,19},
        {9,20},
        {10,21}
    };

    Vertex minpos { obj.originalVertices[0].x, obj.originalVertices[0].y, obj.originalVertices[0].z };
    Vertex maxpos { obj.originalVertices[0].x, obj.originalVertices[0].y, obj.originalVertices[0].z };

    for (const auto& vertex : obj.originalVertices)
    {
        if(minpos.x > vertex.x) minpos.x = vertex.x;
        if(minpos.y > vertex.y) minpos.y = vertex.y;
        if(minpos.z > vertex.z) minpos.z = vertex.z;

        if(maxpos.x < vertex.x) maxpos.x = vertex.x;
        if(maxpos.y < vertex.y) maxpos.y = vertex.y;
        if(maxpos.z < vertex.z) maxpos.z = vertex.z;
    }
    Vertex v { (minpos.x + maxpos.x) / 2, (minpos.y + maxpos.y) / 2, (minpos.z + maxpos.z) / 2 };

    for (auto& vertex : obj.originalVertices)
    {
        vertex.x -= v.x;
        vertex.y -= v.y;
        vertex.z -= v.z;
    }
    obj.vertices = obj.originalVertices;
    return obj;
}

Object initAxes()
{
    Object obj;
    obj.originalVertices = {
        { 0, 0, 0 },
        { 2, 0, 0 },
        { 0, 2, 0 },
        { 0, 0, 2 }
    };
    obj.edges = {
        { 0, 1 },
        { 0, 2 },
        { 0, 3 }
    };
    obj.vertices = obj.originalVertices;
    return obj;
}

template <typename T>
std::vector<std::vector<T>> matrixMult(const std::vector<std::vector<T>>& m1, const std::vector<std::vector<T>>& m2)
{
    if (m1[0].size() != m2.size()) throw std::runtime_error("invalid arg");
    const size_t rows{ m1.size() }, cols{ m2[0].size() };
    std::vector<std::vector<T>> result(rows, std::vector<T>(cols, 0));

    for (size_t i{}; i < rows; i++)
    {
        for (size_t j{}; j < cols; j++)
        {
            for (size_t k{}; k < m2.size(); k++)
            {
                result[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }
    return result;
}

std::vector<std::vector<float>> getTranslateMatrix(float dx, float dy, float dz)
{
    return {
        { 1, 0, 0, dx },
        { 0, 1, 0, dy },
        { 0, 0, 1, dz },
        { 0, 0, 0, 1 }
    };
}

std::vector<std::vector<float>> getScaleMatrix(float a, float b, float c)
{
    return {
        { a, 0, 0, 0 },
        { 0, b, 0, 0 },
        { 0, 0, c, 0 },
        { 0, 0, 0, 1 }
    };
}

std::vector<std::vector<float>> getRotateMatrixX(float angle)
{
    return {
        { 1, 0, 0, 0 },
        { 0, std::cos(angle), std::sin(angle), 0 }, 
        { 0, -std::sin(angle), std::cos(angle), 0 },
        { 0, 0, 0, 1 }
    };
}

std::vector<std::vector<float>> getRotateMatrixY(float angle)
{
    return {
        { std::cos(angle), 0, -std::sin(angle), 0 }, 
        { 0, 1, 0, 0 },
        { std::sin(angle), 0, std::cos(angle), 0 },
        { 0, 0, 0, 1 }
    };
}

std::vector<std::vector<float>> getRotateMatrixZ(float angle)
{
    return {
        { std::cos(angle), std::sin(angle), 0, 0 }, 
        { -std::sin(angle), std::cos(angle), 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, 1 }
    };
}

std::vector<std::vector<float>> getReflectionMatrix(float a, float b, float c)
{
    float rx = (int)b ? -1.0f : 1.0f;  
    float ry = (int)c ? -1.0f : 1.0f; 
    float rz = (int)a ? -1.0f : 1.0f;  
    return {
        { rx, 0, 0, 0 },
        { 0, ry, 0, 0 },
        { 0, 0, rz, 0 },
        { 0, 0, 0, 1 }
    };
}


std::vector<std::vector<float>> getOrthoMatrix(float left, float right, float bottom, float top, float nearVal, float farVal)
{
    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);
    float tz = -(farVal + nearVal) / (farVal - nearVal);
    
    return {
        { 2.0f / (right - left), 0, 0, 0 },
        { 0, 2.0f / (top - bottom), 0, 0 },
        { 0, 0, -2.0f / (farVal - nearVal), 0 },
        { tx, ty, tz, 1 }
    };
}

float degreeToRad(float angle)
{
    return angle * std::numbers::pi / 180.0f;
}

class Controller
{
private:
    Object& obj;

    struct AnimationParam
    {
        bool isAnim;
        bool isMovingOnRight;
        float* translateAxis;
        float* rotatePlane;
        float movingSpeed;
        float rotationSpeed;
        double lastTime;
        
        AnimationParam(float* ta, float* rp) : isAnim{ false }, isMovingOnRight{ true }, translateAxis{ ta }, rotatePlane{ rp }, movingSpeed{ 0.5f }, rotationSpeed{ 90.0f }, lastTime{ glfwGetTime() } { }
    } animationParam;

public:
    Controller(Object& o) : obj{ o }, animationParam{ &obj.center.x, &obj.rotate.z } { }

    std::vector<std::vector<float>> imguiPosSliders()
    {
        ImGui::BeginChild("position");
        ImGui::Text("Position");
        ImGui::SliderFloat("x", &obj.center.x, -1.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("y", &obj.center.y, -1.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("z", &obj.center.z, -1.0f, 1.0f, "%.3f");
        ImGui::EndChild();
        ImGui::Separator();        
        return getTranslateMatrix(obj.center.x, obj.center.y, obj.center.z);
    }

    std::vector<std::vector<float>> imguiScaleSliders()
    {
        ImGui::BeginChild("scale");
        ImGui::Text("Scale");
        ImGui::SliderFloat("x", &obj.scale.x, 0.0f, 5.0f, "%.2f");
        ImGui::SliderFloat("y", &obj.scale.y, 0.0f, 5.0f, "%.2f");
        ImGui::SliderFloat("z", &obj.scale.z, 0.0f, 5.0f, "%.2f");
        ImGui::EndChild();
        ImGui::Separator();

        return getScaleMatrix(obj.scale.x, obj.scale.y, obj.scale.z);
    }

    std::vector<std::vector<float>> imguiRotateSliders()
    {
        ImGui::BeginChild("rotate");
        ImGui::Text("Rotate");
        ImGui::SliderFloat("x", &obj.rotate.x, -180.0f, 180.0f, "%.1f");
        ImGui::SliderFloat("y", &obj.rotate.y, -180.0f, 180.0f, "%.1f");
        ImGui::SliderFloat("z", &obj.rotate.z, -180.0f, 180.0f, "%.1f");
        ImGui::EndChild();
        ImGui::Separator();
        
        std::vector<std::vector<float>> rotateMatrix { getRotateMatrixX(degreeToRad(obj.rotate.x)) };
        rotateMatrix = matrixMult(rotateMatrix, getRotateMatrixY(degreeToRad(obj.rotate.y)));
        rotateMatrix = matrixMult(rotateMatrix, getRotateMatrixZ(degreeToRad(obj.rotate.z)));
        return rotateMatrix;
    }

    std::vector<std::vector<float>> imguiReflectionCB()
    {
        ImGui::BeginChild("reflection");
        ImGui::Text("Reflection");
        ImGui::Checkbox("XOY", &obj.reflection.x);
        ImGui::Checkbox("YOZ", &obj.reflection.y);
        ImGui::Checkbox("XOZ", &obj.reflection.z);
        ImGui::EndChild();
        ImGui::Separator();

        return getReflectionMatrix(obj.reflection.x, obj.reflection.y, obj.reflection.z);
    }

    bool imguiAnim()
    {
        static int chosenTranslationAxis{};
        static int chosenRotationPlane{};
        ImGui::BeginChild("animation");
        ImGui::Text("Animation");
        if(ImGui::Checkbox("On animate", &animationParam.isAnim)) animationParam.lastTime = glfwGetTime();
        ImGui::SliderFloat("moving speed", &animationParam.movingSpeed, 0.0f, 5.0f, "%.1f");
        ImGui::SliderFloat("rotation speed", &animationParam.rotationSpeed, 0.0f, 720.0f, "%.1f");
        ImGui::Text("Axis of translation");
        if(ImGui::RadioButton("x", &chosenTranslationAxis, 0)) animationParam.translateAxis = &obj.center.x;
        if(ImGui::RadioButton("y", &chosenTranslationAxis, 1)) animationParam.translateAxis = &obj.center.y;
        if(ImGui::RadioButton("z", &chosenTranslationAxis, 2)) animationParam.translateAxis = &obj.center.z;

        ImGui::Text("Rotation plane");
        if(ImGui::RadioButton("XOY", &chosenRotationPlane, 0)) animationParam.rotatePlane = &obj.rotate.z;
        if(ImGui::RadioButton("YOZ", &chosenRotationPlane, 1)) animationParam.rotatePlane = &obj.rotate.x;
        if(ImGui::RadioButton("XOZ", &chosenRotationPlane, 2)) animationParam.rotatePlane = &obj.rotate.y;
        ImGui::EndChild();
        ImGui::Separator();
        return animationParam.isAnim;
    }

    void animFrame()
    {   
        double currentTime { glfwGetTime() };
        float dt { static_cast<float>(currentTime - animationParam.lastTime) };
        float chPos { animationParam.movingSpeed * dt };
        float chAngle { animationParam.rotationSpeed * dt };
        animationParam.lastTime = currentTime;

        if(animationParam.isMovingOnRight) *animationParam.translateAxis += chPos;
        else *animationParam.translateAxis -= chPos;
        
        if (*animationParam.translateAxis >= 1.0f) {
            *animationParam.translateAxis = 1.0f;
            animationParam.isMovingOnRight = false;
        }
        else if (*animationParam.translateAxis <= -1.0f) {
            *animationParam.translateAxis = -1.0f;
            animationParam.isMovingOnRight = true;
        }

        *animationParam.rotatePlane = (*animationParam.rotatePlane >= 180) ? -180 : *animationParam.rotatePlane + chAngle;
    }
};


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
    const int windowWidth { 800 };
    const int windowHeight { 800 };
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "аффинные преобразования", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    std::vector<std::vector<float>> projectionMatrix = getOrthoMatrix(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    std::vector<std::vector<float>> modelViewMatrix = getTranslateMatrix(0.0f, 0.0f, -3.0f);
    modelViewMatrix = matrixMult(modelViewMatrix, getRotateMatrixX(degreeToRad(30.0f)));
    modelViewMatrix = matrixMult(modelViewMatrix, getRotateMatrixY(degreeToRad(-40.0f)));

    modelViewMatrix = matrixMult(modelViewMatrix, getScaleMatrix(1.0f, 1.0f, -1.0f));


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 120");

    Object kLetter { initLetterK() };
    kLetter.setProjectionMatrix(projectionMatrix);
    kLetter.setViewMatrix(modelViewMatrix);
    Object Axes { initAxes() };
    Axes.setProjectionMatrix(projectionMatrix);
    Axes.setViewMatrix(modelViewMatrix);
    Axes.transformVertexes();
    
    Controller controller { kLetter };

    std::vector<std::vector<float>> transformMatrix;

    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Settings");

        if (controller.imguiAnim()) controller.animFrame();
        transformMatrix = controller.imguiPosSliders();
        transformMatrix = matrixMult(transformMatrix, controller.imguiRotateSliders());
        transformMatrix = matrixMult(transformMatrix, controller.imguiScaleSliders());
        transformMatrix = matrixMult(transformMatrix, controller.imguiReflectionCB());

        ImGui::End();

        kLetter.setTransformMatrix(transformMatrix);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Axes.draw();
        kLetter.draw();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}