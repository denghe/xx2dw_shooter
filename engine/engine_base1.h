#pragma once
#include <engine_includes.h>
#include <engine_opengl.h>
#include <engine_prims.h>
#include <engine_shader.h>
#include <engine_shader_quad.h>
#include <engine_shader_linestrip.h>
#include <engine_frame.h>
#include <engine_texturepacker.h>
#include <engine_rnd.h>
#include <engine_zstd.h>
#include <engine_base0.h>
#include <engine_node.h>
#include <engine_node_derived.h>

struct EngineBase1 : EngineBase0 {
    XX_FORCE_INLINE static EngineBase1& Instance() { return *(EngineBase1*)gEngine; }

    Shader_QuadInstance shaderQuadInstance;
    Shader_LineStrip shaderLineStrip;
    // ... more

#ifndef __EMSCRIPTEN__
    std::string title = "unnamed title( Init() set title please )";  // fill at Init()
    GLFWwindow* wnd{};
    ~EngineBase1() {
        glfwDestroyWindow(wnd);
        wnd = {};
        glfwTerminate();
    }
#endif

    void GLInit() {
#ifdef __EMSCRIPTEN__
        emscripten_set_canvas_element_size("canvas", (int)windowWidth, (int)windowHeight);
        EmscriptenWebGLContextAttributes attrs;
        emscripten_webgl_init_context_attributes(&attrs);
        attrs.alpha = 0;
        attrs.majorVersion = 2;
        auto glContext = emscripten_webgl_create_context("canvas", &attrs);
        xx_assert(glContext);
        emscripten_webgl_make_context_current(glContext);
#else
        glfwSetErrorCallback([](int error, const char* description) {
            xx::CoutN("glfwSetErrorCallback error = ", error, " description = ", description);
            xx_assert(false);
        });
        xx_assert(glfwInit());

        glfwDefaultWindowHints();
        glfwWindowHint(GLFW_DEPTH_BITS, 0);
        glfwWindowHint(GLFW_STENCIL_BITS, 0);

        wnd = glfwCreateWindow(1280, 800, title.c_str(), nullptr, nullptr);
        xx_assert(wnd);

        //glfwSetKeyCallback(wnd, [](GLFWwindow* wnd, int key, int scancode, int action, int mods) {
        //    if (key < 0) return;    // macos fn key == -1
        //    xx::engine.kbdStates[key] = action;
        //});

        //glfwSetCharCallback(wnd, [](GLFWwindow* wnd, unsigned int key) {
        //    xx::engine.kbdInputs.push_back(key);
        //});

        //glfwSetScrollCallback(wnd, MouseScrollCallback);
        //glfwSetCursorEnterCallback(wnd, CursorEnterCallback);

        //glfwSetCursorPosCallback(wnd, [](GLFWwindow* wnd, double x, double y) {
        //    xx::engine.mousePosition = { (float)x - xx::engine.w / 2, xx::engine.h / 2 - (float)y };
        //});

        //glfwSetMouseButtonCallback(wnd, [](GLFWwindow* wnd, int button, int action, int mods) {
        //    xx::engine.mbtnStates[button] = action;
        //});

        glfwSetFramebufferSizeCallback(wnd, [](GLFWwindow* wnd, int w, int h) {
            gEngine->SetWindowSize((float)w, (float)h);
        });

        int w{}, h{};
        glfwGetFramebufferSize(wnd, &w, &h);
        gEngine->SetWindowSize((float)w, (float)h);

        glfwMakeContextCurrent(wnd);

        glfwSetInputMode(wnd, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
        glfwSwapInterval(0);	// no v-sync by default

        assert(gladLoadGL(glfwGetProcAddress));

        // dump & cleanup glfw3 error
        while (auto e = glGetError()) {
            xx::CoutN("glGetError() == ", e);
        };

        glEnable(GL_PRIMITIVE_RESTART);
        glPrimitiveRestartIndex(65535);
        glPointSize(5);
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
#endif

        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        shaderQuadInstance.Init(this);
        shaderLineStrip.Init(this);
        // ... more
    }

    XX_FORCE_INLINE void GLViewport() {
        glViewport(0, 0, (int)windowSize.x, (int)windowSize.y);
    }

    XX_FORCE_INLINE void GLClear(RGBA8 c) {
        glClearColor(c.r / 255.f, c.g / 255.f, c.b / 255.f, c.a / 255.f);
        glClear(GL_COLOR_BUFFER_BIT);
        //glDepthMask(true);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glDepthMask(false);
    }

    XX_FORCE_INLINE void GLUpdateBegin() {
        GLViewport();
        GLClear({});

        Shader::ClearCounter();
    }

    XX_FORCE_INLINE void GLUpdateEnd() {
        ShaderEnd();
    }

    // example: EngineBase1::Instance().ShaderBegin(EngineBase1::Instance().shaderQuadInstance);
    template<typename ST>
    XX_FORCE_INLINE ST& ShaderBegin(ST& s) {
        if (shader != &s) {
            ShaderEnd();
            s.Begin();
            shader = &s;
        }
        return s;
    }

    XX_FORCE_INLINE void ShaderEnd() {
        if (shader) {
            shader->End();
            shader = {};
        }
    }

    // todo: GLSetBlend == if diff GLShaderEnd + set blend + GLShaderBegin




    /*****************************************************************************************************/
    /*****************************************************************************************************/

    // for sort
    xx::List<ZNode> tmpZNodes;

    EM_BOOL OnMouseDown(EmscriptenMouseEvent const& e) {
        touchMode = false;
        mouse.btnStates[e.button] = true;	// mouse left btn == 0, right btn == 2
        assert(!mouseEventHandler);
        mouseEventHandlers.ForeachPoint(mouseEventHandlers.max_2 + mouse.pos, [&](auto o) {
            tmpZNodes.Emplace(o->z, o);
            });
        std::sort(tmpZNodes.buf, tmpZNodes.buf + tmpZNodes.len, ZNode::GreaterThanComparer);	// event big z first
        for (auto& zn : tmpZNodes) {
            ((MouseEventHandlerNode*)zn.n)->OnMouseDown();
            if (mouseEventHandler) break;
        }
        tmpZNodes.Clear();
        return EM_TRUE;
    }

    EM_BOOL OnMouseMove(EmscriptenMouseEvent const& e) {
        mouse.pos = { (float)e.targetX - windowSize_2.x, windowSize.y - (float)e.targetY - windowSize_2.y };
        if (mouseEventHandler) {
            mouseEventHandler->OnMouseMove();
        } else {
            mouseEventHandlers.ForeachPoint(mouseEventHandlers.max_2 + mouse.pos, [&](auto o) {
                tmpZNodes.Emplace(o->z, o);
                });
            std::sort(tmpZNodes.buf, tmpZNodes.buf + tmpZNodes.len, ZNode::GreaterThanComparer);	// event big z first
            for (auto& zn : tmpZNodes) {
                ((MouseEventHandlerNode*)zn.n)->OnMouseMove();
                if (mouseEventHandler) break;
            }
            tmpZNodes.Clear();
        }
        return EM_TRUE;
    }

    EM_BOOL OnMouseUp(EmscriptenMouseEvent const& e) {
        mouse.btnStates[e.button] = false;
        if (mouseEventHandler) {
            mouseEventHandler->OnMouseUp();
            mouseEventHandler = {};
        }
        return EM_TRUE;
    }

    /*****************************************************************************************************/
    /*****************************************************************************************************/

    EM_BOOL OnKeyDown(EmscriptenKeyboardEvent const& e) {
        if (e.which >= (int32_t)KeyboardKeys::A && e.which <= (int32_t)KeyboardKeys::Z) {
            keyboardKeysStates[e.which] = true;
            return EM_TRUE;
        }
        return EM_FALSE;
    }
    EM_BOOL OnKeyUp(EmscriptenKeyboardEvent const& e) {
        if (e.which >= (int32_t)KeyboardKeys::A && e.which <= (int32_t)KeyboardKeys::Z) {
            keyboardKeysStates[e.which] = false;
            return EM_TRUE;
        }
        return EM_FALSE;
    }

    bool KeyDown(KeyboardKeys k) const {
        return keyboardKeysStates[(KeyboardKeys_t)k];
    }

    bool KeyDownDelay(KeyboardKeys k, double delaySecs) {
        if (!KeyDown(k)) return false;
        if (keyboardKeysDelays[(KeyboardKeys_t)k] > nowSecs) return false;
        keyboardKeysDelays[(KeyboardKeys_t)k] = nowSecs + delaySecs;
        return true;
    }
};
