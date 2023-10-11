#pragma once

#include <xx_task.h>
#include <xx_queue.h>
#include <xx_string.h>
#include <xx_data.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/fetch.h>
#include <webgl/webgl2.h>
#include <engine_js_funcs.h>
#else
#  ifdef __ANDROID__
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#  else
#include <glad.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_NATIVE_INCLUDE_NONE
#include <GLFW/glfw3native.h>
#  endif

#define EM_BOOL int
#define EM_TRUE 1
#define EM_FALSE 0
#define EM_UTF8 char

struct EmscriptenMouseEvent {
	unsigned short button;
	long targetX;
	long targetY;
};

struct EmscriptenKeyboardEvent {
	unsigned long which;
};

#define EMSCRIPTEN_WEBGL_CONTEXT_HANDLE void*

float upload_unicode_char_to_texture(int charSize, char const* buf);
void init_gCanvas(int charSize, int width, int height, const char* font);

#endif
