#pragma once
#include "engine_includes.h"
#include "engine_opengl.h"
#include "engine_prims.h"
#include "engine_shader.h"
#include "engine_shader_quad.h"
#include "engine_shader_linestrip.h"
#include "engine_frame.h"
#include "engine_texturepacker.h"

#include "engine_rnd.h"
#include "engine_zstd.h"
#include "engine_js_funcs.h"

#include "engine_base0.h"

#include "engine_camera.h"
#include "engine_node.h"
#include "engine_node_derived.h"

struct EngineBase1 : EngineBase0 {
    XX_FORCE_INLINE static EngineBase1& Instance() { return *(EngineBase1*)gEngine; }

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE glContext;

    Shader_QuadInstance shaderQuadInstance;
    Shader_LineStrip shaderLineStrip;
    // ... more


    void GLInit() {
        emscripten_set_canvas_element_size("canvas", (int)windowWidth, (int)windowHeight);
        EmscriptenWebGLContextAttributes attrs;
        emscripten_webgl_init_context_attributes(&attrs);
        attrs.alpha = 0;
        attrs.majorVersion = 2;
        glContext = emscripten_webgl_create_context("canvas", &attrs);
        xx_assert(glContext);
        emscripten_webgl_make_context_current(glContext);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        shaderQuadInstance.Init(this);
        shaderLineStrip.Init(this);
        // ... more
    }

    XX_FORCE_INLINE void GLViewport() {
        glViewport(0, 0, (int)windowWidth, (int)windowHeight);
    }

    XX_FORCE_INLINE void GLClear(RGBA8 c) {
        glClearColor(c.r / 255.f, c.g / 255.f, c.b / 255.f, c.a / 255.f);
        //glClear(GL_COLOR_BUFFER_BIT);
        glDepthMask(true);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDepthMask(false);
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
            if (shader) {
                shader->End();
            }
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
        mouse.pos = { (float)e.targetX - this->windowWidth_2, this->windowHeight - (float)e.targetY - this->windowHeight_2 };
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
        if (this->keyboardKeysDelays[(KeyboardKeys_t)k] > this->nowSecs) return false;
        this->keyboardKeysDelays[(KeyboardKeys_t)k] = this->nowSecs + delaySecs;
        return true;
    }

    std::optional<std::pair<MoveDirections, XY>> GetKeyboardMoveInc() {
        union Dirty {
            struct {
                union {
                    struct {
                        uint8_t a, d;
                    };
                    uint16_t ad;
                };
                union {
                    struct {
                        uint8_t s, w;
                    };
                    uint16_t sw;
                };
            };
            uint32_t all{};
        } flags;
        int32_t n = 0;

        if (KeyDown(KeyboardKeys::A)) { flags.a = 255; ++n; }
        if (KeyDown(KeyboardKeys::S)) { flags.s = 255; ++n; }
        if (KeyDown(KeyboardKeys::D)) { flags.d = 255; ++n; }
        if (KeyDown(KeyboardKeys::W)) { flags.w = 255; ++n; }

        if (n > 2) {
            if (flags.ad > 255 << 8) {
                flags.ad = 0;
                n -= 2;
            }
            if (flags.sw > 255 << 8) {
                flags.sw = 0;
                n -= 2;
            }
        }
        if (!n) return {};

        XY v{};
        MoveDirections md;
        constexpr const float SQR2 = 1.4142135623730950488016887242097;

        if (n == 2) {
            if (flags.w) {
                md = MoveDirections::Up;
                if (flags.d) {
                    (int&)md |= (int)MoveDirections::Right;
                    v = { SQR2, -SQR2 };	// up right
                } else if (flags.a) {
                    (int&)md |= (int)MoveDirections::Left;
                    v = { -SQR2, -SQR2 };	// up left
                }
            } else if (flags.s) {
                md = MoveDirections::Down;
                if (flags.d) {
                    (int&)md |= (int)MoveDirections::Right;
                    v = { SQR2, SQR2 };		// right down
                } else if (flags.a) {
                    (int&)md |= (int)MoveDirections::Left;
                    v = { -SQR2, SQR2 };	// left down
                }
            }
        } else if (n == 1) {
            if (flags.w) {
                md = MoveDirections::Up;
                v.y = -1;	// up
            } else if (flags.s) {
                md = MoveDirections::Down;
                v.y = 1;	// down
            } else if (flags.a) {
                md = MoveDirections::Left;
                v.x = -1;	// left
            } else if (flags.d) {
                md = MoveDirections::Right;
                v.x = 1;	// right
            }
        }

        return std::make_pair(md, v);
    }

};
