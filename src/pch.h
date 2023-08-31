#pragma once
#ifndef XX2DW_GEMINI_PCH_H_
#define XX2DW_GEMINI_PCH_H_

#include <xx_task.h>
#include <xx_queue.h>
#include <xx_string.h>
#include <xx_data.h>

#include <rect_pack_2d.h>
#include <random>

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/fetch.h>
#include <webgl/webgl2.h>

/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

// reference from https://github.com/cslarsen/mersenne-twister
// faster than std impl, can store & restore state data directly
// ser/de data size == 5000 bytes
struct Rnd {

#pragma region impl
    inline static const size_t SIZE = 624;
    inline static const size_t PERIOD = 397;
    inline static const size_t DIFF = SIZE - PERIOD;
    inline static const uint32_t MAGIC = 0x9908b0df;

    uint32_t MT[SIZE];
    uint32_t MT_TEMPERED[SIZE];
    size_t index = SIZE;
    uint32_t seed;

#define Random5_M32(x) (0x80000000 & x) // 32nd MSB
#define Random5_L31(x) (0x7FFFFFFF & x) // 31 LSBs
#define Random5_UNROLL(expr) \
  y = Random5_M32(MT[i]) | Random5_L31(MT[i+1]); \
  MT[i] = MT[expr] ^ (y >> 1) ^ (((int32_t(y) << 31) >> 31) & MAGIC); \
  ++i;
    void Generate() {
        size_t i = 0;
        uint32_t y;
        while (i < DIFF) {
            Random5_UNROLL(i + PERIOD);
        }
        while (i < SIZE - 1) {
            Random5_UNROLL(i - DIFF);
        }
        {
            y = Random5_M32(MT[SIZE - 1]) | Random5_L31(MT[0]);
            MT[SIZE - 1] = MT[PERIOD - 1] ^ (y >> 1) ^ (((int32_t(y) << 31) >> 31) & MAGIC);
        }
        for (size_t i = 0; i < SIZE; ++i) {
            y = MT[i];
            y ^= y >> 11;
            y ^= y << 7 & 0x9d2c5680;
            y ^= y << 15 & 0xefc60000;
            y ^= y >> 18;
            MT_TEMPERED[i] = y;
        }
        index = 0;
    }
#undef Random5_UNROLL
#undef Random5_L31
#undef Random5_M32
#pragma endregion

    Rnd() {
        SetSeed(std::random_device()());
    }

    void SetSeed(uint32_t const& seed) {
        this->seed = seed;
        MT[0] = seed;
        index = SIZE;
        for (uint_fast32_t i = 1; i < SIZE; ++i) {
            MT[i] = 0x6c078965 * (MT[i - 1] ^ MT[i - 1] >> 30) + i;
        }
    }

    uint32_t Get() {
        if (index == SIZE) {
            Generate();
            index = 0;
        }
        return MT_TEMPERED[index++];
    }

    void NextBytes(void* buf, size_t len) {
        if (index == SIZE) {
            Generate();
            index = 0;
        }
        if (auto left = (SIZE - index) * 4; left >= len) {
            memcpy(buf, &MT_TEMPERED[index], len);
            index += len / 4 + (len % 4 ? 1 : 0);
        } else {
            memcpy(buf, &MT_TEMPERED[index], left);
            index = SIZE;
            NextBytes((char*)buf + left, len - left);
        }
    }

    std::string NextWord(size_t siz = 0, std::string_view chars = "abcdefghijklmnopqrstuvwxyz"sv) {
        assert(chars.size() < 256);
        if (!siz) {
            siz = Next(2, 10);
        }
        std::string s;
        s.resize(siz);
        NextBytes(s.data(), siz);
        for (auto& c : s) {
            c = chars[c % chars.size()];
        }
        return s;
    }

    template<typename V = int32_t, class = std::enable_if_t<std::is_arithmetic_v<V>>>
    V Next() {
        if constexpr (std::is_same_v<bool, std::decay_t<V>>) {
            return Get() >= std::numeric_limits<uint32_t>::max() / 2;
        } else if constexpr (std::is_integral_v<V>) {
            std::make_unsigned_t<V> v;
            if constexpr (sizeof(V) <= 4) {
                v = (V)Get();
            } else {
                v = (V)(Get() | ((uint64_t)Get() << 32));
            }
            if constexpr (std::is_signed_v<V>) {
                return (V)(v & std::numeric_limits<V>::max());
            } else return (V)v;
        } else if constexpr (std::is_floating_point_v<V>) {
            if constexpr (sizeof(V) == 4) {
                return (float)(double(Get()) / 0xFFFFFFFFu);
            } else if constexpr (sizeof(V) == 8) {
                constexpr auto max53 = (1ull << 53) - 1;
                auto v = ((uint64_t)Get() << 32) | Get();
                return double(v & max53) / max53;
            }
        }
        assert(false);
    }

    template<typename V>
    V Next(V const& from, V const& to) {
        if (from == to) return from;
        assert(from < to);
        if constexpr (std::is_floating_point_v<V>) {
            return from + Next<V>() * (to - from);
        } else {
            return from + Next<V>() % (to - from + 1);
        }
    }

    template<typename V>
    V Next2(V from, V to) {
        if (from == to) return from;
        if (to < from) {
            std::swap(from, to);
        }
        if constexpr (std::is_floating_point_v<V>) {
            return from + Next<V>() * (to - from);
        } else {
            return from + Next<V>() % (to - from + 1);
        }
    }

    template<typename V>
    V Next(V const& to) {
        return Next((V)0, to);
    }

    template<typename V>
    V Next(std::pair<V, V> const& fromTo) {
        return Next(fromTo.first, fromTo.second);
    }
};

/**********************************************************************************************************************************/
/**********************************************************************************************************************************/


// code at library_js.js
extern "C" {
void init_gCanvas(int charSize, int width, int height);
double upload_unicode_char_to_texture(int charSize, const char *unicodeChar);
void load_texture_from_url(GLuint texture, const char *url, int *outWidth, int *outHeight);
}

/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

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

/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

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

/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

template<typename T>
concept HasFieldXY = requires { T::x; T::y; };

template<typename T>
concept IsArithmetic = std::is_arithmetic_v<T>;

template<typename T = int32_t>
struct Vec2 {
    using ElementType = T;
    T x, y;

    // -x
    Vec2 operator-() const {
        return { -x, -y };
    }

