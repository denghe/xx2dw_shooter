#pragma once
#include <engine_base2.h>
#include <engine_label.h>
#include <engine_richlabel.h>
#include <engine_button.h>

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

struct EngineBase3 : EngineBase2 {
	XX_FORCE_INLINE static EngineBase3& Instance() { return *(EngineBase3*)gEngine; }

    // task utils
    xx::Task<> AsyncSleep(double secs) {
        auto e = nowSecs + secs;
        do {
            co_yield 0;
        } while (nowSecs < e);
    }

#ifdef __EMSCRIPTEN__

    template<bool showLog = false, int timeoutSeconds = 30>
    xx::Task<xx::Ref<GLTexture>> AsyncLoadTextureFromUrl(char const* url) {
        if constexpr (showLog) {
            printf("LoadTextureFromUrl( %s ) : begin. nowSecs = %f\n", url, nowSecs);
        }
        auto i = GLGenTextures<true>();
        int tw{}, th{};
        load_texture_from_url(i, url, &tw, &th);
        auto elapsedSecs = nowSecs + timeoutSeconds;
        while (nowSecs < elapsedSecs) {
            co_yield 0;
            if (tw) {
                if constexpr (showLog) {
                    printf("LoadTextureFromUrl( %s ) : loaded. nowSecs = %f, size = %d, %d\n", url, nowSecs, tw, th);
                }
                co_return xx::MakeRef<GLTexture>(i, tw, th, url);
            }
        }
        if constexpr (showLog) {
            printf("LoadTextureFromUrl( %s ) : timeout. timeoutSeconds = %d\n", url, timeoutSeconds);
        }
        co_return xx::Ref<GLTexture>{};
    }

    template<bool showLog = false, int timeoutSeconds = 30>
    xx::Task<std::vector<xx::Ref<GLTexture>>> AsyncLoadTexturesFromUrls(std::initializer_list<char const*> urls) {
        std::vector<xx::Ref<GLTexture>> rtv;
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

    template<bool showLog = false, int timeoutSeconds = 30>
    xx::Task<xx::Ref<Frame>> AsyncLoadFrameFromUrl(char const* url) {
        co_return Frame::Create(co_await AsyncLoadTextureFromUrl<showLog, timeoutSeconds>(url));
    }

    // todo: timeout support
    template<bool autoDecompress = false>
    xx::Task<xx::Ref<xx::Data>> AsyncDownloadFromUrl(char const* url) {
        emscripten_fetch_attr_t attr;
        emscripten_fetch_attr_init(&attr);
        strcpy(attr.requestMethod, "GET");
        attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;

        using UD = std::pair<bool*, xx::Ref<xx::Data>*>;

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

        xx::Ref<xx::Data> sd;
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
    xx::Task<xx::Ref<TexturePacker>> AsyncLoadTexturePackerFromUrl(char const* blistUrl) {
        auto blistData = co_await AsyncDownloadFromUrl<autoDecompress>(blistUrl);
        if (!blistData) co_return xx::Ref<TexturePacker>{};

        auto tp = xx::MakeRef<TexturePacker>();
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
    xx::Task<xx::Ref<TMX::Map>> AsyncLoadTiledMapFromUrl(char const* bmxUrl, std::string root = "res/") {
        auto map = xx::MakeRef<TMX::Map>();
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
            });
        }
        while (n) co_yield 0;	// wait all

        // fill ext data for easy use
        map->FillExts();

        co_return map;
    }

#else
    // todo
#endif
};
