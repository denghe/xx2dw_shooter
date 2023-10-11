#include "pch.h"

//int main() {
//
//    glfwSetErrorCallback([](int error, const char* description) {
//        xx::CoutN("glfwSetErrorCallback error = ", error, " description = ", description);
//        xx_assert(false);
//    });
//
//    if (!glfwInit()) return -1;
//    auto sg_glfw = xx::MakeSimpleScopeGuard([] { glfwTerminate(); });
//
//    glfwDefaultWindowHints();
//    glfwWindowHint(GLFW_DEPTH_BITS, 0);
//    glfwWindowHint(GLFW_STENCIL_BITS, 0);
//
//    auto wnd = glfwCreateWindow(1280, 800, "this is title", nullptr, nullptr);
//    if (!wnd) return -2;
//    auto sg_wnd = xx::MakeSimpleScopeGuard([&] { glfwDestroyWindow(wnd); });
//
//    glfwSetFramebufferSizeCallback(wnd, [](GLFWwindow* wnd, int w, int h) {
//        xx::CoutN("glfwSetFramebufferSizeCallback w, h = ", w, ", ", h);
//    });
//
//    int width{}, height{};
//    glfwGetFramebufferSize(wnd, &width, &height);
//
//    glfwMakeContextCurrent(wnd);
//
//    glfwSetInputMode(wnd, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
//    glfwSwapInterval(0);	// no v-sync by default
//
//    if (!gladLoadGL(glfwGetProcAddress)) return -3;
//
//    // dump & cleanup glfw3 error
//    while (auto e = glGetError()) {
//        xx::CoutN("glGetError() == ", e);
//    };
//
//    glEnable(GL_PRIMITIVE_RESTART);
//    glPrimitiveRestartIndex(65535);
//    glPointSize(5);
//    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
//
//    glDisable(GL_CULL_FACE);
//    glDisable(GL_DEPTH_TEST);
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//    FontDefinition fd{
//        "", 24
//    };
//    BitmapDC dc;
//    dc.Init(glfwGetWin32Window(wnd));
//
//    auto lastSecs = xx::NowSteadyEpochSeconds();
//    int64_t drawCounter{};
//    double fps{};
//    while (!glfwWindowShouldClose(wnd)) {
//        glfwPollEvents();
//        glViewport(0, 0, width, height);
//        glClearColor(0, 0, 0, 0);
//        glDepthMask(true);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        glDepthMask(false);
//
//        //
//        ++drawCounter;
//        auto nowSecs = xx::NowSteadyEpochSeconds();
//        if (auto elapsedSecs = nowSecs - lastSecs; elapsedSecs >= 1) {
//            lastSecs = nowSecs;
//            fps = drawCounter / elapsedSecs;
//            drawCounter = 0;
//            xx::CoutN("fps = ", fps);
//        }
//        int w, h;
//        auto d = dc.getTextureDataForText("asdf", fd, TextAlign::CENTER, w, h);
//        xx::CoutN("w = ", w, " h = ", h, " d = ", d, "d.len = ", d.len);
//
//        glfwSwapBuffers(wnd);
//    }
//    return 0;
//}
//
