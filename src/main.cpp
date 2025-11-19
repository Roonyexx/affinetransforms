#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numbers>
#include <sstream>
#include <iomanip>

struct Vertex {
    float x{}, y{}, z{}, w{1.0f};
};
inline Vertex make_vertex(float x, float y, float z) {
    return Vertex{x, y, z, 1.0f};
}

Vertex mulMatVec(const std::vector<std::vector<float>>& m, const Vertex& v) {
    Vertex r;
    r.x = m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z + m[0][3]*v.w;
    r.y = m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z + m[1][3]*v.w;
    r.z = m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z + m[2][3]*v.w;
    r.w = m[3][0]*v.x + m[3][1]*v.y + m[3][2]*v.z + m[3][3]*v.w;
    return r;
}

std::vector<std::vector<float>> matMul(const std::vector<std::vector<float>>& A, const std::vector<std::vector<float>>& B) {
    if (A[0].size() != B.size()) throw std::runtime_error("invalid matrix multiplication");
    size_t r = A.size(), c = B[0].size(), k = B.size();
    std::vector<std::vector<float>> R(r, std::vector<float>(c, 0.0f));
    for (size_t i = 0; i < r; i++)
        for (size_t j = 0; j < c; j++)
            for (size_t t = 0; t < k; t++)
                R[i][j] += A[i][t] * B[t][j];
    return R;
}

std::vector<std::vector<float>> translate(float dx, float dy, float dz) {
    return {{1,0,0,dx},{0,1,0,dy},{0,0,1,dz},{0,0,0,1}};
}
std::vector<std::vector<float>> scaleMat(float sx, float sy, float sz) {
    return {{sx,0,0,0},{0,sy,0,0},{0,0,sz,0},{0,0,0,1}};
}
std::vector<std::vector<float>> rotX(float a) {
    return {{1,0,0,0},{0,(float)cos(a),(float)sin(a),0},{0,(float)-sin(a),(float)cos(a),0},{0,0,0,1}};
}
std::vector<std::vector<float>> rotY(float a) {
    return {{(float)cos(a),0,(float)-sin(a),0},{0,1,0,0},{(float)sin(a),0,(float)cos(a),0},{0,0,0,1}};
}
std::vector<std::vector<float>> rotZ(float a) {
    return {{(float)cos(a),(float)sin(a),0,0},{(float)-sin(a),(float)cos(a),0,0},{0,0,1,0},{0,0,0,1}};
}
std::vector<std::vector<float>> reflect(bool rx, bool ry, bool rz) {
    float fx = rx ? -1.0f : 1.0f;
    float fy = ry ? -1.0f : 1.0f;
    float fz = rz ? -1.0f : 1.0f;
    return {{fx,0,0,0},{0,fy,0,0},{0,0,fz,0},{0,0,0,1}};
}

std::vector<std::vector<float>> ortho(float left, float right, float bottom, float top, float n, float f) {
    float tx = -(right+left)/(right-left), ty=-(top+bottom)/(top-bottom), tz=-(f+n)/(f-n);
    return {{2.0f/(right-left), 0, 0, tx},
            {0, 2.0f/(top-bottom), 0, ty},
            {0, 0, -2.0f/(f-n), tz},
            {0, 0, 0, 1}};
}

float dot(Vertex v1, Vertex v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}


inline float deg2rad(float d) { return d * std::numbers::pi_v<float> / 180.0f; }

struct Plane {
    std::vector<int> verts;
    std::vector<std::vector<int>> tris;  
    float dot;
    bool facing{true};
};

class Object {
public:
    Vertex viewDirection{ 0, 0, 1 };
    std::vector<Vertex> original, world, projected;
    std::vector<std::pair<int,int>> edges;
    std::vector<Plane> planes;
    std::vector<std::vector<int>> edgeAdj;
    
    std::vector<std::vector<float>> model, view, projection;
    Vertex center;
    bool useRoberts{true};


    Object() {
        model = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
        view = model;
        projection = model;
    }

    void recompute() {
        auto VM = matMul(view, model);
        world.resize(original.size());
        projected.resize(original.size());
        for (size_t i = 0; i < original.size(); ++i) {
            world[i] = mulMatVec(VM, original[i]);
            projected[i] = mulMatVec(projection, world[i]);
        }
        updateFaceFacingEye();
        buildEdgeAdjacency();
    }

