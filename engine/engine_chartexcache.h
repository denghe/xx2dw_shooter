#pragma once
#include "engine_base.h"
#include "engine_framebuffer.h"
#include "engine_frame.h"
#include "engine_quad.h"

template<int charSize_ = 24, int canvasWidth_ = int(charSize_ / 0.75), int canvasHeight_ = int(charSize_ / 0.75), int texWidth_ = 2048, int texHeight_ = 2048>
struct CharTexCache {
    static constexpr int charSize = charSize_, canvasWidth = canvasWidth_, canvasHeight = canvasHeight_, texWidth = texWidth_, texHeight = texHeight_;
    static constexpr int canvasWidth_2 = canvasWidth / 2, canvasHeight_2 = canvasHeight / 2;
    std::vector<xx::Shared<GLTexture>> texs;
    Quad cq;
    FrameBuffer fb;
    float cw{};
    XY p{ 0, texHeight - 1 };

    std::array<TinyFrame, 256> bases;
    std::unordered_map<char32_t, TinyFrame> extras;

    // need ogl frame env
    void Init(char const* font = "Arial") {
        fb.Init();
        init_gCanvas(charSize, canvasWidth, canvasHeight, font);

        texs.emplace_back(FrameBuffer::MakeTexture(Vec2{ texWidth, texHeight }));
        auto ct = xx::Make<GLTexture>(GLGenTextures<true>(), canvasWidth, canvasHeight, "");
        cq.SetFrame(Frame::Create(std::move(ct))).SetAnchor({ 0, 1 });

        char buf[16];
        for (char32_t c = 0; c < 256; ++c) {
            MakeCharInfo(c);
        }
    }

    void FillCharTex(char32_t c) {
        char buf[16];
        buf[xx::Char32ToUtf8(c, buf)] = '\0';
        glBindTexture(GL_TEXTURE_2D, cq.texId);
        cw = upload_unicode_char_to_texture(charSize, buf);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    TinyFrame& MakeCharInfo(char32_t c) {
        FillCharTex(c);
        TinyFrame* ci{};
        if (c < 256) {
            ci = &bases[c];
        } else {
            auto rtv = extras.insert(std::make_pair(c, TinyFrame{}));
            ci = &rtv.first->second;
        }

        auto cp = p;
        if (p.x + cw > texWidth) {                  // line wrap
            cp.x = 0;
            p.x = cw;
            if (p.y - canvasHeight < 0) {                     // new page
                texs.emplace_back(FrameBuffer::MakeTexture(Vec2{ texWidth, texHeight }));
                p.y = cp.y = texHeight - 1;
            } else {                                // new line
                p.y -= canvasHeight;
                cp.y = p.y;
            }
        } else {                                    // current line
            p.x += cw;
        }

        auto& t = texs.back();
        fb.DrawTo(t, {}, [&]() {
            cq.SetPosition(cp + XY{ -texWidth / 2, -texHeight / 2 }).Draw();
        });

        ci->tex = t;
        ci->texRect.x = cp.x;
        ci->texRect.y = texHeight - 1 - cp.y;        // flip y for uv
        ci->texRect.w = cw;
        ci->texRect.h = canvasHeight;
        return *ci;
    }

    TinyFrame& Find(char32_t c) {
        if (c < 256) {
            return bases[c];
        } else {
            if (auto iter = extras.find(c); iter != extras.end()) {
                return iter->second;
            } else {
                return MakeCharInfo(c);
            }
        }
    }

    // anchor: {0, 0.5}   todo: anchor, max width limit ?
    void Draw(XY pos, std::u32string_view const& s, RGBA8 color = { 255,255,255,255 }) {

        // make sure all char texture exists ( avoid framebuffer incomplete issue )
        auto cis = (TinyFrame**)alloca(s.size() * sizeof(void*));
        auto e = s.size();
        for (size_t i = 0; i < e; ++i) {
            cis[i] = &Find(s[i]);
        }

        auto& shader = gEngineBase->ShaderBegin(gEngineBase->shaderQuadInstance);
        for (size_t i = 0; i < e; ++i) {
            auto&& ci = *cis[i];
            auto& q = *shader.Draw(ci.tex->GetValue(), 1);
            q.anchor = { 0.f, 0.5f };
            q.color = color;
            q.colormulti = 1;
            q.pos = pos;
            q.radians = {};
            q.scale = { 1, 1 };
            q.texRect.data = ci.texRect.data;

            pos.x += ci.texRect.w;
        }
    }

    void Draw(XY const& pos, std::string_view const& s, RGBA8 color = {255,255,255,255}) {
        Draw(pos, xx::StringU8ToU32(s), color);
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
