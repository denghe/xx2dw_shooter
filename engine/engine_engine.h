#pragma once
#include <engine_base3.h>

template<typename T> concept Has_Init = requires(T t) { { t.Init() } -> std::same_as<void>; };
template<typename T> concept Has_AfterInit = requires(T t) { { t.AfterInit() } -> std::same_as<void>; };
template<typename T> concept Has_Update = requires(T t) { { t.Update() } -> std::same_as<void>; };
template<typename T> concept Has_Draw = requires(T t) { { t.Draw() } -> std::same_as<void>; };
template <typename T> concept Has_MainTask = requires(T t) { { t.MainTask() } -> std::same_as<xx::Task<>>; };

template <typename T> concept Has_OnKeyPress = requires(T t) { { t.OnKeyPress(std::declval<EmscriptenKeyboardEvent const&>()) } -> std::same_as<EM_BOOL>; };
template <typename T> concept Has_OnKeyDown = requires(T t) { { t.OnKeyDown(std::declval<EmscriptenKeyboardEvent const&>()) } -> std::same_as<EM_BOOL>; };
template <typename T> concept Has_OnKeyUp = requires(T t) { { t.OnKeyUp(std::declval<EmscriptenKeyboardEvent const&>()) } -> std::same_as<EM_BOOL>; };

template <typename T> concept Has_OnMouseDown = requires(T t) { { t.OnMouseDown(std::declval<EmscriptenMouseEvent const&>()) } -> std::same_as<EM_BOOL>; };
template <typename T> concept Has_OnMouseUp = requires(T t) { { t.OnMouseUp(std::declval<EmscriptenMouseEvent const&>()) } -> std::same_as<EM_BOOL>; };
template <typename T> concept Has_OnClick = requires(T t) { { t.OnClick(std::declval<EmscriptenMouseEvent const&>()) } -> std::same_as<EM_BOOL>; };
template <typename T> concept Has_OnDblClick = requires(T t) { { t.OnDblClick(std::declval<EmscriptenMouseEvent const&>()) } -> std::same_as<EM_BOOL>; };
template <typename T> concept Has_OnMouseMove = requires(T t) { { t.OnMouseMove(std::declval<EmscriptenMouseEvent const&>()) } -> std::same_as<EM_BOOL>; };
template <typename T> concept Has_OnMouseEnter = requires(T t) { { t.OnMouseEnter(std::declval<EmscriptenMouseEvent const&>()) } -> std::same_as<EM_BOOL>; };
template <typename T> concept Has_OnMouseLeave = requires(T t) { { t.OnMouseLeave(std::declval<EmscriptenMouseEvent const&>()) } -> std::same_as<EM_BOOL>; };
template <typename T> concept Has_OnMouseOver = requires(T t) { { t.OnMouseOver(std::declval<EmscriptenMouseEvent const&>()) } -> std::same_as<EM_BOOL>; };
template <typename T> concept Has_OnMouseOut = requires(T t) { { t.OnMouseOut(std::declval<EmscriptenMouseEvent const&>()) } -> std::same_as<EM_BOOL>; };

template <typename T> concept Has_OnTouchStart = requires(T t) { { t.OnTouchStart(std::declval<EmscriptenTouchEvent const&>()) } -> std::same_as<EM_BOOL>; };
template <typename T> concept Has_OnTouchMove = requires(T t) { { t.OnTouchMove(std::declval<EmscriptenTouchEvent const&>()) } -> std::same_as<EM_BOOL>; };
template <typename T> concept Has_OnTouchEnd = requires(T t) { { t.OnTouchEnd(std::declval<EmscriptenTouchEvent const&>()) } -> std::same_as<EM_BOOL>; };
template <typename T> concept Has_OnTouchCancel = requires(T t) { { t.OnTouchCancel(std::declval<EmscriptenTouchEvent const&>()) } -> std::same_as<EM_BOOL>; };

// Derived need inherit from gDesign
template<typename Derived>
struct Engine : EngineBase3 {

    Engine() {

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

        if constexpr (Has_OnMouseDown<Derived>) {
            emscripten_set_mousedown_callback("canvas", this, true, [](int, const EmscriptenMouseEvent* e, void* ud)->EM_BOOL {
                return ((Derived*)ud)->OnMouseDown(*e);
            });
        }
        if constexpr (Has_OnMouseUp<Derived>) {
            emscripten_set_mouseup_callback("canvas", this, true, [](int, const EmscriptenMouseEvent* e, void* ud)->EM_BOOL {
                return ((Derived*)ud)->OnMouseUp(*e);
            });
        }
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
        if constexpr (Has_OnMouseMove<Derived>) {
            emscripten_set_mousemove_callback("canvas", this, true, [](int, const EmscriptenMouseEvent* e, void* ud)->EM_BOOL {
                return ((Derived*)ud)->OnMouseMove(*e);
            });
        }
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

        windowWidth = ((Derived*)this)->width;
        windowHeight = ((Derived*)this)->height;
        windowWidth_2 = ((Derived*)this)->width / 2;
        windowHeight_2 = ((Derived*)this)->height / 2;
        framePerSeconds = ((Derived*)this)->fps;
        mouseEventHandlers.Init(128, 128, ((Derived*)this)->width * 2, ((Derived*)this)->height * 2);

        GLInit();

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
    }

