#pragma once
#include <engine_base1.h>
#include <engine_framebuffer.h>
#include <engine_frame.h>
#include <engine_quad.h>

template<int charSize_ = 24, int canvasWidth_ = int(charSize_ / 0.75), int canvasHeight_ = int(charSize_ / 0.75), int texWidth_ = 2048, int texHeight_ = 2048>
struct CharTexCache {
    static constexpr int charSize = charSize_, canvasWidth = canvasWidth_, canvasHeight = canvasHeight_, texWidth = texWidth_, texHeight = texHeight_;
    static constexpr int canvasWidth_2 = canvasWidth / 2, canvasHeight_2 = canvasHeight / 2;
    static constexpr float padding = 1;
    std::vector<xx::Ref<GLTexture>> texs;
    Quad cq;
    FrameBuffer fb;
    XY p{ 0, texHeight - 1 };

    std::array<TinyFrame, 256> bases;
    std::unordered_map<char32_t, TinyFrame> extras;

    // need ogl frame env
    void Init(char const* font = "Arial") {
        fb.Init();
        init_gCanvas(charSize, canvasWidth, canvasHeight, font);

        texs.emplace_back(FrameBuffer::MakeTexture({ texWidth, texHeight }));
        auto ct = xx::MakeRef<GLTexture>(GLGenTextures<true>(), canvasWidth, canvasHeight, "");
        cq.SetFrame(Frame::Create(std::move(ct))).SetAnchor({ 0, 1 });

        for (char32_t c = 0; c < 256; ++c) {
            MakeCharFrame(c);
        }
    }

    TinyFrame& MakeCharFrame(char32_t c) {
        char buf[16];
        buf[xx::Char32ToUtf8(c, buf)] = '\0';
        glBindTexture(GL_TEXTURE_2D, cq.texId);
#ifndef __EMSCRIPTEN__
        auto [cw, ch] = upload_unicode_char_to_texture(charSize, buf);
#else
        float cw = upload_unicode_char_to_texture(charSize, buf);
        float ch = canvasHeight;
#endif
        glBindTexture(GL_TEXTURE_2D, 0);

        TinyFrame* f{};
        if (c < 256) {
            f = &bases[c];
        } else {
            auto rtv = extras.insert(std::make_pair(c, TinyFrame{}));
            f = &rtv.first->second;
        }

        auto cp = p;
        if (p.x + cw > texWidth) {                  // line wrap
            cp.x = 0;
            p.x = cw + padding;
            if (p.y - canvasHeight < 0) {                     // new page
                texs.emplace_back(FrameBuffer::MakeTexture({ texWidth, texHeight }));
                p.y = cp.y = texHeight - 1;
            } else {                                // new line
                p.y -= canvasHeight;
                cp.y = p.y;
            }
        } else {                                    // current line
            p.x += cw + padding;
        }

        auto& t = texs.back();
        if (cw > 0) {
#ifndef __EMSCRIPTEN__
            cq.texRect.w = (uint16_t)cw;
            cq.texRect.h = (uint16_t)ch;
            fb.DrawTo(t, {}, [&]() {
                cq.SetPosition(cp + XY{ -texWidth / 2, (canvasHeight - ch) / 2 - texHeight / 2 }).Draw();
            });
#else
            fb.DrawTo(t, {}, [&]() {
                cq.SetPosition(cp + XY{ -texWidth / 2, -texHeight / 2 }).Draw();
            });
#endif
        }

        f->tex = t;
        f->texRect.x = uint16_t(cp.x);
        f->texRect.y = uint16_t(texHeight - 1 - cp.y);        // flip y for uv
        f->texRect.w = uint16_t(cw);
        f->texRect.h = uint16_t(ch);
        return *f;
    }

    TinyFrame& Find(char32_t c) {
        if (c < 256) {
            return bases[c];
        } else {
            if (auto iter = extras.find(c); iter != extras.end()) {
                return iter->second;
            } else {
                return MakeCharFrame(c);
            }
        }
    }

    void Draw(XY const& position, XY const& anchor, RGBA8 color, std::u32string_view const& txt) {

        // make sure all char texture exists
        auto fs = (TinyFrame**)alloca(txt.size() * sizeof(TinyFrame*));
        XY size{ 0, canvasHeight };
        auto e = txt.size();
        for (size_t i = 0; i < e; ++i) {
            fs[i] = &Find(txt[i]);
        }
        auto pos = position - size * anchor;
        auto& shader = EngineBase1::Instance().ShaderBegin(EngineBase1::Instance().shaderQuadInstance);
        for (size_t i = 0; i < e; ++i) {
            auto f = fs[i];
            auto& q = *shader.Draw(f->tex->GetValue(), 1);
            q.anchor = { 0.f, 0.f };
            q.color = color;
            q.colorplus = 1;
            q.pos = pos;
            q.radians = {};
            q.scale = { 1, 1 }; // todo
            q.texRect.data = f->texRect.data;

            pos.x += f->texRect.w;
        }
    }

    void Draw(XY const& pos, std::string_view const& s, RGBA8 color = RGBA8_White, XY const& anchor = { 0, 0.5 }) {
        Draw(pos, anchor, color, xx::StringU8ToU32(s));
    }

    float Measure(std::u32string_view const& s) {
        float w{};
        for (size_t i = 0; i < s.size(); ++i) {
            w += Find(s[i]).texRect.w;
        }
        return w;
    }

    float Measure(std::string_view const& s) {
        return Measure(xx::StringU8ToU32(s));
    }

};
