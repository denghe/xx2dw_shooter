#pragma once
#include "engine_shader.h"

struct QuadInstanceData {
    XY pos{}, anchor{ 0.5, 0.5 };       // float * 4

    XY scale{ 1, 1 };
    float radians{}, colorplus{};       // float * 4

    RGBA8 color{ 255, 255, 255, 255 };  // u8n * 4

    UVRect texRect{};    // u16 * 4
};

struct Shader_QuadInstance : Shader {
    using Shader::Shader;
    GLint uCxy{ -1 }, uTex0{ -1 }, aVert{ -1 }, aPosAnchor{ -1 }, aScaleRadiansColorplus{ -1 }, aColor{ -1 }, aTexRect{ -1 };
    GLVertexArrays va;
    GLBuffer vb, ib;

    static const size_t maxQuadNums{ 200000 };
    GLuint lastTextureId{};
    std::unique_ptr<QuadInstanceData[]> quadInstanceDatas = std::make_unique<QuadInstanceData[]>(maxQuadNums);
    size_t quadCount{};
    EngineBase0* eb{};

    void Init(EngineBase0* eb_) {
        eb = eb_;

        v = LoadGLVertexShader({ R"(#version 300 es
uniform vec2 uCxy;	// screen center coordinate

in vec2 aVert;	// fans index { 0, 0 }, { 0, 1.f }, { 1.f, 0 }, { 1.f, 1.f }

in vec4 aPosAnchor;
in vec4 aScaleRadiansColorplus;
in vec4 aColor;
in vec4 aTexRect;

out vec2 vTexCoord;
out float vColorplus;
out vec4 vColor;

void main() {
    vec2 pos = aPosAnchor.xy;
    vec2 anchor = aPosAnchor.zw;
    vec2 scale = vec2(aScaleRadiansColorplus.x * aTexRect.z, aScaleRadiansColorplus.y * aTexRect.w);
    float radians = aScaleRadiansColorplus.z;
    vec2 offset = vec2((aVert.x - anchor.x) * scale.x, (aVert.y - anchor.y) * scale.y);

    float c = cos(radians);
    float s = sin(radians);
    vec2 v = pos + vec2(
       dot(offset, vec2(c, s)),
       dot(offset, vec2(-s, c))
    );

    gl_Position = vec4(v * uCxy, 0, 1);
    vColor = aColor;
    vColorplus = aScaleRadiansColorplus.w;
    vTexCoord = vec2(aTexRect.x + aVert.x * aTexRect.z, aTexRect.y + aTexRect.w - aVert.y * aTexRect.w);
})"sv });

        f = LoadGLFragmentShader({ R"(#version 300 es
precision highp float;
uniform sampler2D uTex0;

in vec2 vTexCoord;
in float vColorplus;
in vec4 vColor;

out vec4 oColor;

void main() {
    vec4 c = vColor * texture(uTex0, vTexCoord / vec2(textureSize(uTex0, 0)));
    oColor = vec4( c.x + vColorplus, c.y + vColorplus, c.z + vColorplus, c.w );
})"sv });

        p = LinkGLProgram(v, f);

        uCxy = glGetUniformLocation(p, "uCxy");
        uTex0 = glGetUniformLocation(p, "uTex0");

        aVert = glGetAttribLocation(p, "aVert");
        aPosAnchor = glGetAttribLocation(p, "aPosAnchor");
        aScaleRadiansColorplus = glGetAttribLocation(p, "aScaleRadiansColorplus");
        aColor = glGetAttribLocation(p, "aColor");
        aTexRect = glGetAttribLocation(p, "aTexRect");
        CheckGLError();

        glGenVertexArrays(1, va.GetValuePointer());
        glBindVertexArray(va);

        glGenBuffers(1, (GLuint*)&ib);
        static const XY verts[4] = { { 0, 0 }, { 0, 1.f }, { 1.f, 0 }, { 1.f, 1.f } };
        glBindBuffer(GL_ARRAY_BUFFER, ib);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
        glVertexAttribPointer(aVert, 2, GL_FLOAT, GL_FALSE, sizeof(XY), 0);
        glEnableVertexAttribArray(aVert);

        glGenBuffers(1, (GLuint*)&vb);
        glBindBuffer(GL_ARRAY_BUFFER, vb);

        glVertexAttribPointer(aPosAnchor, 4, GL_FLOAT, GL_FALSE, sizeof(QuadInstanceData), 0);  // offsetof(QuadInstanceData, pos
        glVertexAttribDivisor(aPosAnchor, 1);
        glEnableVertexAttribArray(aPosAnchor);

        glVertexAttribPointer(aScaleRadiansColorplus, 4, GL_FLOAT, GL_FALSE, sizeof(QuadInstanceData), (GLvoid*)offsetof(QuadInstanceData, scale));
        glVertexAttribDivisor(aScaleRadiansColorplus, 1);
        glEnableVertexAttribArray(aScaleRadiansColorplus);

        glVertexAttribPointer(aColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(QuadInstanceData), (GLvoid*)offsetof(QuadInstanceData, color));
        glVertexAttribDivisor(aColor, 1);
        glEnableVertexAttribArray(aColor);

        glVertexAttribPointer(aTexRect, 4, GL_UNSIGNED_SHORT, GL_FALSE, sizeof(QuadInstanceData), (GLvoid*)offsetof(QuadInstanceData, texRect));
        glVertexAttribDivisor(aTexRect, 1);
        glEnableVertexAttribArray(aTexRect);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        CheckGLError();
    }

    virtual void Begin() override {
        assert(!eb->shader);
        glUseProgram(p);
        glActiveTexture(GL_TEXTURE0/* + textureUnit*/);
        glUniform1i(uTex0, 0);
        glUniform2f(uCxy, 2 / eb->windowWidth, 2 / eb->windowHeight * eb->flipY);
        glBindVertexArray(va);
    }

    virtual void End() override {
        assert(eb->shader == this);
        if (quadCount) {
            Commit();
        }
    }

    void Commit() {
        glBindBuffer(GL_ARRAY_BUFFER, vb);
        glBufferData(GL_ARRAY_BUFFER, sizeof(QuadInstanceData) * quadCount, quadInstanceDatas.get(), GL_STREAM_DRAW);

        glBindTexture(GL_TEXTURE_2D, lastTextureId);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, quadCount);

        //auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        //printf("fboStatus = %d lastTextureId = %d\n", fboStatus, lastTextureId);
        CheckGLError();

        drawVerts += quadCount * 6;
        drawCall += 1;

        lastTextureId = 0;
        quadCount = 0;
    }

    QuadInstanceData* Draw(GLuint texId, int numQuads) {
        assert(eb->shader == this);
        assert(numQuads <= maxQuadNums);
        if (quadCount + numQuads > maxQuadNums || (lastTextureId && lastTextureId != texId)) {
            Commit();
        }
        lastTextureId = texId;
        auto r = &quadInstanceDatas[quadCount];
        quadCount += numQuads;
        return r;
    }

    void Draw(GLuint texId, QuadInstanceData const& qv) {
        memcpy(Draw(texId, 1), &qv, sizeof(QuadInstanceData));
    }
};