    void setModel(const std::vector<std::vector<float>>& m) { model = m; recompute(); }
    void setView(const std::vector<std::vector<float>>& v) { view = v; recompute(); }
    void setProjection(const std::vector<std::vector<float>>& p) { projection = p; recompute(); }

    void updateFaceFacingEye() {
        for (auto &pl : planes) {
            Vertex v0, v1, v2;
            if (!pl.tris.empty()) {
                auto &tri = pl.tris[0];
                v0 = projected[tri[0]]; v1 = projected[tri[1]]; v2 = projected[tri[2]];
            } 
            else {
                if (pl.verts.size() < 3) { pl.facing = false; continue; }
                v0 = projected[pl.verts[0]]; v1 = projected[pl.verts[1]]; v2 = projected[pl.verts[2]];
            }
            Vertex a{v1.x-v0.x, v1.y-v0.y, v1.z-v0.z};
            Vertex b{v2.x-v0.x, v2.y-v0.y, v2.z-v0.z};
            Vertex n;
            n.x = a.y*b.z - a.z*b.y;
            n.y = a.z*b.x - a.x*b.z;
            n.z = a.x*b.y - a.y*b.x;
        
            pl.dot = dot(n, viewDirection);
        
            pl.facing = (pl.dot > 0.0f);
        }
    }

    void buildEdgeAdjacency() {
        edgeAdj.assign(edges.size(), {});
        for (size_t pi = 0; pi < planes.size(); ++pi) {  
            auto &pl = planes[pi];
            for (size_t i = 0; i < pl.verts.size(); ++i) {
                int a = pl.verts[i];
                int b = pl.verts[(i+1)%pl.verts.size()];
                for (size_t ei = 0; ei < edges.size(); ++ei) {
                    int e1 = edges[ei].first, e2 = edges[ei].second;
                    if ((e1==a && e2==b) || (e1==b && e2==a)) {
                        edgeAdj[ei].push_back(pi);
                        break;
                    }
                }
            }
        }
    }


    void draw() {
        if (useRoberts) drawRoberts(); else drawWire();
    }

    void drawWire() {
        glBegin(GL_LINES);
        glColor3f(1,1,1);
        for (auto &e : edges) {
            auto &v1 = projected[e.first];
            auto &v2 = projected[e.second];
            glVertex3f(v1.x,v1.y,v1.z);
            glVertex3f(v2.x,v2.y,v2.z);
        }
        glEnd();
    }

    void drawRoberts() {
        glEnable(GL_DEPTH_TEST);
        for (auto &pl : planes) {
            if (!pl.facing) continue;
            if (!pl.tris.empty()) {
                glBegin(GL_TRIANGLES);
                glColor3f(0.3f,0.6f,0.9f);
                for (auto &tri : pl.tris) {
                    for (int idx : tri) {
                        auto &v = projected[idx];
                        glVertex3f(v.x,v.y,v.z);
                    }
                }
                glEnd();

            } else {
                if (pl.verts.size()==3) glBegin(GL_TRIANGLES);
                else glBegin(GL_POLYGON);
                glColor3f(0.3f,0.6f,0.9f);
                for (int idx : pl.verts) {
                    auto &v = projected[idx];
                    glVertex3f(v.x,v.y,v.z);
                }
                glEnd();

            }
        }
        glDisable(GL_DEPTH_TEST);
        glBegin(GL_LINES);
        for (size_t ei=0; ei<edges.size(); ++ei) {
            bool vis=false;
            for (int pi : edgeAdj[ei]) {
                if (planes[pi].facing) { vis=true; break; }
            }
            if (!vis) continue;
            auto &v1 = projected[edges[ei].first];
            auto &v2 = projected[edges[ei].second];
            glColor3f(1,1,1);
            glVertex3f(v1.x,v1.y,v1.z);
            glVertex3f(v2.x,v2.y,v2.z);
        }
        glEnd();
    }
};