    // + - * /
    Vec2 operator+(HasFieldXY auto const& v) const {
        return { T(x + v.x), T(y + v.y) };
    }
    Vec2 operator-(HasFieldXY auto const& v) const {
        return { T(x - v.x), T(y - v.y) };
    }
    Vec2 operator*(HasFieldXY auto const& v) const {
        return { T(x * v.x), T(y * v.y) };
    }
    Vec2 operator/(HasFieldXY auto const& v) const {
        return { T(x / v.x), T(y / v.y) };
    }

    Vec2 operator+(IsArithmetic auto const& v) const {
        return { T(x + v), T(y + v) };
    }
    Vec2 operator-(IsArithmetic auto const& v) const {
        return { T(x - v), T(y - v) };
    }
    Vec2 operator*(IsArithmetic auto const& v) const {
        return { T(x * v), T(y * v) };
    }
    Vec2 operator/(IsArithmetic auto const& v) const {
        return { T(x / v), T(y / v) };
    }

    // += -= *= /=
    Vec2& operator+=(HasFieldXY auto const& v) {
        x = T(x + v.x);
        y = T(y + v.y);
        return *this;
    }
    Vec2& operator-=(HasFieldXY auto const& v) {
        x = T(x - v.x);
        y = T(y - v.y);
        return *this;
    }
    Vec2& operator*=(HasFieldXY auto const& v) {
        x = T(x * v.x);
        y = T(y * v.y);
        return *this;
    }
    Vec2& operator/=(HasFieldXY auto const& v) {
        x = T(x / v.x);
        y = T(y / v.y);
        return *this;
    }

    Vec2& operator+=(IsArithmetic auto const& v) {
        x = T(x + v);
        y = T(y + v);
        return *this;
    }
    Vec2 operator-=(IsArithmetic auto const& v) {
        x = T(x - v);
        y = T(y - v);
        return *this;
    }
    Vec2& operator*=(IsArithmetic auto const& v) {
        x = T(x * v);
        y = T(y * v);
        return *this;
    }
    Vec2 operator/=(IsArithmetic auto const& v) {
        x = T(x / v);
        y = T(y / v);
        return *this;
    }

    // == !=
    bool operator==(HasFieldXY auto const& v) const {
        return x == v.x && y == v.y;
    }
    bool operator!=(HasFieldXY auto const& v) const {
        return x != v.x || y != v.y;
    }

    Vec2 MakeAdd(IsArithmetic auto const& vx, IsArithmetic auto const& vy) const {
        return { x + vx, y + vy };
    }

    void Set(HasFieldXY auto const& v) {
        x = T(v.x);
        y = T(v.y);
    }

    template<typename U = float>
    auto As() const -> Vec2<U> {
        return { (U)x, (U)y };
    }

    bool IsZero() const {
        return x == T{} && y == T{};
    }

    void Clear() {
        x = {};
        y = {};
    }

    template<typename U = float>
    Vec2& Normalize() {
        auto v = std::sqrt(U(x * x + y * y));
        assert(v);
        x = T(x / v);
        y = T(y / v);
        return *this;
    }
    template<typename R = T, typename U = float>
    auto MakeNormalize() -> Vec2<R> {
        auto v = std::sqrt(U(x * x + y * y));
        assert(v);
        return { R(x / v), R(y / v) };
    }

    Vec2& FlipY() {
        y = -y;
        return *this;
    }
    template<typename R = T>
    auto MakeFlipY() -> Vec2<R> {
        return { R(x), R(-y) };
    }
};

template<typename T>
struct IsVec2 : std::false_type {};
template<typename T>
struct IsVec2<Vec2<T>> : std::true_type {};
template<typename T>
struct IsVec2<Vec2<T>&> : std::true_type {};
template<typename T>
struct IsVec2<Vec2<T> const&> : std::true_type {};
template<typename T>
constexpr bool IsVec2_v = IsVec2<T>::value;

// pos
using XY = Vec2<float>;

// texture uv mapping pos
struct UV {
    uint16_t u, v;
};

// 4 bytes color
struct RGBA8 {
    uint8_t r, g, b, a;
    bool operator==(RGBA8 const&) const = default;
    bool operator!=(RGBA8 const&) const = default;
};

// 4 floats color
struct RGBA {
    float r, g, b, a;

    operator RGBA8() const {
        return { uint8_t(r * 255), uint8_t(g * 255), uint8_t(b * 255), uint8_t(a * 255) };
    }

    RGBA operator+(RGBA const& v) const {
        return { r + v.r, g + v.g, b + v.b, a + v.a };
    }
    RGBA operator-(RGBA const& v) const {
        return { r - v.r, g - v.g, b - v.b, a - v.a };
    }

    RGBA operator*(IsArithmetic auto const& v) const {
        return { r * v, g * v, b * v, a * v };
    }
    RGBA operator/(IsArithmetic auto const& v) const {
        return { r / v, g / v, b / v, a / v };
    }

    RGBA& operator+=(RGBA const& v) {
        r += v.r;
        g += v.g;
        b += v.b;
        a += v.a;
        return *this;
    }
};

// pos + size
struct Rect : XY {
    XY wh;
};

namespace xx {
    template<typename T>
    struct DataFuncs<T, std::enable_if_t<IsVec2_v<T>>> {
        template<bool needReserve = true>
        static inline void Write(Data& d, T const& in) {
            d.Write<needReserve>(in.x, in.y);
        }
        static inline int Read(Data_r& d, T& out) {
            return d.Read(out.x, out.y);
        }
    };
}

/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

struct Shader {
    inline static int drawVerts{}, drawCall{};
    inline static void ClearCounter() {
        drawVerts = {};
        drawCall = {};
    }

    GLShader v, f;
    GLProgram p;
    Shader() = default;
    Shader(Shader const&) = delete;
    Shader& operator=(Shader const&) = delete;
};

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

/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

struct EngineBase {
    inline static float w = 800, h = 600;          // can change at Init()
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE glContext;
    float flipY{ 1 };   // -1: flip  for ogl frame buffer
    Shader_QuadInstance shader;
    Rnd rnd;

    double nowSecs{}, delta{};
    double timePool{};
    int frameNumber{};
    EM_BOOL running{ EM_TRUE };

    void GLInit() {
        emscripten_set_canvas_element_size("canvas", (int)w, (int)h);
        EmscriptenWebGLContextAttributes attrs;
        emscripten_webgl_init_context_attributes(&attrs);
        attrs.alpha = 0;
        attrs.majorVersion = 2;
        glContext = emscripten_webgl_create_context("canvas", &attrs);
        xx_assert(glContext);
        emscripten_webgl_make_context_current(glContext);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        shader.Init();
    }

    void GLViewport() {
        glViewport(0, 0, (int)w, (int)h);
    }