    /*
int main() {
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
        if (showFps) {
            fpsViewer.Draw(ctcDefault);
        }

        GLUpdateEnd();
        return running;
    }

    // task utils
    xx::Task<> AsyncSleep(double secs) {
        auto e = nowSecs + secs;
        do {
            co_yield 0;
        } while (nowSecs < e);
    }

    template<bool showLog = false, int timeoutSeconds = 30>
    xx::Task<xx::Shared<GLTexture>> AsyncLoadTextureFromUrl(char const* url) {
        if constexpr(showLog) {
            printf("LoadTextureFromUrl( %s ) : begin. nowSecs = %f\n", url, nowSecs);
        }
        auto i = GLGenTextures<true>();
        int tw{}, th{};
        load_texture_from_url(i, url, &tw, &th);
        auto elapsedSecs = nowSecs + timeoutSeconds;
        while (nowSecs < elapsedSecs) {
            co_yield 0;
            if (tw) {
                if constexpr(showLog) {
                    printf("LoadTextureFromUrl( %s ) : loaded. nowSecs = %f, size = %d, %d\n", url, nowSecs, tw, th);
                }
                co_return xx::Make<GLTexture>(i, tw, th, url);
            }
        }
        if constexpr(showLog) {
            printf("LoadTextureFromUrl( %s ) : timeout. timeoutSeconds = %d\n", url, timeoutSeconds);
        }
        co_return xx::Shared<GLTexture>{};
    }

    template<bool showLog = false, int timeoutSeconds = 30>
    xx::Task<std::vector<xx::Shared<GLTexture>>> AsyncLoadTexturesFromUrls(std::initializer_list<char const*> urls) {
        std::vector<xx::Shared<GLTexture>> rtv;
        rtv.resize(urls.size());
        size_t counter = 0;
        for (size_t i = 0; i < urls.size(); ++i) {
            tasks.Add([this, &counter, tar = &rtv[i], url = *(urls.begin() + i)]()->xx::Task<> {
                *tar = co_await AsyncLoadTextureFromUrl<showLog, timeoutSeconds>(url);
                ++counter;
            });
        }
        while (counter < urls.size()) co_yield 0; // wait all
        co_return rtv;
    }

    // todo: timeout support
    template<bool autoDecompress = false>
    xx::Task<xx::Shared<xx::Data>> AsyncDownloadFromUrl(char const* url) {
        emscripten_fetch_attr_t attr;
        emscripten_fetch_attr_init(&attr);
        strcpy(attr.requestMethod, "GET");
        attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;

        using UD = std::pair<bool*, xx::Shared<xx::Data>*>;

        attr.onsuccess = [](emscripten_fetch_t* fetch) {
            UD& ud = *(UD*)fetch->userData;
            *ud.first = true;
            ud.second->Emplace()->WriteBuf(fetch->data, fetch->numBytes);
            emscripten_fetch_close(fetch);
        };

        attr.onerror = [](emscripten_fetch_t* fetch) {
            UD& ud = *(UD*)fetch->userData;
            *ud.first = true;
            emscripten_fetch_close(fetch);
        };

        xx::Shared<xx::Data> sd;
        bool callbacked = false;
        UD ud = { &callbacked, &sd };
        attr.userData = &ud;

        emscripten_fetch(&attr, url);
        
        while (!callbacked) {
            co_yield 0;
        }

        if constexpr (autoDecompress) {
            TryZstdDecompress(*sd);
        }
        co_return sd;
    }

    // blist == texture packer export cocos plist file's bin version, use xx2d's tools: plist 2 blist convert
    template<bool autoDecompress = false>
    xx::Task<xx::Shared<TexturePacker>> AsyncLoadTexturePackerFromUrl(char const* blistUrl) {
        auto blistData = co_await AsyncDownloadFromUrl<autoDecompress>(blistUrl);
        if (!blistData) co_return xx::Shared<TexturePacker>{};

        auto tp = xx::Make<TexturePacker>();
        int r = tp->Load(*blistData, blistUrl);
        xx_assert(!r);

        auto tex = co_await AsyncLoadTextureFromUrl<autoDecompress>(tp->realTextureFileName.c_str());
        xx_assert(tex);

        for (auto& f : tp->frames) {
            f->tex = tex;
        }
        co_return tp;
    }

    // bmx == tiledmap editor store tmx file's bin version, use xx2d's tools: tmx 2 bmx convert
    template<bool autoDecompress = false>
    xx::Task<xx::Shared<TMX::Map>> AsyncLoadTiledMapFromUrl(char const* bmxUrl, std::string root = "res/") {
        auto map = xx::Make<TMX::Map>();
        // download bmx & fill
        {
            auto sd = co_await AsyncDownloadFromUrl<autoDecompress>(bmxUrl);
            xx_assert(sd);

            xx::TmxData td;
            td = std::move(*sd);
            auto r = td.Read(*map);
            xx_assert(!r);
        }
        // download textures
        auto n = map->images.size();
        for (auto& img : map->images) {
            tasks.Add([this, &n, img = img, url = root + img->source]()->xx::Task<> {
                img->texture = co_await AsyncLoadTextureFromUrl(url.c_str());
                --n;
                //printf("url loaded: %s\n", url.c_str());
            });
        }
        while (n) co_yield 0;	// wait all

        // fill ext data for easy use
        map->FillExts();

        co_return map;
    }
};
