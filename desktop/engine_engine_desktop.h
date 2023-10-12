#pragma once
#include <engine_base3.h>

template<typename T> concept Has_Init = requires(T t) { { t.Init() } -> std::same_as<void>; };
template<typename T> concept Has_AfterInit = requires(T t) { { t.AfterInit() } -> std::same_as<void>; };
template<typename T> concept Has_Update = requires(T t) { { t.Update() } -> std::same_as<void>; };
template<typename T> concept Has_Draw = requires(T t) { { t.Draw() } -> std::same_as<void>; };
template <typename T> concept Has_MainTask = requires(T t) { { t.MainTask() } -> std::same_as<xx::Task<>>; };

// Derived need inherit from gDesign
template<typename Derived>
struct Engine : EngineBase3 {

    Engine() {

        if constexpr (Has_Init<Derived>) {
            ((Derived*)this)->Init();
        }

        framePerSeconds = ((Derived*)this)->fps;
        this->SetWindowSize(((Derived*)this)->width, ((Derived*)this)->height);
        mouseEventHandlers.Init(128, 128, (int)this->windowSize.x * 2, (int)this->windowSize.y * 2);

        GLInit();

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


    // task utils
    xx::Task<> AsyncSleep(double secs) {
        auto e = nowSecs + secs;
        do {
            co_yield 0;
        } while (nowSecs < e);
    }

};