    void GLClear(RGBA8 c) {
        glClearColor(c.r / 255.f, c.g / 255.f, c.b / 255.f, c.a / 255.f);
        //glClear(GL_COLOR_BUFFER_BIT);
        glDepthMask(true);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDepthMask(false);
    }

    void GLUpdate() {
        GLViewport();
        GLClear({});

        Shader::ClearCounter();
        GLShaderBegin();
    }

    void GLShaderBegin() {
        shader.Begin(w, h * flipY);
    }

    void GLShaderEnd() {
        shader.End();
    }

    void GLUpdateEnd() {
        shader.End();
    }


    EngineBase();
};

inline EngineBase* gEngine{};

inline EngineBase::EngineBase() {
    gEngine = this;
}


/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

struct FrameBuffer {
    GLFrameBuffer fb;
    XY bak{};

    // need ogl frame env
    explicit FrameBuffer(bool autoInit = false) {
        if (autoInit) {
            Init();
        }
    }

    // need ogl frame env
    FrameBuffer& Init() {
        if (fb) {
            throw std::logic_error("excessive initializations ?");
        }
        fb = MakeGLFrameBuffer();
        return *this;
    }

    template<typename WH>
    inline static xx::Shared<GLTexture> MakeTexture(WH const& wh, bool const& hasAlpha = true) {
        return xx::Make<GLTexture>(GLTexture::Create(wh.x, wh.y, hasAlpha));
    }

    template<typename Func>
    void DrawTo(xx::Shared<GLTexture>& t, std::optional<RGBA8> const& c, Func&& func) {
        Begin(t, c);
        func();
        End();
    }

    template<typename Func>
    xx::Shared<GLTexture> Draw(Vec2<uint32_t> const& wh, bool const& hasAlpha, std::optional<RGBA8> const& c, Func&& func) {
        auto t = MakeTexture(wh, hasAlpha);
        DrawTo(t, c, std::forward<Func>(func));
        return t;
    }

protected:
    void Begin(xx::Shared<GLTexture>& t, std::optional<RGBA8> const& c = {}) {
        gEngine->GLShaderEnd();
        bak.x = std::exchange(gEngine->w, t->Width());
        bak.y = std::exchange(gEngine->h, t->Height());
        gEngine->flipY = -1;
        BindGLFrameBufferTexture(fb, *t);
        gEngine->GLViewport();
        if (c.has_value()) {
            gEngine->GLClear(c.value());
        }
        gEngine->GLShaderBegin();
    }
    void End() {
        gEngine->GLShaderEnd();
        UnbindGLFrameBuffer();
        gEngine->w = bak.x;
        gEngine->h = bak.y;
        gEngine->flipY = 1;
        gEngine->GLViewport();
        gEngine->GLShaderBegin();
    }
};

/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

// sprite frame
struct Frame {
    xx::Shared<GLTexture> tex;
    std::string key;
    // std::vector<std::string> aliases;	// unused
    std::optional<XY> anchor;
    XY spriteOffset{};
    XY spriteSize{};		// content size
    XY spriteSourceSize{};	// original pic size
    Rect textureRect{};
    size_t ud{};   // user data
    bool textureRotated{};
    std::vector<uint16_t> triangles;
    std::vector<float> vertices;
    std::vector<float> verticesUV;

    // single texture -> frame
    inline xx::Shared<Frame> static Create(xx::Shared<GLTexture> t) {
        auto f = xx::Make<Frame>();
        f->key = t->FileName();
        f->anchor = { 0.5, 0.5 };
        f->textureRotated = false;
        f->spriteSize = f->spriteSourceSize = { (float)t->Width(), (float)t->Height() };
        f->spriteOffset = {};
        f->textureRect = { 0, 0, f->spriteSize.x, f->spriteSize.y };
        f->tex = std::move(t);
        return f;
    }
};


/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

// sprite
struct Quad : QuadInstanceData {
    xx::Shared<GLTexture> tex;

    Quad& SetTexture(xx::Shared<GLTexture> const& t) {
        tex = t;
        texRectX = 0;
        texRectY = 0;
        texRectW = t->Width();
        texRectH = t->Height();
        return *this;
    }
    Quad& SetFrame(xx::Shared<Frame> const& f) {
        assert(f && !f->textureRotated);	// known issue: not support now
        tex = f->tex;
        texRectX = f->textureRect.x;
        texRectY = f->textureRect.y;
        texRectW = (uint16_t)f->textureRect.wh.x;
        texRectH = (uint16_t)f->textureRect.wh.y;
        return *this;
    }
    template<typename T = float>
    XY Size() const {
        assert(tex);
        return { (T)texRectW, (T)texRectH };
    }
    Quad& SetAnchor(XY const& a) {
        anchor = a;
        return *this;
    }
    Quad& SetRotate(float const& r) {
        radians = r;
        return *this;
    }
    Quad& AddRotate(float const& r) {
        radians += r;
        return *this;
    }
    Quad& SetScale(XY const& s) {
        scale = s;
        return *this;
    }
    Quad& SetScale(float const& s) {
        scale = { s, s };
        return *this;
    }
    Quad& SetPosition(XY const& p) {
        pos = p;
        return *this;
    }
    Quad& SetPositionX(float const& x) {
        pos.x = x;
        return *this;
    }
    Quad& SetPositionY(float const& y) {
        pos.y = y;
        return *this;
    }
    Quad& AddPosition(XY const& p) {
        pos += p;
        return *this;
    }
    Quad& AddPositionX(float const& x) {
        pos.x += x;
        return *this;
    }
    Quad& AddPositionY(float const& y) {
        pos.y += y;
        return *this;
    }
    Quad& SetColor(RGBA8 const& c) {
        color = c;
        return *this;
    }
    Quad& SetColorA(uint8_t const& a) {
        color.a = a;
        return *this;
    }
    Quad& SetColorAf(float const& a) {
        color.a = 255 * a;
        return *this;
    }
    Quad& Draw() {
        gEngine->shader.Draw(*tex, *this);
        return *this;
    }
};

/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

struct CharInfo {
    xx::Shared<GLTexture> tex;
    uint16_t texRectX{}, texRectY{}, texRectW{}, texRectH{};
};

template<int charSize_ = 24, int canvasWidth_ = int(charSize_ / 0.75), int canvasHeight_ = int(charSize_ / 0.75), int texWidth_ = 2048, int texHeight_ = 2048>
struct CharTexCache {
    static constexpr int charSize = charSize_, canvasWidth = canvasWidth_, canvasHeight = canvasHeight_, texWidth = texWidth_, texHeight = texHeight_;
    std::vector<xx::Shared<GLTexture>> texs;
    xx::Shared<GLTexture> ct;
    FrameBuffer fb;
    float cw{};
    XY p{ 0, texHeight - 1 };

