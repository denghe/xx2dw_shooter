#include <pch.h>
#include <game_looper.h>


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

#define STB_VORBIS_HEADER_ONLY
#include "extras/stb_vorbis.c"	// +20k
#define MA_NO_WAV				// -50k
#define MA_NO_FLAC				// -100k
#define MA_NO_MP3				// -50k
#define MA_NO_RESOURCE_MANAGER	// -30k
#define MA_NO_NODE_GRAPH		// -40k	/ 1xxk
#define MA_NO_GENERATION		// -12k
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#undef STB_VORBIS_HEADER_ONLY
#include "extras/stb_vorbis.c"
#undef L
#undef C
#undef R

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
	gLooper.title = "xx2dw_d16";
	gLooper.clearColor = {0x33,0x33,0x33,0xff};
	gLooper.Init();
	gLooper.Run<true>();
}
#endif