Object initLetterK() {
    Object o;
    o.original = {
        make_vertex(-0.2f,  0.2f,  0.0f), 
        make_vertex(-0.2f, -0.2f,  0.0f), 
        make_vertex(-0.1f, -0.2f,  0.0f),
        make_vertex(-0.1f, -0.05f, 0.0f), 
        make_vertex( 0.05f,-0.2f,  0.0f), 
        make_vertex( 0.15f,-0.2f,  0.0f), 
        make_vertex(-0.05f, 0.0f,  0.0f), 
        make_vertex( 0.1f,  0.2f,  0.0f),
        make_vertex( 0.0f,  0.2f,  0.0f), 
        make_vertex(-0.1f,  0.075f,0.0f), 
        make_vertex(-0.1f,  0.2f,  0.0f), 
        make_vertex(-0.2f,  0.2f, -0.1f), 
        make_vertex(-0.2f, -0.2f, -0.1f), 
        make_vertex(-0.1f, -0.2f, -0.1f), 
        make_vertex(-0.1f, -0.05f,-0.1f), 
        make_vertex( 0.05f,-0.2f, -0.1f), 
        make_vertex( 0.15f,-0.2f, -0.1f), 
        make_vertex(-0.05f, 0.0f, -0.1f), 
        make_vertex( 0.1f,  0.2f, -0.1f), 
        make_vertex( 0.0f,  0.2f, -0.1f), 
        make_vertex(-0.1f,  0.075f,-0.1f), 
        make_vertex(-0.1f,  0.2f, -0.1f) 
    };

    o.edges = {
        {0,1},{1,2},{2,3},{3,4},{4,5},{5,6},{6,7},{7,8},{8,9},{9,10},{10,0},
        {11,12},{12,13},{13,14},{14,15},{15,16},{16,17},{17,18},{18,19},{19,20},{20,21},{21,11},
        {0,11},{1,12},{2,13},{3,14},{4,15},{5,16},{6,17},{7,18},{8,19},{9,20},{10,21}
    };

    Vertex minp = o.original[0], maxp = o.original[0];
    for (auto &v : o.original) {
        minp.x = std::min(minp.x, v.x);
        minp.y = std::min(minp.y, v.y);
        minp.z = std::min(minp.z, v.z);
        maxp.x = std::max(maxp.x, v.x);
        maxp.y = std::max(maxp.y, v.y);
        maxp.z = std::max(maxp.z, v.z);
    }
    Vertex center = {(minp.x+maxp.x)/2.0f, (minp.y+maxp.y)/2.0f, (minp.z+maxp.z)/2.0f};
    for (auto &v : o.original) {
        v.x -= center.x;
        v.y -= center.y;
        v.z -= center.z;
    }

    o.planes.push_back({{0,1,2,3,4,5,6,7,8,9,10},
                        {{0,1,10},{1,2,10},{3,4,9},{4,6,9},{4,5,6},{6,7,9},{7,8,9}}});
    o.planes.push_back({{11,12,13,14,15,16,17,18,19,20,21},
                        {{11,21,12},{12,21,13},{14,20,15},{15,20,17},{15,17,16},{17,20,18},{18,20,19}}});
    o.planes.push_back({{0,11,12,1},{}});
    o.planes.push_back({{1,12,13,2},{}});
    o.planes.push_back({{2,13,14,3},{}});
    o.planes.push_back({{3,14,15,4},{}});
    o.planes.push_back({{4,15,16,5},{}});
    o.planes.push_back({{5,16,17,6},{}});
    o.planes.push_back({{6,17,18,7},{}});
    o.planes.push_back({{7,18,19,8},{}});
    o.planes.push_back({{8,19,20,9},{}});
    o.planes.push_back({{9,20,21,10},{}});
    o.planes.push_back({{10,21,11,0},{}});
    return o;
}

Object initAxes() {
    Object a;
    a.original = { make_vertex(0,0,0), make_vertex(2,0,0),
                   make_vertex(0,2,0), make_vertex(0,0,2) };
    a.edges = {{0,1},{0,2},{0,3}};
    a.useRoberts = false;
    return a;
}

class Controller {
    Object &obj;

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

    float rx{}, ry{}, rz{};
    float px{}, py{}, pz{};


public:
    Controller(Object &o): obj(o), animationParam(&px, &rz) {}

    std::vector<std::vector<float>> posSliders() {
        ImGui::BeginChild("position");
        ImGui::Text("Position");
        ImGui::SliderFloat("X", &px, -1.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Y", &py, -1.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Z", &pz, -1.0f, 1.0f, "%.3f");
        ImGui::EndChild(); ImGui::Separator();
        return translate(px,py,pz);
    }
    std::vector<std::vector<float>> scaleSliders() {
        ImGui::BeginChild("scale");
        ImGui::Text("Scale");
        static float sx=1, sy=1, sz=1;
        ImGui::SliderFloat("X", &sx, 0.01f, 5.0f, "%.2f");
        ImGui::SliderFloat("Y", &sy, 0.01f, 5.0f, "%.2f");
        ImGui::SliderFloat("Z", &sz, 0.01f, 5.0f, "%.2f");
        ImGui::EndChild(); ImGui::Separator();
        return scaleMat(sx,sy,sz);
    }