    std::array<CharInfo, 256> bases;
    std::unordered_map<char32_t, CharInfo> extras;

    // need ogl frame env
    void Init() {
        fb.Init();
        init_gCanvas(charSize, canvasWidth, canvasHeight);

        texs.emplace_back(FrameBuffer::MakeTexture(Vec2{ texWidth, texHeight }));
        ct = xx::Make<GLTexture>(GLGenTextures<true>(), canvasWidth, canvasHeight, "");

        char buf[16];
        for (char32_t c = 0; c < 256; ++c) {
            MakeCharInfo(c);
        }
    }

    void FillCharTex(char32_t c) {
        char buf[16];
        buf[xx::Char32ToUtf8(c, buf)] = '\0';
        glBindTexture(GL_TEXTURE_2D, *ct);
        cw = upload_unicode_char_to_texture(charSize, buf);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    CharInfo& MakeCharInfo(char32_t c) {
        FillCharTex(c);
        CharInfo* ci{};
        if (c < 256) {
            ci = &bases[c];
        } else {
            auto rtv = extras.insert(std::make_pair(c, CharInfo{}));
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
            Quad().SetAnchor({0, 1}).SetPosition(cp + XY{-texWidth / 2, -texHeight / 2}).SetTexture(ct).Draw();
        });

        ci->tex = t;
        ci->texRectX = cp.x;
        ci->texRectY = texHeight - 1 - cp.y;        // flip y for uv
        ci->texRectW = cw;
        ci->texRectH = canvasHeight;
        return *ci;
    }

    CharInfo& Find(char32_t c) {
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
    void Draw(XY pos, std::u32string_view const& s) {
        Quad q;
        q.SetAnchor({ 0.f, 0.5f });

        // make sure all char texture exists ( avoid framebuffer incomplete issue )
        auto cis = (CharInfo**)alloca(s.size() * sizeof(void*));
        auto e = s.size();
        for (size_t i = 0; i < e; ++i) {
            cis[i] = &Find(s[i]);
        }

        for (size_t i = 0; i < e; ++i) {
            auto&& ci = *cis[i];
            q.tex = ci.tex;
            q.texRectX = ci.texRectX;
            q.texRectY = ci.texRectY;
            q.texRectW = ci.texRectW;
            q.texRectH = ci.texRectH;
            q.SetPosition(pos).Draw();
            pos.x += ci.texRectW;
        }
    }

    void Draw(XY const& pos, std::string_view const& s) {
        Draw(pos, xx::StringU8ToU32(s));
    }

    float Measure(std::u32string_view const& s) {
        float w{};
        for (size_t i = 0; i < s.size(); ++i) {
            w += Find(s[i]).texRectW;
        }
        return w;
    }

    float Measure(std::string_view const& s) {
        return Measure(xx::StringU8ToU32(s));
    }

};

/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

struct Frames {
    std::vector<xx::Shared<Frame>> frames;

    // get frame by key
    xx::Shared<Frame> const& Get(std::string_view const& key) const {
        for (auto& f : frames) {
            if (f->key == key) return f;
        }
        throw std::logic_error(xx::ToString(key, " is not found"));
    }

    xx::Shared<Frame> const& Get(char const* const& buf, size_t const& len) const {
        return Get(std::string_view(buf, len));
    }

    // get frames by key's prefix + number. example: "p1 p2 p3" prefix is 'p'

    std::vector<xx::Shared<Frame>> GetByPrefix(std::string_view const& prefix) const {
        std::vector<xx::Shared<Frame>> fs;
        GetToByPrefix(fs, prefix);
        return fs;
    }

    void GetTo(std::vector<xx::Shared<Frame>>& fs, std::initializer_list<std::string_view> keys) const {
        for (auto& k : keys) {
            fs.push_back(Get(k));
        }
    }

    size_t GetToByPrefix(std::vector<xx::Shared<Frame>>& fs, std::string_view const& prefix) const {
        size_t n{};
        for (auto& f : frames) {
            if (f->key.starts_with(prefix)) {
                auto s = f->key.substr(prefix.size());
                if (s[0] >= '0' && s[0] <= '9') {
                    fs.push_back(f);
                    ++n;
                }
            }
        }
        return n;
    }

    xx::Shared<Frame> TryGet(std::string_view const& key) const {
        for (auto& f : frames) {
            if (f->key == key) return f;
        }
        return {};
    }

    std::unordered_map<std::string_view, xx::Shared<Frame>> GetMapSV() const {
        std::unordered_map<std::string_view, xx::Shared<Frame>> fs;
        for (auto& f : frames) {
            fs[std::string_view(f->key)] = f;
        }
        return fs;
    }

    std::unordered_map<std::string, xx::Shared<Frame>> GetMapS() const {
        std::unordered_map<std::string, xx::Shared<Frame>> fs;
        for (auto& f : frames) {
            fs[f->key] = f;
        }
        return fs;
    }
};

struct TexturePacker : Frames {
    bool premultiplyAlpha;
    std::string realTextureFileName;
    std::string plistUrl;

    // fill by plist file's binary content ( .blist ). success return 0
    int Load(xx::Data_r dr, std::string_view const& plistUrl_) {
        frames.clear();
        premultiplyAlpha = {};
        realTextureFileName.clear();
        plistUrl = plistUrl_;

        auto tp = xx::Make<TexturePacker>();
        std::string rootPath;
        if (auto&& i = plistUrl.find_last_of("/"); i != plistUrl.npos) {
            rootPath = plistUrl.substr(0, i + 1);
        }

        size_t numFrames{};
        if (int r = dr.Read(premultiplyAlpha, realTextureFileName, numFrames)) return r;
        for (size_t i = 0; i < numFrames; ++i) {
            auto f = xx::Make<Frame>();
            if (int r = dr.Read(f->key, f->anchor)) return r;
            if (int r = dr.Read((xx::RWFloatUInt16&)f->spriteOffset.x, (xx::RWFloatUInt16&)f->spriteOffset.y)) return r;
            if (int r = dr.Read((xx::RWFloatUInt16&)f->spriteSize.x, (xx::RWFloatUInt16&)f->spriteSize.y)) return r;
            if (int r = dr.Read((xx::RWFloatUInt16&)f->spriteSourceSize.x, (xx::RWFloatUInt16&)f->spriteSourceSize.y)) return r;
            if (int r = dr.Read((xx::RWFloatUInt16&)f->textureRect.x, (xx::RWFloatUInt16&)f->textureRect.y)) return r;
            if (int r = dr.Read((xx::RWFloatUInt16&)f->textureRect.wh.x, (xx::RWFloatUInt16&)f->textureRect.wh.y)) return r;
            frames.emplace_back(std::move(f));
        }
        realTextureFileName = rootPath + realTextureFileName;

        return 0;
    }
};

/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

template<int texWH_ = 2048>
struct DynamicTexturePacker : Frames {
    static constexpr int texWH = texWH_;

