﻿#pragma once
#include <engine_base3.h>

// Derived need inherit from gDesign
template<typename Derived>
struct Engine : EngineBase3 {

    void Init() {

        auto u8s = std::filesystem::absolute("./").u8string();
        rootPath = ToSearchPath((std::string&)u8s);
        SearchPathReset();

        framePerSeconds = ((Derived*)this)->fps;
        this->SetWindowSize(((Derived*)this)->width, ((Derived*)this)->height);
        mouseEventHandlers.Init(128, 128, (int)this->windowSize.x * 2, (int)this->windowSize.y * 2);

        GLInit();

        if constexpr (Has_OnMouseDown<Derived> || Has_OnMouseUp<Derived>) {
            static_assert(Has_OnMouseDown<Derived> && Has_OnMouseUp<Derived>);

            glfwSetMouseButtonCallback(wnd, [](GLFWwindow* wnd, int button, int action, int mods) {
                if (action) {
                    ((Derived*)gEngine)->OnMouseDown(EmscriptenMouseEvent{ .button = (uint16_t)button });
                } else {
                    ((Derived*)gEngine)->OnMouseUp(EmscriptenMouseEvent{ .button = (uint16_t)button });
                }
            });
        }

        if constexpr (Has_OnMouseMove<Derived>) {
            glfwSetCursorPosCallback(wnd, [](GLFWwindow* wnd, double x, double y) {
                ((Derived*)gEngine)->OnMouseMove(EmscriptenMouseEvent{ .targetX = (long)x, .targetY = (long)y });
            });
        }

        if constexpr (Has_AfterInit<Derived>) {
            ((Derived*)this)->AfterInit();
        }

        tasks.Add([this]()->xx::Task<> {
            ctcDefault.Init();
            co_return;
            });

        if constexpr (Has_MainTask<Derived>) {
            tasks.Add(((Derived*)this)->MainTask());
        }
    }

    void Run() {
        xx_assert(framePerSeconds);

        while (!glfwWindowShouldClose(wnd) && running) {
            glfwPollEvents();

            auto s = xx::NowSteadyEpochSeconds();
            delta = s - nowSecs;
            nowSecs = s;

            GLUpdateBegin();

            if (delta > maxFrameDelay) {
                delta = maxFrameDelay;
            }
            timePool += delta;
            while (timePool >= ((Derived*)this)->frameDelay) {
                timePool -= ((Derived*)this)->frameDelay;
                ++frameNumber;
                fpsViewer.Update();
                if constexpr (Has_Update<Derived>) {
                    ((Derived*)this)->Update();
                }
                tasks();
            }
            if constexpr (Has_Draw<Derived>) {
                ((Derived*)this)->Draw();
            }
            if (showFps) {
                fpsViewer.Draw(ctcDefault);
            }

            GLUpdateEnd();

            glfwSwapBuffers(wnd);
        }
    }

};
