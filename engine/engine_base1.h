#pragma once
#include <engine_base0.h>
#include <engine_opengl.h>
#include <engine_shader.h>
#include <engine_shader_quad.h>
#include <engine_shader_linestrip.h>
#include <engine_frame.h>
#include <engine_texturepacker.h>
#include <engine_tiledmap_sede.h>
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
        emscripten_set_canvas_element_size("canvas", (int)windowSize.x, (int)windowSize.y);
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

        // todo
        glfwSetKeyCallback(wnd, [](GLFWwindow* wnd, int key, int scancode, int action, int mods) {
            // mods 0b 0011 win alt ctrl shift
            // action 1: down  0: up  2: repeat
            if (key < 0) return;    // macos fn key == -1
            if (action == 0) {
                EngineBase1::Instance().OnKeyUp(EmscriptenKeyboardEvent{ .which = (unsigned long)key });
            } else if (action == 1) {
                EngineBase1::Instance().OnKeyDown(EmscriptenKeyboardEvent{ .which = (unsigned long)key });
            }
            // else action == 2
        });

        //glfwSetCharCallback(wnd, [](GLFWwindow* wnd, unsigned int key) {
        //    xx::engine.kbdInputs.push_back(key);
        //});

        //glfwSetScrollCallback(wnd, MouseScrollCallback);
        //glfwSetCursorEnterCallback(wnd, CursorEnterCallback);

        glfwSetCursorPosCallback(wnd, [](GLFWwindow* wnd, double x, double y) {
            EngineBase1::Instance().OnMouseMove(EmscriptenMouseEvent{ .targetX = (long)x, .targetY = (long)y });
        });

        glfwSetMouseButtonCallback(wnd, [](GLFWwindow* wnd, int button, int action, int mods) {
            // mods 0b 0011 win alt ctrl shift
            // action 1: down  0: up
            // button: MouseButtons enum
            if (action) {
                EngineBase1::Instance().OnMouseDown(EmscriptenMouseEvent{ .button = (uint16_t)button });
            } else {
                EngineBase1::Instance().OnMouseUp(EmscriptenMouseEvent{ .button = (uint16_t)button });
            }
        });

        glfwSetFramebufferSizeCallback(wnd, [](GLFWwindow* wnd, int w, int h) {
            gEngine->SetWindowSize((float)w, (float)h);
        });

        int w{}, h{};
        glfwGetFramebufferSize(wnd, &w, &h);
        gEngine->SetWindowSize((float)w, (float)h);

        glfwMakeContextCurrent(wnd);

        glfwSetInputMode(wnd, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
        glfwSwapInterval(0);	// no v-sync by default

        xx_assert(gladLoadGL(glfwGetProcAddress));

        // dump & cleanup glfw3 error
        while (auto e = glGetError()) {
            xx::CoutN("glGetError() == ", (int)e);
        };

        glEnable(GL_PRIMITIVE_RESTART);
        glPrimitiveRestartIndex(65535);
        glPointSize(5);
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
#endif

        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);

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

    /*****************************************************************************************************/
    /*****************************************************************************************************/

    XX_FORCE_INLINE void GLUpdateBegin() {
        GLViewport();
        GLClear(clearColor);
        blend = blendDefault;
        glBlendFunc(blendDefault[0], blendDefault[1]);
        glBlendEquation(blendDefault[2]);

        Shader::ClearCounter();
    }
    XX_FORCE_INLINE void GLUpdateEnd() {
        ShaderEnd();
    }

    /*****************************************************************************************************/
    /*****************************************************************************************************/

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

    /*****************************************************************************************************/
    /*****************************************************************************************************/

    std::array<uint32_t, 3> blendDefault{ GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_FUNC_ADD };
    std::array<uint32_t, 3> blend{ blendDefault };

    template<bool autoEndShader = true>
    void GLBlendFunc(std::array<uint32_t, 3> args) {
        if (blend != args) {
            if constexpr (autoEndShader) {
                ShaderEnd();
            }
            blend = args;
            glBlendFunc(args[0], args[1]);
            glBlendEquation(args[2]);
        }
    }

    /*****************************************************************************************************/
    /*****************************************************************************************************/

    template<bool autoDecompress = false>
    xx::Ref<GLTexture> LoadTexture(std::string_view const& fn) {
        auto [d, p] = LoadFileData<autoDecompress>(fn);
        xx_assert(d);
        return xx::MakeRef<GLTexture>(LoadGLTexture(d, p));
    }

    template<bool autoDecompress = false>
    xx::Ref<Frame> LoadFrame(std::string_view const& fn) {
        return Frame::Create(LoadTexture<autoDecompress>(fn));
    }

    template<bool autoDecompress = false>
    xx::Ref<TexturePacker> LoadTexturePacker(std::string_view const& fn) {
        auto [blistData, fp] = LoadFileData<autoDecompress>(fn);
        xx_assert(blistData);

        auto tp = xx::MakeRef<TexturePacker>();
        int r = tp->Load(blistData, fp);
        xx_assert(!r);

        auto tex = LoadTexture<autoDecompress>(tp->realTextureFileName);
        xx_assert(tex);

        for (auto& f : tp->frames) {
            f->tex = tex;
        }
        return tp;
    }

    template<bool autoDecompress = false>
    xx::Ref<TMX::Map> LoadTiledMap(char const* bmxPath, std::string root/* = "res/"*/) {
        auto map = xx::MakeRef<TMX::Map>();
        {
            auto [d, fp] = LoadFileData<autoDecompress>(bmxPath);
            xx_assert(d);
            xx::TmxData td(std::move(d));
            auto r = td.Read(*map);
            xx_assert(!r);
        }
        for (auto& img : map->images) {
            img->texture = LoadTexture(root + img->source);
        }
        map->FillExts();
        return map;
    }

    // more load here ?

    /*****************************************************************************************************/
    /*****************************************************************************************************/

    // for sort
    xx::List<ZNode> tmpZNodes;

    // for easy draw root node only
    void DrawNode(Node* tar) {
        FillZNodes(tmpZNodes, tar);
        OrderByZDrawAndClear(tmpZNodes);
    }

    /*****************************************************************************************************/
    /*****************************************************************************************************/


    EM_BOOL OnMouseDown(EmscriptenMouseEvent const& e) {
        touchMode = false;
        mouse.btnStates[e.button] = true;	// mouse left btn == 0, right btn == 2( js )
        assert(!mouseEventHandler);
        mouseEventHandlers.ForeachPoint(mouseEventHandlers.max_2 + mouse.pos, [&](auto o) {
            tmpZNodes.Emplace(o->z, o);
        });
        std::sort(tmpZNodes.buf, tmpZNodes.buf + tmpZNodes.len, ZNode::GreaterThanComparer);	// event big z first
        for (auto& zn : tmpZNodes) {
            if (!((MouseEventHandlerNode*)zn.n)->PosInScissor(mouse.pos)) continue;
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
                if (!((MouseEventHandlerNode*)zn.n)->PosInScissor(mouse.pos)) continue;
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
        keyboardKeysStates[e.which] = true;
        return EM_TRUE;
    }
    EM_BOOL OnKeyUp(EmscriptenKeyboardEvent const& e) {
        keyboardKeysStates[e.which] = false;
        return EM_TRUE;
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