    void Clear() {
        frames.clear();
    }

    // need ogl frame env
    // return true: success
    template<bool newFrame = false>
    bool Fill(std::vector<xx::Shared<Frame>>& subFrames) {
        using namespace rect_pack_2d;
        frames.clear();
        frames.reserve(subFrames.size());
        std::vector<rect_xywhf> rects;
        std::vector<rect_xywhf*> rectptrs;
        rects.reserve(subFrames.size());
        rectptrs.reserve(subFrames.size());
        for (auto& sf : subFrames) {
            auto& r = rects.emplace_back(0, 0, sf->textureRect.wh.x, sf->textureRect.wh.y);
            r.ud = &sf;
            rectptrs.push_back(&r);
        }
        std::vector<bin> bins;
        if (!pack(rectptrs.data(), rectptrs.size(), texWH, false, bins)) return false;

        FrameBuffer fb(true);
        for (auto& bin : bins) {
            int w = xx::Round2n(bin.size.w);
            int h = xx::Round2n(bin.size.h);
            auto t = FrameBuffer::MakeTexture(Vec2{ w, h });
            XY basePos{ -w / 2, -h / 2 };
            fb.DrawTo(t, {}, [&]() {
                Quad q;
                q.SetAnchor({ 0, 1 });
                for (auto& r : bin.rects) {
                    auto& sf = *(xx::Shared<Frame>*)r->ud;
                    q.SetPosition(basePos + XY{ r->x, r->y }).SetFrame(sf).Draw();
                    if constexpr (newFrame) {
                        auto&& f = frames.emplace_back().Emplace();
                        *f = *sf;
                        f->textureRect.x = r->x;
                        f->textureRect.y = h - 1 - r->y;
                        f->tex = t;
                    } else {
                        frames.emplace_back(sf);
                        sf->textureRect.x = r->x;
                        sf->textureRect.y = h - 1 - r->y;
                        sf->tex = t;
                    }
                }
            });
        }
        return true;
    }

    bool Fill(std::vector<xx::Shared<GLTexture>> const& subTexs) {
        std::vector<xx::Shared<Frame>> fs;
        fs.reserve(subTexs.size());
        for (auto& t : subTexs) {
            fs.emplace_back(Frame::Create(t));
        }
        return Fill(fs);
    }
};

/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

struct CurvePoint {
    XY pos{};
    float tension{ 0.2f };
    int32_t numSegments{ 100 };

    inline CurvePoint operator+(CurvePoint const& v) const {
        return { pos + v.pos, tension, numSegments };
    }

    inline CurvePoint operator-(CurvePoint const& v) const {
        return { pos - v.pos, tension, numSegments };
    }

    inline CurvePoint operator*(float const& v) const {
        return { pos * v, tension, numSegments };
    }
};

struct CurvePoints {
    std::string name;
    bool isLoop{};
    std::vector<CurvePoint> points;
};

struct CurvesPointsCollection {
    std::vector<CurvePoints> data;
};

struct MovePathPoint {
    XY pos{}, inc{};
    float radians{}, distance{};

    inline MovePathPoint& operator=(CurvePoint const& v) {
        pos = v.pos;
        return *this;
    }
};

struct MovePath {
    std::vector<MovePathPoint> points;
    float totalDistance{};
    bool loop{};

    void Clear() {
        points.clear();
        totalDistance = {};
        loop = {};
    }

    void Fill(XY const* ps, size_t len, bool loop) {
        assert(len > 1);
        totalDistance = {};
        this->loop = loop;
        if (ps) {
            points.resize(len);
            for (size_t i = 0; i < len; i++) {
                points[i].pos = ps[i];
            }
        } else {
            assert(len <= points.size());
        }
        for (size_t i = 0; i < len - 1; i++) {
            FillFields(points[i], points[i + 1]);
            totalDistance += points[i].distance;
        }
        if (loop) {
            FillFields(points[len - 1], points[0]);
            totalDistance += points[len - 1].distance;
        } else {
            points[len - 1].distance = {};
            points[len - 1].inc = {};
            points[len - 1].radians = points[len - 2].radians;
        }
    }

    void Fill(bool loop) {
        Fill(nullptr, points.size(), loop);
    }

    void FillFields(MovePathPoint& p1, MovePathPoint& p2) {
        auto v = p2.pos - p1.pos;
        p1.radians = std::atan2(v.y, v.x);
        p1.inc = { std::cos(p1.radians), std::sin(p1.radians) };
        p1.distance = std::sqrt(v.x * v.x + v.y * v.y);
    }

    // pathway curve ( p p p ... )  to 2 control points bezier( p c c p c c p ... )
    template<typename List1, typename List2>
    inline void CurveToBezier(List1& bs, List2 const& cs) {
        auto n = cs.size();
        auto len = n * 3 - 2;
        bs.resize(len);

        bs[0] = cs[0];
        bs[1] = (cs[1] - cs[0]) * cs[0].tension + cs[0];

        for (size_t i = 0; i < n - 2; i++) {
            auto diff = (cs[i + 2] - cs[i]) * cs[i].tension;
            bs[3 * i + 2] = cs[i + 1] - diff;
            bs[3 * i + 3] = cs[i + 1];
            bs[3 * i + 4] = cs[i + 1] + diff;
        }
        bs[len - 2] = (cs[n - 2] - cs[n - 1]) * cs[n - 2].tension + cs[n - 1];
        bs[len - 1] = cs[n - 1];
    }

