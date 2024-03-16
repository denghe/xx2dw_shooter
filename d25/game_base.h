#pragma once
#include "looper.h"

struct ItemBase : GridItemBase {
	static constexpr XY cAnchor{ 0.5f, 0.5f };
	static constexpr float cRadius{ 16 };

    virtual void Focus() {};

    ItemBase() = default;
    ItemBase(ItemBase const&) = delete;
    ItemBase& operator=(ItemBase const&) = delete;
    ItemBase(ItemBase &&) = default;
    ItemBase& operator=(ItemBase &&) = default;
    virtual ~ItemBase() {};
};

struct MonsterBase : ItemBase {
	static constexpr double cHP{ 100 };
	static constexpr float cSpeed{ 200.f / gDesign.fps };
	static constexpr RGBA8 cColor{ RGBA8_White };

	double hp{};
	float radius{};
	int32_t mapPathIndex{}, trackIndex{};
	float pointIndex{}, speed{}, radians{};
	// todo: more?
};

struct TowerBase : ItemBase {
	// todo: more?
};

struct BulletBase : ItemBase {
	static constexpr double cDamage{ 100 };
	static constexpr RGBA8 cColor{ RGBA8_White };

	double damage{};
	float radius{};
	float speed{}, radians{};
	int32_t deathFrameNumber{};
	// todo: more?
};










struct RingInstanceData {
    XY pos{};
    float radius{};
    RGBA8 color{ 255, 255, 255, 255 };
};

struct Shader_RingInstance : Shader {
    using Shader::Shader;
    GLint uSwh2{ -1 }, aVert{ -1 }, aPosRadius{ -1 }, aColor{ -1 };
    GLVertexArrays va;
    GLBuffer vb, ib;

    static constexpr int32_t maxRingNums{ 200000 };
    std::unique_ptr<RingInstanceData[]> ringInstanceDatas = std::make_unique<RingInstanceData[]>(maxRingNums);
    int32_t ringCount{};
    EngineBase0* eb{};

    void Init(EngineBase0* eb_) {
        eb = eb_;

        v = LoadGLVertexShader({ R"(#version 300 es
uniform vec2 uSwh2;	// screen width & height / 2

in vec2 aVert;	// fans index { 0, 0 }, { 0, 1.f }, { 1.f, 0 }, { 1.f, 1.f }

in vec3 aPosRadius;
in vec4 aColor;

out vec3 vPosRadius;
out vec4 vColor;

void main() {
    float d = aPosRadius.z * 2.f;
    vec2 v = aPosRadius.xy + vec2((aVert.x - 0.5f) * d, (aVert.y - 0.5f) * d);

    gl_Position = vec4(v / uSwh2, 0, 1);   // -1 ~ 1

    vPosRadius.xy = aPosRadius.xy + uSwh2;   // frag coord
    vPosRadius.z = aPosRadius.z;
    vColor = aColor;
})"sv });

        f = LoadGLFragmentShader({ R"(#version 300 es
precision mediump float;

in vec3 vPosRadius;
in vec4 vColor;

out vec4 oColor;

void main() {
    float r = vPosRadius.z;
    float d = distance(vPosRadius.xy, gl_FragCoord.xy);
    // if (d > r) discard;
    oColor.xyz = vColor.xyz;
    oColor.a = smoothstep(r - 100.f, r, d) * smoothstep(r, r - 1.f, d) * (step(r - 7.f, d) * 0.2f + 0.3f);
})"sv });

        p = LinkGLProgram(v, f);

        uSwh2 = glGetUniformLocation(p, "uSwh2");

        aVert = glGetAttribLocation(p, "aVert");
        aPosRadius = glGetAttribLocation(p, "aPosRadius");
        aColor = glGetAttribLocation(p, "aColor");
        CheckGLError();

        glGenVertexArrays(1, va.GetValuePointer());
        glBindVertexArray(va);

        glGenBuffers(1, (GLuint*)&ib);
        static const XY verts[4] = { { 0, 0 }, { 0, 1.f }, { 1.f, 0 }, { 1.f, 1.f } };  // maybe octagon better performance ?
        glBindBuffer(GL_ARRAY_BUFFER, ib);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
        glVertexAttribPointer(aVert, 2, GL_FLOAT, GL_FALSE, sizeof(XY), 0);
        glEnableVertexAttribArray(aVert);

        glGenBuffers(1, (GLuint*)&vb);
        glBindBuffer(GL_ARRAY_BUFFER, vb);

        glVertexAttribPointer(aPosRadius, 3, GL_FLOAT, GL_FALSE, sizeof(RingInstanceData), 0);
        glVertexAttribDivisor(aPosRadius, 1);
        glEnableVertexAttribArray(aPosRadius);

        glVertexAttribPointer(aColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(RingInstanceData), (GLvoid*)offsetof(RingInstanceData, color));
        glVertexAttribDivisor(aColor, 1);
        glEnableVertexAttribArray(aColor);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        CheckGLError();
    }

    virtual void Begin() override {
        assert(!eb->shader);
        glUseProgram(p);
        glUniform2f(uSwh2, eb->windowSize.x / 2, eb->windowSize.y / 2);
        glBindVertexArray(va);
    }

    virtual void End() override {
        assert(eb->shader == this);
        if (ringCount) {
            Commit();
        }
    }

    void Commit() {
        glBindBuffer(GL_ARRAY_BUFFER, vb);
        glBufferData(GL_ARRAY_BUFFER, sizeof(RingInstanceData) * ringCount, ringInstanceDatas.get(), GL_STREAM_DRAW);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ringCount);
        CheckGLError();

        drawVerts += ringCount * 6;
        drawCall += 1;

        ringCount = 0;
    }

    RingInstanceData* Draw(int32_t numRings) {
        assert(eb->shader == this);
        assert(numRings <= maxRingNums);
        if (ringCount + numRings > maxRingNums) {
            Commit();
        }
        auto r = &ringInstanceDatas[ringCount];
        ringCount += numRings;
        return r;
    }

    inline XX_FORCE_INLINE void Draw(RingInstanceData& rid) {
        memcpy(Draw(1), &rid, sizeof(rid));
    }

    void DrawOne(RingInstanceData& rid) {
        EngineBase1::Instance().ShaderBegin(*this).Draw(rid);
    }

};
