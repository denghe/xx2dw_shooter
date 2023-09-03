#pragma once
#include "engine_includes.h"

#ifndef NDEBUG
inline void CheckGLErrorAt(const char* file, int line, const char* func) {
		if (auto e = glGetError(); e != GL_NO_ERROR) {
            printf("glGetError() == %d\n", e);
			throw std::runtime_error(std::string("OpenGL error: ") + file + std::to_string(line) + func);
		}
	}
#define CheckGLError() CheckGLErrorAt(__FILE__, __LINE__, __FUNCTION__)
#else
#define CheckGLError() ((void)0)
#endif

enum class GLResTypes {
    Shader, Program, VertexArrays, Buffer, Texture, FrameBuffer
};

template<GLResTypes T, typename...VS>
struct GLRes {

    std::tuple<GLuint, VS...> vs;

    GLRes(GLuint i) : vs(std::make_tuple(i)) {}

    template<typename...Args>
    GLRes(GLuint i, Args&&... args) : vs(std::make_tuple(i, std::forward<Args>(args)...)) {}

    operator GLuint const& () const { return std::get<0>(vs); }
    GLuint* Get() { return &std::get<0>(vs); }

    GLRes(GLRes const&) = delete;
    GLRes& operator=(GLRes const&) = delete;
    GLRes() = default;
    GLRes(GLRes&& o) noexcept {
        std::swap(vs, o.vs);
    }
    GLRes& operator=(GLRes&& o) noexcept {
        std::swap(vs, o.vs);
        return *this;
    }

    ~GLRes() {
        if (!std::get<0>(vs)) return;
        if constexpr (T == GLResTypes::Shader) {
            glDeleteShader(std::get<0>(vs));
        }
        if constexpr (T == GLResTypes::Program) {
            glDeleteProgram(std::get<0>(vs));
        }
        if constexpr (T == GLResTypes::VertexArrays) {
            glDeleteVertexArrays(1, &std::get<0>(vs));
        }
        if constexpr (T == GLResTypes::Buffer) {
            glDeleteBuffers(1, &std::get<0>(vs));
        }
        if constexpr (T == GLResTypes::Texture) {
            glDeleteTextures(1, &std::get<0>(vs));
        }
        if constexpr (T == GLResTypes::FrameBuffer) {
            glDeleteFramebuffers(1, &std::get<0>(vs));
        }
        std::get<0>(vs) = 0;
    }
};

using GLShader = GLRes<GLResTypes::Shader>;

using GLProgram = GLRes<GLResTypes::Program>;

using GLVertexArrays = GLRes<GLResTypes::VertexArrays>;

using GLBuffer = GLRes<GLResTypes::Buffer>;

using GLFrameBuffer = GLRes<GLResTypes::FrameBuffer>;

using GLTextureCore = GLRes<GLResTypes::Texture>;


template<GLuint filter = GL_NEAREST /* GL_LINEAR */, GLuint wraper = GL_CLAMP_TO_EDGE /* GL_REPEAT */>
void GLTexParameteri() {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wraper);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wraper);
}

template<bool exitBind0 = false, GLuint filter = GL_NEAREST /* GL_LINEAR */, GLuint wraper = GL_CLAMP_TO_EDGE /* GL_REPEAT */>
GLuint GLGenTextures() {
    GLuint t{};
    glGenTextures(1, &t);
    glBindTexture(GL_TEXTURE_2D, t);
    GLTexParameteri<filter, wraper>();
    if constexpr(exitBind0) {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    //printf("GLGenTextures t = %d\n", t);
    return t;
}

struct GLTexture : GLRes<GLResTypes::Texture, GLsizei, GLsizei, std::string> {
    using BT = GLRes<GLResTypes::Texture, GLsizei, GLsizei, std::string>;
    using BT::BT;

    template<GLuint filter = GL_NEAREST /* GL_LINEAR */, GLuint wraper = GL_CLAMP_TO_EDGE /* GL_REPEAT */>
    void SetGLTexParm() {
        glBindTexture(GL_TEXTURE_2D, std::get<0>(vs));
        GLTexParameteri<filter, wraper>();
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    auto const& Width() const { return std::get<1>(vs); }
    auto const& Height() const { return std::get<2>(vs); }
    auto const& FileName() const { return std::get<3>(vs); }

    template<GLuint filter = GL_NEAREST /* GL_LINEAR */, GLuint wraper = GL_CLAMP_TO_EDGE /* GL_REPEAT */>
    static GLTexture Create(uint32_t const& w, uint32_t const& h, bool const& hasAlpha) {
        auto t = GLGenTextures<false, filter, wraper>();
        auto c = hasAlpha ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, c, w, h, 0, c, GL_UNSIGNED_BYTE, {});
        glBindTexture(GL_TEXTURE_2D, 0);
        return GLTexture(t, w, h, "");
    }
};

/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

inline GLShader LoadGLShader(GLenum const& type, std::initializer_list<std::string_view>&& codes_) {
    assert(codes_.size() && (type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER));
    auto&& shader = glCreateShader(type);
    if (!shader)
        throw std::logic_error(std::string("glCreateShader(") + std::to_string(type) + ") failed.");
    std::vector<GLchar const*> codes;
    codes.resize(codes_.size());
    std::vector<GLint> codeLens;
    codeLens.resize(codes_.size());
    auto ss = codes_.begin();
    for (size_t i = 0; i < codes.size(); ++i) {
        codes[i] = (GLchar const*)ss[i].data();
        codeLens[i] = (GLint)ss[i].size();
    }
    glShaderSource(shader, (GLsizei)codes_.size(), codes.data(), codeLens.data());
    glCompileShader(shader);
    GLint r = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &r);
    if (!r) {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &r);	// fill txt len into r
        std::string s;
        if (r) {
            s.resize(r);
            glGetShaderInfoLog(shader, r, nullptr, s.data());	// copy txt to s
        }
        throw std::logic_error("glCompileShader failed: err msg = " + s);
    }
    return GLShader(shader);
}

inline GLShader LoadGLVertexShader(std::initializer_list<std::string_view>&& codes_) {
    return LoadGLShader(GL_VERTEX_SHADER, std::move(codes_));
}

inline GLShader LoadGLFragmentShader(std::initializer_list<std::string_view>&& codes_) {
    return LoadGLShader(GL_FRAGMENT_SHADER, std::move(codes_));
}

inline GLProgram LinkGLProgram(GLuint const& vs, GLuint const& fs) {
    auto program = glCreateProgram();
    if (!program)
        throw std::logic_error("glCreateProgram failed.");
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    GLint r = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &r);
    if (!r) {
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &r);
        std::string s;
        if (r) {
            s.resize(r);
            glGetProgramInfoLog(program, r, nullptr, s.data());
        }
        throw std::logic_error("glLinkProgram failed: err msg = " + s);
    }
    return GLProgram(program);
}

/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

inline GLFrameBuffer MakeGLFrameBuffer() {
    GLuint f{};
    glGenFramebuffers(1, &f);
    //glBindFramebuffer(GL_FRAMEBUFFER, f);
    return GLFrameBuffer(f);
}

inline void BindGLFrameBufferTexture(GLuint const& f, GLuint const& t) {
    glBindFramebuffer(GL_FRAMEBUFFER, f);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t, 0);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

inline void UnbindGLFrameBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