    // 2 control points bezier( p c c p c c p ... ) to split points
    template<typename List1, typename List2>
    inline void BezierToPoints(List1& ps, List2 const& bs) {
        auto len = (bs.size() - 1) / 3;
        size_t totalSegments = 0;
        for (size_t j = 0; j < len; ++j) {
            totalSegments += bs[j * 3].numSegments;
        }
        ps.resize(totalSegments);
        totalSegments = 0;
        for (size_t j = 0; j < len; ++j) {
            auto idx = j * 3;
            auto numSegments = bs[idx].numSegments;
            auto step = 1.0f / numSegments;
            for (int i = 0; i < numSegments; ++i) {
                auto t = step * i;
                auto t1 = 1 - t;
                ps[totalSegments + i] = bs[idx] * t1 * t1 * t1
                    + bs[idx + 1] * 3 * t * t1 * t1
                    + bs[idx + 2] * 3 * t * t * (1 - t)
                    + bs[idx + 3] * t * t * t;
            }
            totalSegments += numSegments;
        }
    }

    void FillCurve(bool loop, std::vector<CurvePoint> const& ps) {
        auto len = ps.size();
        assert(len >= 2);

        // 2 point: line
        if (len == 2) {
            points.emplace_back(MovePathPoint{ ps[0].pos });
            points.emplace_back(MovePathPoint{ ps[1].pos });
        }
        // curve
        else {
            std::vector<CurvePoint> cs;

            // curve to 2 control points curve
            if (loop) {
                std::vector<CurvePoint> bs;
                bs.reserve(len + 6);
                // insert addons
                bs.push_back(ps[len - 3]);
                bs.push_back(ps[len - 2]);
                bs.push_back(ps[len - 1]);
                bs.insert(bs.end(), ps.begin(), ps.end());
                bs.push_back(ps[0]);
                bs.push_back(ps[1]);
                bs.push_back(ps[2]);
                CurveToBezier(cs, bs);
                // remove addons
                cs.resize(cs.size() - 6);
                cs.erase(cs.begin(), cs.begin() + 9);
            } else {
                CurveToBezier(cs, ps);
            }

            // 2 control points curve to split points
            BezierToPoints(points, cs);

            if (!loop) {
                auto& last = ps.back();
                auto& point = points.emplace_back();
                point.pos = last.pos;
            }
        }
        // fill inc, radians, distance
        Fill(loop);
    }
};

struct MovePathSteper {
    xx::Shared<MovePath> mp;
    size_t cursor{};	// mp[ index ]
    float cursorDistance{};	// forward

    void Init(xx::Shared<MovePath> mp) {
        this->mp = std::move(mp);
        cursor = {};
        cursorDistance = {};
    }

    struct MoveResult {
        XY pos{};
        float radians{}, movedDistance{};
        bool terminated;
    };

    MoveResult MoveToBegin() {
        assert(mp);
        assert(mp->points.size());
        cursor = {};
        cursorDistance = {};
        auto& p = mp->points.front();
        return { .pos = p.pos, .radians = p.radians, .movedDistance = {}, .terminated = false };
    }

    MoveResult MoveForward(float const& stepDistance) {
        assert(mp);
        assert(mp->points.size());
        auto& ps = mp->points;
        auto siz = ps.size();
        auto loop = mp->loop;
        auto d = stepDistance;
    LabLoop:
        auto& p = ps[cursor];
        auto left = p.distance - cursorDistance;
        if (d > left) {
            d -= left;
            cursorDistance = 0.f;
            ++cursor;
            if (cursor == siz) {
                if (loop) {
                    cursor = 0;
                } else {
                    cursor = siz - 1;
                    return { .pos = p.pos, .radians = p.radians, .movedDistance = stepDistance - d, .terminated = true };
                }
            }
            goto LabLoop;
        } else {
            cursorDistance += d;
        }
        return { .pos = p.pos + (p.inc * cursorDistance), .radians = p.radians, .movedDistance = stepDistance, .terminated = !mp->loop && cursor == siz - 1 };
    }
};

struct MovePathCachePoint {
    XY pos{};
    float radians{};
};

struct MovePathCache {
    std::vector<MovePathCachePoint> points;
    bool loop{};
    float stepDistance{};

    void Init(MovePath const& mp, float const& stepDistance = 1.f) {
        assert(stepDistance > 0);
        assert(mp.totalDistance > stepDistance);
        this->stepDistance = stepDistance;
        this->loop = mp.loop;
        auto td = mp.totalDistance + stepDistance;
        points.clear();
        points.reserve(std::ceil(mp.totalDistance / stepDistance));

        MovePathSteper mpr;
        mpr.mp.pointer = (MovePath*)&mp;	// tmp fill fake ptr instead Init(std::move(mp));
        auto mr = mpr.MoveToBegin();
        points.push_back({ mr.pos, mr.radians });
        for (float d = stepDistance; d < td; d += stepDistance) {
            mr = mpr.MoveForward(stepDistance);
            points.push_back({ mr.pos, mr.radians });
        }
        mpr.mp.pointer = {};	// clear fake ptr
    }

    MovePathCachePoint* Move(float const& totalDistance) {
        int i = totalDistance / stepDistance;
        if (loop) {
            return &points[i % points.size()];
        } else {
            return i < points.size() ? &points[i] : nullptr;
        }
    }
};

/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

struct FpsViewer {
    double fpsTimePool{}, lastSecs{ xx::NowSteadyEpochSeconds() }, counter{}, fps{};

    // CTC: char texture cache
    template<typename CTC>
    void Draw(CTC& cp) {
        ++counter;
        fpsTimePool += xx::NowSteadyEpochSeconds(lastSecs);
        if (fpsTimePool >= 1) {
            fpsTimePool -= 1;
            fps = counter;
            counter = 0;
        }

        gEngine->GLShaderEnd();
        auto s = std::string("FPS:") + std::to_string((uint32_t)fps)
                 + " DC:" + std::to_string(Shader::drawCall)
                 + " VC:" + std::to_string(Shader::drawVerts);
        gEngine->GLShaderBegin();

        cp.Draw({ -gEngine->w / 2, -gEngine->h / 2 + cp.canvasHeight / 2 }, s);
    }
};

/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

// space grid index system for circle
template<typename Item>
struct SpaceGridC;

// for inherit
template<typename Derived>
struct SpaceGridCItem {
    SpaceGridC<Derived>* _sgc{};
    Derived* _sgcPrev{}, * _sgcNext{};
    int32_t _sgcIdx{ -1 };
    Vec2<int32_t> _sgcPos;

    void SGCInit(SpaceGridC<Derived>* const& sgc) {
        assert(!_sgc);
        _sgc = sgc;
    }

    void SGCSetPos(Vec2<int32_t> const& pos) {
        assert(_sgc);
        assert(_sgcPos.x >= 0 && _sgcPos.x < _sgc->maxX);
        assert(_sgcPos.y >= 0 && _sgcPos.y < _sgc->maxY);
        _sgcPos = pos;
    }

