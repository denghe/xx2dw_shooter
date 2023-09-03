#pragma once
#include "engine_shader.h"

struct QuadInstanceData {
    XY pos{}, anchor{ 0.5, 0.5 }, scale{ 1, 1 };
    float radians{};
    RGBA8 color{ 255, 255, 255, 255 };
    uint16_t texRectX{}, texRectY{}, texRectW{}, texRectH{};
};

struct Shader_QuadInstance : Shader {
    using Shader::Shader;
    GLint uCxy = -1, uTex0 = -1, aVert = -1, aPosAnchor = -1, aScaleRadians = -1, aColor = -1, aTexRect = -1;
    GLVertexArrays va;
    GLBuffer vb, ib;

    static const size_t maxQuadNums = 200000;
    GLuint lastTextureId = 0;
    std::unique_ptr<QuadInstanceData[]> quadInstanceDatas = std::make_unique<QuadInstanceData[]>(maxQuadNums);
    size_t quadCount = 0;

    void Init() {
        v = LoadGLVertexShader({ R"(#version 300 es
uniform vec2 uCxy;	// screen center coordinate

in vec2 aVert;	// fans index { 0, 0 }, { 0, 1.f }, { 1.f, 0 }, { 1.f, 1.f }

in vec4 aPosAnchor;
in vec3 aScaleRadians;
in vec4 aColor;
in vec4 aTexRect;

out vec2 vTexCoord;
out vec4 vColor;

void main() {
    vec2 pos = aPosAnchor.xy;
	vec2 anchor = aPosAnchor.zw;
    vec2 scale = vec2(aScaleRadians.x * aTexRect.z, aScaleRadians.y * aTexRect.w);
	float radians = aScaleRadians.z;
    vec2 offset = vec2((aVert.x - anchor.x) * scale.x, (aVert.y - anchor.y) * scale.y);

    float c = cos(radians);
    float s = sin(radians);
    vec2 v = pos + vec2(
       dot(offset, vec2(c, s)),
       dot(offset, vec2(-s, c))
    );

    gl_Position = vec4(v * uCxy, 0, 1);
	vColor = aColor;
	vTexCoord = vec2(aTexRect.x + aVert.x * aTexRect.z, aTexRect.y + aTexRect.w - aVert.y * aTexRect.w);
})"sv });

        f = LoadGLFragmentShader({ R"(#version 300 es
precision highp float;
uniform sampler2D uTex0;

in vec4 vColor;
in vec2 vTexCoord;

out vec4 oColor;

void main() {
	oColor = vColor * texture(uTex0, vTexCoord / vec2(textureSize(uTex0, 0)));
})"sv });

        p = LinkGLProgram(v, f);

        uCxy = glGetUniformLocation(p, "uCxy");
        uTex0 = glGetUniformLocation(p, "uTex0");

        aVert = glGetAttribLocation(p, "aVert");
        aPosAnchor = glGetAttribLocation(p, "aPosAnchor");
        aScaleRadians = glGetAttribLocation(p, "aScaleRadians");
        aColor = glGetAttribLocation(p, "aColor");
        aTexRect = glGetAttribLocation(p, "aTexRect");
        CheckGLError();

        glGenVertexArrays(1, va.Get());
        glBindVertexArray(va);

        glGenBuffers(1, (GLuint*)&ib);
        static const XY verts[4] = { { 0, 0 }, { 0, 1.f }, { 1.f, 0 }, { 1.f, 1.f } };
        glBindBuffer(GL_ARRAY_BUFFER, ib);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
        glVertexAttribPointer(aVert, 2, GL_FLOAT, GL_FALSE, sizeof(XY), 0);
        glEnableVertexAttribArray(aVert);

        glGenBuffers(1, (GLuint*)&vb);
        glBindBuffer(GL_ARRAY_BUFFER, vb);

        glVertexAttribPointer(aPosAnchor, 4, GL_FLOAT, GL_FALSE, sizeof(QuadInstanceData), 0);
        glVertexAttribDivisor(aPosAnchor, 1);
        glEnableVertexAttribArray(aPosAnchor);

        glVertexAttribPointer(aScaleRadians, 3, GL_FLOAT, GL_FALSE, sizeof(QuadInstanceData), (GLvoid*)offsetof(QuadInstanceData, scale));
        glVertexAttribDivisor(aScaleRadians, 1);
        glEnableVertexAttribArray(aScaleRadians);

        glVertexAttribPointer(aColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(QuadInstanceData), (GLvoid*)offsetof(QuadInstanceData, color));
        glVertexAttribDivisor(aColor, 1);
        glEnableVertexAttribArray(aColor);

        glVertexAttribPointer(aTexRect, 4, GL_UNSIGNED_SHORT, GL_FALSE, sizeof(QuadInstanceData), (GLvoid*)offsetof(QuadInstanceData, texRectX));
        glVertexAttribDivisor(aTexRect, 1);
        glEnableVertexAttribArray(aTexRect);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        CheckGLError();
    }

    void Begin(float w, float h) {
        glUseProgram(p);
        glActiveTexture(GL_TEXTURE0/* + textureUnit*/);
        glUniform1i(uTex0, 0);
        glUniform2f(uCxy, 2 / w, 2 / h);
        glBindVertexArray(va);
    }

    void End() {
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

    QuadInstanceData* Draw(GLTexture const& tex, int numQuads) {
        assert(numQuads <= maxQuadNums);
        if (quadCount + numQuads > maxQuadNums || (lastTextureId && lastTextureId != tex)) {
            Commit();
        }
        lastTextureId = tex;
        auto r = &quadInstanceDatas[quadCount];
        quadCount += numQuads;
        return r;
    }

    void Draw(GLTexture const& tex, QuadInstanceData const& qv) {
        memcpy(Draw(tex, 1), &qv, sizeof(QuadInstanceData));
    }
};
