#include <pch.h>
#ifndef __EMSCRIPTEN__

#define GLAD_MALLOC(sz)       malloc(sz)
#define GLAD_FREE(ptr)        free(ptr)
#define GLAD_GL_IMPLEMENTATION
#include <glad.h>

//#define STBI_NO_JPEG
//#define STBI_NO_PNG
#define STBI_NO_GIF
#define STBI_NO_PSD
#define STBI_NO_PIC
#define STBI_NO_PNM
#define STBI_NO_HDR
#define STBI_NO_TGA
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#endif

GameLooper gLooper;

#ifdef __EMSCRIPTEN__
int32_t main() {
	emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
		return gLooper.JsLoopCallback(ms);
	}, nullptr);
}
#else
int32_t main() {
	gLooper.showFps = true;
	gLooper.title = "xx2dw_d9";
	gLooper.Init();
	gLooper.Run();
}
#endif
