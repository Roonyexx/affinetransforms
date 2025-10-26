#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <vector>
#include <numbers>

struct Vertex
{
    float x{}, y{}, z{};
};

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

    void transformVertexes()
    {
        for (size_t i = 0; i < originalVertices.size(); i++)
        {
            const Vertex& src = originalVertices[i];
            Vertex& dst = vertices[i];

            for (size_t j = 0; j < 3; j++)
            {
                float temp { src.x * transformMatrix[0][j] +
                            src.y * transformMatrix[1][j] +
                            src.z * transformMatrix[2][j] +
                            1.0f  * transformMatrix[3][j] };

                switch (j)
                {
                    case 0: dst.x = temp; break;
                    case 1: dst.y = temp; break;
                    case 2: dst.z = temp; break;
                }
            }
        }
    }

    std::vector<Vertex> originalVertices;
    std::vector<Vertex> vertices;
    std::vector<Edge> edges;
    std::vector<std::vector<float>> transformMatrix;
public:
    Vertex center;
    Vertex scale;
    Vertex rotate;
    Reflection reflection;

    Object() : transformMatrix(4, std::vector<float>(4, 0.0f)), scale { 1, 1, 1 }
    {
        for (size_t i{}; i < 4; i++)
            transformMatrix[i][i] = 1.0f;
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



    friend Object initLetterK();
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
    obj.vertices = obj.originalVertices;

    Vertex minpos { obj.vertices[0].x, obj.vertices[0].y, obj.vertices[0].z };
    Vertex maxpos { obj.vertices[0].x, obj.vertices[0].y, obj.vertices[0].z };

    for (const auto& vertex : obj.vertices)
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
        { 1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 1, 0 },
        { dx, dy, dz, 1 }
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
    a = (int)a ? -1 : 1;
    b = (int)b ? -1 : 1;
    c = (int)c ? -1 : 1;
    return {
        { b, 0, 0, 0 },
        { 0, c, 0, 0 },
        { 0, 0, a, 0 },
        { 0, 0, 0, 1 }
    };
}

float degreeToRad(float angle)
{
    return angle * std::numbers::pi / 180.0;
}

class Controller
{
private:
    Object& obj;

public:
    Controller(Object& o) : obj { o } { }

    std::vector<std::vector<float>> imguiPosSliders()
    {
        ImGui::BeginChild("position");
        ImGui::Text("Position");
        ImGui::SliderFloat("x", &obj.center.x, -1.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("y", &obj.center.y, -1.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("z", &obj.center.z, -1.0f, 1.0f, "%.3f");
        ImGui::EndChild();

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

        Vertex curCenter { obj.center };

        std::vector<std::vector<float>> tmp { getTranslateMatrix(-curCenter.x, -curCenter.y, -curCenter.z) };
        tmp = matrixMult(tmp, getScaleMatrix(obj.scale.x, obj.scale.y, obj.scale.z));
        tmp = matrixMult(tmp, getTranslateMatrix(curCenter.x, curCenter.y, curCenter.z));

        return tmp;
    }

    std::vector<std::vector<float>> imguiRotateSliders()
    {
        ImGui::BeginChild("rotate");
        ImGui::Text("Rotate");
        ImGui::SliderFloat("x", &obj.rotate.x, -180.0f, 180.0f, "%.1f");
        ImGui::SliderFloat("y", &obj.rotate.y, -180.0f, 180.0f, "%.1f");
        ImGui::SliderFloat("z", &obj.rotate.z, -180.0f, 180.0f, "%.1f");
        ImGui::EndChild();
        
        std::vector<std::vector<float>> rotateMatrix { getRotateMatrixX(degreeToRad(obj.rotate.x)) };
        rotateMatrix = matrixMult(rotateMatrix, getRotateMatrixY(degreeToRad(obj.rotate.y)));
        rotateMatrix = matrixMult(rotateMatrix, getRotateMatrixZ(degreeToRad(obj.rotate.z)));
        rotateMatrix = matrixMult(getTranslateMatrix(-obj.center.x, -obj.center.y, -obj.center.z), rotateMatrix);
        rotateMatrix = matrixMult(rotateMatrix, getTranslateMatrix(obj.center.x, obj.center.y, obj.center.z));
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

        return getReflectionMatrix(obj.reflection.x, obj.reflection.y, obj.reflection.z);
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

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "надеюсь, я не забуду поменять это глупенькое название, я кстати попил вкусный зеленый чай", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glMatrixMode(GL_PROJECTION);
    glOrtho(-1, 1, -1.0, 1.0, 0.1, 100.0);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 120");

    Object kLetter { initLetterK() };
    Controller controller { kLetter };


    std::vector<std::vector<float>> transformMatrix;
    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Settings");
        transformMatrix = controller.imguiPosSliders();
        transformMatrix = matrixMult(transformMatrix, controller.imguiScaleSliders());
        transformMatrix = matrixMult(transformMatrix, controller.imguiRotateSliders());
        transformMatrix = matrixMult(transformMatrix, controller.imguiReflectionCB());
        // 12.Вращение вокруг геометрического центра в одной плоскости с одновременным перемещением вдоль одной из осей.
        
        ImGui::End();

        kLetter.setTransformMatrix(transformMatrix);


        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glTranslatef(0.0f, 0.0f, -3.0f);
        glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(-40.0f, 0.0f, 1.0f, 0.0f);

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

