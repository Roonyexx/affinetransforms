#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <vector>


class Object 
{
private:
    struct Vertex 
    {
        float x, y, z;
    };

    struct Edge 
    {
        int v1, v2;
    };

    struct Center
    {
        float x{}, y{}, z{};
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
    Center center;

    Object() : transformMatrix(4, std::vector<float>(4, 0.0f))
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

    void setTransformMatrix(std::vector<std::vector<float>> transformMatrix)
    {
        if (transformMatrix.size() != 4 || transformMatrix[0].size() != 4) throw std::runtime_error("invalid arg");
        for (size_t i{}; i < 4; i++)
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
    return obj;
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

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "надеюсь, я не забуду поменять это глупенькое название, я кстати попил вкусного зеленого чая", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
    {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 120");

    Object kLetter { initLetterK() };


    while (!glfwWindowShouldClose(window)) 
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Settings");
        ImGui::Text("Position:");
        if(ImGui::SliderFloat("x", &kLetter.center.x, -1.0f, 1.0f, "%.2f"))
            kLetter.setTransformMatrix(getTranslateMatrix(kLetter.center.x, kLetter.center.y, kLetter.center.z));
        if(ImGui::SliderFloat("y", &kLetter.center.y, -1.0f, 1.0f, "%.2f"))
            kLetter.setTransformMatrix(getTranslateMatrix(kLetter.center.x, kLetter.center.y, kLetter.center.z));
        if(ImGui::SliderFloat("z", &kLetter.center.z, -1.0f, 1.0f, "%.2f"))
            kLetter.setTransformMatrix(getTranslateMatrix(kLetter.center.x, kLetter.center.y, kLetter.center.z));

        ImGui::End();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();

        kLetter.draw();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