    std::vector<std::vector<float>> rotateSliders() {
        ImGui::BeginChild("rotate");
        ImGui::Text("Rotate");
        ImGui::SliderFloat("X", &rx, -180.0f, 180.0f, "%.1f");
        ImGui::SliderFloat("Y", &ry, -180.0f, 180.0f, "%.1f");
        ImGui::SliderFloat("Z", &rz, -180.0f, 180.0f, "%.1f");
        ImGui::EndChild(); ImGui::Separator();
        auto m = rotX(deg2rad(rx));
        m = matMul(m, rotY(deg2rad(ry)));
        m = matMul(m, rotZ(deg2rad(rz)));
        return m;
    }

    std::vector<std::vector<float>> reflectionCB() {
        ImGui::BeginChild("reflection");
        ImGui::Text("Reflection");
        static bool rx=false, ry=false, rz=false;
        ImGui::Checkbox("X-Y plane", &rx);
        ImGui::Checkbox("Y-Z plane", &ry);
        ImGui::Checkbox("X-Z plane", &rz);
        ImGui::EndChild(); ImGui::Separator();
        return reflect(rx, ry, rz);
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
        if(ImGui::RadioButton("x", &chosenTranslationAxis, 0)) animationParam.translateAxis = &px;
        if(ImGui::RadioButton("y", &chosenTranslationAxis, 1)) animationParam.translateAxis = &py;
        if(ImGui::RadioButton("z", &chosenTranslationAxis, 2)) animationParam.translateAxis = &pz;

        ImGui::Text("Rotation plane");
        if(ImGui::RadioButton("XOY", &chosenRotationPlane, 0)) animationParam.rotatePlane = &rz;
        if(ImGui::RadioButton("YOZ", &chosenRotationPlane, 1)) animationParam.rotatePlane = &rx;
        if(ImGui::RadioButton("XOZ", &chosenRotationPlane, 2)) animationParam.rotatePlane = &ry;
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0,0,width,height);
}

int main() {
    if (!glfwInit()) { std::cerr<<"failed to init glfw\n"; return -1; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    GLFWwindow* window = glfwCreateWindow(800,800,"гойда", nullptr, nullptr);
    if (!window) { std::cerr<<"failed to create window\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { std::cerr<<"failed to init glad\n"; return -1; }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window,true);
    ImGui_ImplOpenGL3_Init("#version 120");

    auto view = translate(0.0f,0.0f,-3.0f);
    view = matMul(view, rotX(deg2rad(30.0f)));
    view = matMul(view, rotY(deg2rad(-40.0f)));
    view = matMul(view, scaleMat(1.0f,1.0f,-1.0f));

    Object k = initLetterK();
    Object axes = initAxes();
    k.setView(view);  axes.setView(view);

    auto proj = ortho(-1.0f,1.0f,-1.0f,1.0f,0.1f,100.0f);
    k.setProjection(proj);
    axes.setProjection(proj);

    Controller ctrl(k);


    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        ImGui::Begin("Settings");
        if(ctrl.imguiAnim())
        {
            ctrl.animFrame();
        }
        auto T = ctrl.posSliders();
        auto R = ctrl.rotateSliders();
        auto S = ctrl.scaleSliders();
        auto refl = ctrl.reflectionCB();



        auto modelMat = T;
        modelMat = matMul(modelMat, R);
        modelMat = matMul(modelMat, S);
        //modelMat = matMul(modelMat, refl);
        k.setModel(modelMat);

        ImGui::End();

        ImGui::Begin("Roberts Info");
        for (size_t i=0; i<k.planes.size(); ++i) {
            std::ostringstream oss;
            oss<<"Face "<<i<<" dot = "<<k.planes[i].dot;
            if (k.planes[i].facing) 
                ImGui::TextColored(ImVec4(0.3f,1.0f,0.3f,1.0f), "%s", oss.str().c_str());
            else
                ImGui::TextColored(ImVec4(1.0f,0.3f,0.3f,1.0f), "%s", oss.str().c_str());
        }
        ImGui::End();

        glClearColor(0.1f,0.1f,0.1f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        axes.draw();
        k.draw();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}