    void SGCAdd() {
        assert(_sgc);
        _sgc->Add(((Derived*)(this)));
    }
    void SGCUpdate() {
        assert(_sgc);
        _sgc->Update(((Derived*)(this)));
    }
    void SGCRemove() {
        assert(_sgc);
        _sgc->Remove(((Derived*)(this)));
    }

    void SGCInit(SpaceGridC<Derived>* const& sgc, Vec2<int32_t> const& pos) {
        assert(!_sgc);
        _sgc = sgc;
        SGCSetPos(pos);
        SGCAdd();
    }
    void SGCUpdate(Vec2<int32_t> const& pos) {
        SGCSetPos(pos);
        SGCUpdate();
    }
    void SGCTryRemove() {
        if (_sgc) {
            SGCRemove();
            _sgc = {};
        }
    }
};

template<typename Item>
struct SpaceGridC {
    int32_t numRows{}, numCols{}, maxDiameter{};
    int32_t maxY{}, maxX{}, maxY1{}, maxX1{}, numItems{}, numActives{};	// for easy check & stat
    std::vector<Item*> cells;

    int32_t PosToIndex(Vec2<int32_t> const& p) {
        auto rcIdx = p / maxDiameter;
        return rcIdx.y * numCols + rcIdx.x;
    }

    void Init(int32_t const& numRows_, int32_t const& numCols_, int32_t const& maxDiameter_) {
        numRows = numRows_;
        numCols = numCols_;
        maxDiameter = maxDiameter_;
        maxY = maxDiameter * numRows;
        maxX = maxDiameter * numCols;
        maxY1 = maxY - 1;
        maxX1 = maxX - 1;
        cells.clear();
        cells.resize(numRows * numCols);
    }

    void Add(Item* const& c) {
        assert(c);
        assert(c->_sgc == this);
        assert(c->_sgcIdx == -1);
        assert(!c->_sgcPrev);
        assert(!c->_sgcNext);
        assert(c->_sgcPos.x >= 0 && c->_sgcPos.x < maxX);
        assert(c->_sgcPos.y >= 0 && c->_sgcPos.y < maxY);

        // calc rIdx & cIdx
        int rIdx = c->_sgcPos.y / maxDiameter, cIdx = c->_sgcPos.x / maxDiameter;
        int idx = rIdx * numCols + cIdx;
        assert(idx <= cells.size());
        assert(!cells[idx] || !cells[idx]->_sgcPrev);

        // link
        if (cells[idx]) {
            cells[idx]->_sgcPrev = c;
        }
        c->_sgcNext = cells[idx];
        c->_sgcIdx = idx;
        cells[idx] = c;
        assert(!cells[idx]->_sgcPrev);
        assert(c->_sgcNext != c);
        assert(c->_sgcPrev != c);

        // stat
        ++numItems;
    }

    void Remove(Item* const& c) {
        assert(c);
        assert(c->_sgc == this);
        assert(!c->_sgcPrev && cells[c->_sgcIdx] == c || c->_sgcPrev->_sgcNext == c && cells[c->_sgcIdx] != c);
        assert(!c->_sgcNext || c->_sgcNext->_sgcPrev == c);
        //assert(cells[c->_sgcIdx] include c);

        // unlink
        if (c->_sgcPrev) {	// isn't header
            assert(cells[c->_sgcIdx] != c);
            c->_sgcPrev->_sgcNext = c->_sgcNext;
            if (c->_sgcNext) {
                c->_sgcNext->_sgcPrev = c->_sgcPrev;
                c->_sgcNext = {};
            }
            c->_sgcPrev = {};
        } else {
            assert(cells[c->_sgcIdx] == c);
            cells[c->_sgcIdx] = c->_sgcNext;
            if (c->_sgcNext) {
                c->_sgcNext->_sgcPrev = {};
                c->_sgcNext = {};
            }
        }
        assert(cells[c->_sgcIdx] != c);
        c->_sgcIdx = -1;

        // stat
        --numItems;
    }

    void Update(Item* const& c) {
        assert(c);
        assert(c->_sgc == this);
        assert(c->_sgcIdx > -1);
        assert(c->_sgcNext != c);
        assert(c->_sgcPrev != c);
        //assert(cells[c->_sgcIdx] include c);

        auto idx = CalcIndexByPosition(c->_sgcPos.x, c->_sgcPos.y);
        if (idx == c->_sgcIdx) return;	// no change
        assert(!cells[idx] || !cells[idx]->_sgcPrev);
        assert(!cells[c->_sgcIdx] || !cells[c->_sgcIdx]->_sgcPrev);

        // unlink
        if (c->_sgcPrev) {	// isn't header
            assert(cells[c->_sgcIdx] != c);
            c->_sgcPrev->_sgcNext = c->_sgcNext;
            if (c->_sgcNext) {
                c->_sgcNext->_sgcPrev = c->_sgcPrev;
                //c->_sgcNext = {};
            }
            //c->_sgcPrev = {};
        } else {
            assert(cells[c->_sgcIdx] == c);
            cells[c->_sgcIdx] = c->_sgcNext;
            if (c->_sgcNext) {
                c->_sgcNext->_sgcPrev = {};
                //c->_sgcNext = {};
            }
        }
        //c->_sgcIdx = -1;
        assert(cells[c->_sgcIdx] != c);
        assert(idx != c->_sgcIdx);

        // link
        if (cells[idx]) {
            cells[idx]->_sgcPrev = c;
        }
        c->_sgcPrev = {};
        c->_sgcNext = cells[idx];
        cells[idx] = c;
        c->_sgcIdx = idx;
        assert(!cells[idx]->_sgcPrev);
        assert(c->_sgcNext != c);
        assert(c->_sgcPrev != c);
    }

    int32_t CalcIndexByPosition(int32_t const& x, int32_t const& y) {
        assert(x >= 0 && x < maxX);
        assert(y >= 0 && y < maxY);
        int32_t rIdx = y / maxDiameter, cIdx = x / maxDiameter;
        auto idx = rIdx * numCols + cIdx;
        assert(idx <= cells.size());
        return idx;
    }

    template<bool enableLimit = false, bool enableExcept = false, typename F>
    void Foreach(int32_t const& idx, F&& f, int32_t* limit = nullptr, Item* const& except = nullptr) {
        if constexpr (enableLimit) {
            assert(limit);
            if (*limit <= 0) return;
        }
        assert(idx >= 0 && idx < cells.size());
        auto c = cells[idx];
        while (c) {
            assert(cells[c->_sgcIdx]->_sgcPrev == nullptr);
            assert(c->_sgcNext != c);
            assert(c->_sgcPrev != c);
            if constexpr (enableExcept) {
                if (c != except) {
                    f(c);
                }
            } else {
                f(c);
            }
            if constexpr (enableLimit) {
                if (--*limit <= 0) return;
            }
            c = c->_sgcNext;
        }
    }

