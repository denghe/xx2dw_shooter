#pragma once
#include <engine_base3.h>

// Derived need inherit from gDesign
template<typename Derived>
struct Engine : EngineBase3 {

    void EngineInit() {

        // EM_BOOL OnKeyXXXXXXXXXXX(EmscriptenKeyboardEvent const& e) { return EM_TRUE; }

        if constexpr (Has_OnKeyPress<Derived>) {
            emscripten_set_keypress_callback("body", this, true, [](int, const EmscriptenKeyboardEvent* e, void* ud)->EM_BOOL {
                return ((Derived*)ud)->OnKeyPress(*e);
            });
        }
        if constexpr (Has_OnKeyDown<Derived>) {
            emscripten_set_keydown_callback("body", this, true, [](int, const EmscriptenKeyboardEvent* e, void* ud)->EM_BOOL {
                return ((Derived*)ud)->OnKeyDown(*e);
            });
        }
        if constexpr (Has_OnKeyUp<Derived>) {
            emscripten_set_keyup_callback("body", this, true, [](int, const EmscriptenKeyboardEvent* e, void* ud)->EM_BOOL {
                return ((Derived*)ud)->OnKeyUp(*e);
            });
        }

        // EM_BOOL OnMouseXXXXXXXXXXX(EmscriptenMouseEvent const& e) { return EM_TRUE; }

        emscripten_set_mousedown_callback("canvas", this, true, [](int, const EmscriptenMouseEvent* e, void* ud)->EM_BOOL {
            if constexpr (Has_OnMouseDown<Derived>) {
                return ((Derived*)ud)->OnMouseDown(*e);
            } else {
                return ((Derived*)ud)->OnMouseDown_(*e);
            }
        });
        emscripten_set_mouseup_callback("canvas", this, true, [](int, const EmscriptenMouseEvent* e, void* ud)->EM_BOOL {
            if constexpr (Has_OnMouseUp<Derived>) {
                return ((Derived*)ud)->OnMouseUp(*e);
            } else {
                return ((Derived*)ud)->OnMouseUp_(*e);
            }
        });
        if constexpr (Has_OnClick<Derived>) {
            emscripten_set_click_callback("canvas", this, true, [](int, const EmscriptenMouseEvent* e, void* ud)->EM_BOOL {
                return ((Derived*)ud)->OnClick(*e);
            });
        }
        if constexpr (Has_OnDblClick<Derived>) {
            emscripten_set_dblclick_callback("canvas", this, true, [](int, const EmscriptenMouseEvent* e, void* ud)->EM_BOOL {
                return ((Derived*)ud)->OnDblClick(*e);
            });
        }
        emscripten_set_mousemove_callback("canvas", this, true, [](int, const EmscriptenMouseEvent* e, void* ud)->EM_BOOL {
            if constexpr (Has_OnMouseMove<Derived>) {
                return ((Derived*)ud)->OnMouseMove(*e);
            } else {
                return ((Derived*)ud)->OnMouseMove_(*e);
            }
        });
        if constexpr (Has_OnMouseEnter<Derived>) {
            emscripten_set_mouseenter_callback("canvas", this, true, [](int, const EmscriptenMouseEvent* e, void* ud)->EM_BOOL {
                return ((Derived*)ud)->OnMouseEnter(*e);
            });
        }
        if constexpr (Has_OnMouseLeave<Derived>) {
            emscripten_set_mouseleave_callback("canvas", this, true, [](int, const EmscriptenMouseEvent* e, void* ud)->EM_BOOL {
                return ((Derived*)ud)->OnMouseLeave(*e);
            });
        }
        if constexpr (Has_OnMouseOver<Derived>) {
            emscripten_set_mouseover_callback("canvas", this, true, [](int, const EmscriptenMouseEvent* e, void* ud)->EM_BOOL {
                return ((Derived*)ud)->OnMouseOver(*e);
            });
        }
        if constexpr (Has_OnMouseOut<Derived>) {
            emscripten_set_mouseout_callback("canvas", this, true, [](int, const EmscriptenMouseEvent* e, void* ud)->EM_BOOL {
                return ((Derived*)ud)->OnMouseOut(*e);
            });
        }

        // EM_BOOL OnTouchXXXXXXXXXXX(EmscriptenTouchEvent const& e) { return EM_TRUE; }

        if constexpr (Has_OnTouchStart<Derived>) {
            emscripten_set_touchstart_callback("canvas", this, true, [](int, const EmscriptenTouchEvent* e, void* ud)->EM_BOOL {
                return ((Derived*)ud)->OnTouchStart(*e);
            });
        }
        if constexpr (Has_OnTouchMove<Derived>) {
            emscripten_set_touchmove_callback("canvas", this, true, [](int, const EmscriptenTouchEvent* e, void* ud)->EM_BOOL {
                return ((Derived*)ud)->OnTouchMove(*e);
            });
        }
        if constexpr (Has_OnTouchEnd<Derived>) {
            emscripten_set_touchend_callback("canvas", this, true, [](int, const EmscriptenTouchEvent* e, void* ud)->EM_BOOL {
                return ((Derived*)ud)->OnTouchEnd(*e);
            });
        }
        if constexpr (Has_OnTouchCancel<Derived>) {
            emscripten_set_touchcancel_callback("canvas", this, true, [](int, const EmscriptenTouchEvent* e, void* ud)->EM_BOOL {
                return ((Derived*)ud)->OnTouchCancel(*e);
            });
        }

        // todo:
        // emscripten_set_wheel_callback(target, userData, useCapture, callback)             
        // emscripten_set_resize_callback(target, userData, useCapture, callback)            
        // emscripten_set_scroll_callback(target, userData, useCapture, callback)            
        // emscripten_set_blur_callback(target, userData, useCapture, callback)              
        // emscripten_set_focus_callback(target, userData, useCapture, callback)             
        // emscripten_set_focusin_callback(target, userData, useCapture, callback)           
        // emscripten_set_focusout_callback(target, userData, useCapture, callback)          
        // emscripten_set_deviceorientation_callback(userData, useCapture, callback)         
        // emscripten_set_devicemotion_callback(userData, useCapture, callback)              
        // emscripten_set_orientationchange_callback(userData, useCapture, callback)         
        // emscripten_set_fullscreenchange_callback(target, userData, useCapture, callback)  
        // emscripten_set_pointerlockchange_callback(target, userData, useCapture, callback) 
        // emscripten_set_pointerlockerror_callback(target, userData, useCapture, callback)  
        // emscripten_set_visibilitychange_callback(userData, useCapture, callback)          
        // emscripten_set_gamepadconnected_callback(userData, useCapture, callback)          
        // emscripten_set_gamepaddisconnected_callback(userData, useCapture, callback)       
        // emscripten_set_batterychargingchange_callback(userData, callback)                 
        // emscripten_set_batterylevelchange_callback(userData, callback)              
        // emscripten_set_beforeunload_callback(userData, callback)                    


        if constexpr (Has_Init<Derived>) {
            ((Derived*)this)->Init();
        }

        framePerSeconds = ((Derived*)this)->fps;
        this->SetWindowSize(((Derived*)this)->width, ((Derived*)this)->height);
        mouseEventHandlers.Init(128, 128, (int)this->windowSize.x * 2, (int)this->windowSize.y * 2);

        GLInit();
        // todo: remap event handler
        GLInit2();

        if constexpr(Has_AfterInit<Derived>) {
            ((Derived*)this)->AfterInit();
        }

        tasks.Add([this]()->xx::Task<> {
            ctcDefault.Init();
            co_return;
        });

        if constexpr (Has_MainTask<Derived>) {
            tasks.Add(((Derived*)this)->MainTask());
        }

        if constexpr (Has_DrawTask<Derived>) {
            drawTask = ((Derived*)this)->DrawTask();
        }
    }

    /*
int main() {
    gLooper.EngineInit();
    emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
        return ???????????????.JsLoopCallback(ms);
    }, nullptr);
}
    */
    EM_BOOL JsLoopCallback(double ms) {
        (void)ms;
        auto s = xx::NowSteadyEpochSeconds();
        delta = s - nowSecs;
        nowSecs = s;

        GLUpdateBegin();

        if (delta > ((Derived*)this)->maxFrameDelay) {
            delta = ((Derived*)this)->maxFrameDelay;
        }

        if constexpr (Has_BeforeUpdate<Derived>) {
            ((Derived*)this)->BeforeUpdate();
        }

        timePool += delta;
        while (timePool >= ((Derived*)this)->frameDelay) {
            timePool -= ((Derived*)this)->frameDelay;
            ++frameNumber;
            fpsViewer.Update();
            if constexpr(Has_Update<Derived>) {
                ((Derived*)this)->Update();
            }
            tasks();
        }

        if constexpr(Has_Draw<Derived>) {
            ((Derived*)this)->Draw();
        }

        if constexpr (Has_DrawTask<Derived>) {
            drawTask();
        }

        if (showFps) {
            fpsViewer.Draw(ctcDefault);
        }

        GLUpdateEnd();
        return running;
    }

};
