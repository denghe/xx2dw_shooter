#pragma once
#include "engine_base1.h"
#include "engine_chartexcache.h"
#include "engine_tiledmap_sede.h"
#include "engine_fpsviewer.h"
#include "engine_linestrip.h"
#include "engine_dynamictexturepacker.h"
#include "engine_scale9sprite.h"
#include "engine_camera.h"
#include "engine_node.h"
#include "engine_node_derived.h"

struct EngineBase2 : EngineBase1 {
    XX_FORCE_INLINE static EngineBase2& Instance() { return *(EngineBase2*)gEngine; }

    CharTexCache<24> ctcDefault;
    FpsViewer fpsViewer;
    bool showFps{ true };


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
        constexpr const float SQR2_2 = 1.4142135623730950488016887242097 / 2;

        if (n == 2) {
            if (flags.w) {
                md = MoveDirections::Up;
                if (flags.d) {
                    (int&)md |= (int)MoveDirections::Right;
                    v = { SQR2_2, -SQR2_2 };	// up right
                } else if (flags.a) {
                    (int&)md |= (int)MoveDirections::Left;
                    v = { -SQR2_2, -SQR2_2 };	// up left
                }
            } else if (flags.s) {
                md = MoveDirections::Down;
                if (flags.d) {
                    (int&)md |= (int)MoveDirections::Right;
                    v = { SQR2_2, SQR2_2 };		// right down
                } else if (flags.a) {
                    (int&)md |= (int)MoveDirections::Left;
                    v = { -SQR2_2, SQR2_2 };	// left down
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