    template<bool enableLimit = false, bool enableExcept = false, typename F>
    void Foreach(int32_t const& rIdx, int32_t const& cIdx, F&& f, int32_t* limit = nullptr, Item* const& except = nullptr) {
        if (rIdx < 0 || rIdx >= numRows) return;
        if (cIdx < 0 || cIdx >= numCols) return;
        Foreach<enableLimit, enableExcept>(rIdx * numCols + cIdx, std::forward<F>(f), limit, except);
    }

    template<bool enableLimit = false, typename F>
    void Foreach8NeighborCells(int32_t const& rIdx, int32_t const& cIdx, F&& f, int32_t* limit = nullptr) {
        Foreach<enableLimit>(rIdx + 1, cIdx, f, limit);
        if constexpr (enableLimit) {
            if (*limit <= 0) return;
        }
        Foreach<enableLimit>(rIdx - 1, cIdx, f, limit);
        if constexpr (enableLimit) {
            if (*limit <= 0) return;
        }
        Foreach<enableLimit>(rIdx, cIdx + 1, f, limit);
        if constexpr (enableLimit) {
            if (*limit <= 0) return;
        }
        Foreach<enableLimit>(rIdx, cIdx - 1, f, limit);
        if constexpr (enableLimit) {
            if (*limit <= 0) return;
        }
        Foreach<enableLimit>(rIdx + 1, cIdx + 1, f, limit);
        if constexpr (enableLimit) {
            if (*limit <= 0) return;
        }
        Foreach<enableLimit>(rIdx + 1, cIdx - 1, f, limit);
        if constexpr (enableLimit) {
            if (*limit <= 0) return;
        }
        Foreach<enableLimit>(rIdx - 1, cIdx + 1, f, limit);
        if constexpr (enableLimit) {
            if (*limit <= 0) return;
        }
        Foreach<enableLimit>(rIdx - 1, cIdx - 1, f, limit);
    }

    template<bool enableLimit = false, typename F>
    void Foreach9NeighborCells(Item* c, F&& f, int32_t* limit = nullptr) {
        Foreach<enableLimit, true>(c->_sgcIdx, f, limit, c);
        if constexpr (enableLimit) {
            if (*limit <= 0) return;
        }
        auto rIdx = c->_sgcIdx / numCols;
        auto cIdx = c->_sgcIdx - numCols * rIdx;
        Foreach8NeighborCells<enableLimit>(rIdx, cIdx, f, limit);
    }

    template<bool enableLimit = false, typename F>
    void Foreach9NeighborCells(int32_t const& idx, F&& f, int32_t* limit = nullptr) {
        Foreach<enableLimit>(idx, f, limit);
        if constexpr (enableLimit) {
            if (*limit <= 0) return;
        }
        auto rIdx = idx / numCols;
        auto cIdx = idx - numCols * rIdx;
        Foreach8NeighborCells<enableLimit>(rIdx, cIdx, f, limit);
    }
};


/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

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


// Derived content requires:
// constexpr static float fps = 60, frameDelay = 1.f / fps, maxFrameDelay = frameDelay * 3;
template<typename Derived>
struct Engine : EngineBase {
    xx::Tasks tasks;

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


        if constexpr(Has_Init<Derived>) {
            ((Derived*)this)->Init();
        }
        GLInit();

        if constexpr(Has_AfterInit<Derived>) {
            ((Derived*)this)->AfterInit();
        }

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
        auto s = ms / 1000;
        delta = s - nowSecs;
        nowSecs = s;

        GLUpdate();

        if (delta > ((Derived*)this)->maxFrameDelay) {
            delta = ((Derived*)this)->maxFrameDelay;
        }
        timePool += delta;
        while (timePool >= ((Derived*)this)->frameDelay) {
            timePool -= ((Derived*)this)->frameDelay;
            ++frameNumber;
            if constexpr(Has_Update<Derived>) {
                ((Derived*)this)->Update();
            }
            tasks();
        }
        if constexpr(Has_Draw<Derived>) {
            ((Derived*)this)->Draw();
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

    template<bool showLog = false, int timeoutSeconds = 10>
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

    template<bool showLog = false, int timeoutSeconds = 10>
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
        co_return sd;
    }

    xx::Task<xx::Shared<TexturePacker>> AsyncLoadTexturePackerFromUrl(char const* plistUrl) {
        auto plistData = co_await AsyncDownloadFromUrl(plistUrl);
        if (!plistData) co_return xx::Shared<TexturePacker>{};

        auto tp = xx::Make<TexturePacker>();
        if (int r = tp->Load(*plistData, plistUrl)) {
            throw std::logic_error(xx::ToString("parse plist file content error: r = ", r, ", url = ", plistUrl));
        }

        auto tex = co_await AsyncLoadTextureFromUrl(tp->realTextureFileName.c_str());
        if (!tex) {
            throw std::logic_error(xx::ToString("async load texturepacker's texture timeout. url = ", tp->realTextureFileName));
        }
        for (auto& f : tp->frames) {
            f->tex = tex;
        }

        co_return tp;
    }

};

template<int width_, int height_>
struct GDesign {
    static constexpr float width = width_;
    static constexpr float height = height_;
    static constexpr float width_2 = width / 2;
    static constexpr float height_2 = height / 2;
    /*
        screen design anchor point
       ┌───────────────────────────────┐
       │ 7             8             9 │
       │                               │
       │                               │
       │ 4             5             6 │
       │                               │
       │                               │
       │ 1             2             3 │
       └───────────────────────────────┘
    */
    static constexpr float x1 = -width_2;
    static constexpr float y1 = -height_2;
    static constexpr float x2 = 0.f;
    static constexpr float y2 = -height_2;
    static constexpr float x3 = width_2;
    static constexpr float y3 = -height_2;
    static constexpr float x4 = -width_2;
    static constexpr float y4 = 0.f;
    static constexpr float x5 = 0.f;
    static constexpr float y5 = 0.f;
    static constexpr float x6 = width_2;
    static constexpr float y6 = 0.f;
    static constexpr float x7 = -width_2;
    static constexpr float y7 = height_2;
    static constexpr float x8 = 0.f;
    static constexpr float y8 = height_2;
    static constexpr float x9 = width_2;
    static constexpr float y9 = height_2;
};

#endif
