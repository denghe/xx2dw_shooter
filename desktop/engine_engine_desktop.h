#pragma once
#include "engine_base3.h"

template<typename T> concept Has_Init = requires(T t) { { t.Init() } -> std::same_as<void>; };
template<typename T> concept Has_AfterInit = requires(T t) { { t.AfterInit() } -> std::same_as<void>; };
template<typename T> concept Has_Update = requires(T t) { { t.Update() } -> std::same_as<void>; };
template<typename T> concept Has_Draw = requires(T t) { { t.Draw() } -> std::same_as<void>; };
template <typename T> concept Has_MainTask = requires(T t) { { t.MainTask() } -> std::same_as<xx::Task<>>; };

// need inherit from gDesign
template<typename Derived>
struct Engine : EngineBase3 {

    Engine() {

        if constexpr (Has_Init<Derived>) {
            ((Derived*)this)->Init();
        }

        windowWidth = ((Derived*)this)->width;
        windowHeight = ((Derived*)this)->height;
        windowWidth_2 = ((Derived*)this)->width / 2;
        windowHeight_2 = ((Derived*)this)->height / 2;
        framePerSeconds = ((Derived*)this)->fps;
        mouseEventHandlers.Init(128, 128, ((Derived*)this)->width * 2, ((Derived*)this)->height * 2);

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

    //EM_BOOL JsLoopCallback(double ms) {
    //    (void)ms;
    //    auto s = xx::NowSteadyEpochSeconds();
    //    delta = s - nowSecs;
    //    nowSecs = s;

    //    GLUpdateBegin();

    //    if (delta > ((Derived*)this)->maxFrameDelay) {
    //        delta = ((Derived*)this)->maxFrameDelay;
    //    }
    //    timePool += delta;
    //    while (timePool >= ((Derived*)this)->frameDelay) {
    //        timePool -= ((Derived*)this)->frameDelay;
    //        ++frameNumber;
    //        fpsViewer.Update();
    //        if constexpr (Has_Update<Derived>) {
    //            ((Derived*)this)->Update();
    //        }
    //        tasks();
    //    }
    //    if constexpr (Has_Draw<Derived>) {
    //        ((Derived*)this)->Draw();
    //    }
    //    if (showFps) {
    //        fpsViewer.Draw(ctcDefault);
    //    }

    //    GLUpdateEnd();
    //    return running;
    //}

    // task utils
    xx::Task<> AsyncSleep(double secs) {
        auto e = nowSecs + secs;
        do {
            co_yield 0;
        } while (nowSecs < e);
    }

};
