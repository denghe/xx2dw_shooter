#pragma once
#ifdef __EMSCRIPTEN__
#include "engine_engine.h"
#else
#include "engine_engine_desktop.h"
#endif

/*****************************************************************/
/*****************************************************************/
// game_looper.h example:
/*


#pragma once
#include "engine.h"

constexpr GDesign<1280, 720, 60> gDesign;

struct GameLooper : Engine<GameLooper>, decltype(gDesign) {
	void Update();
	xx::Task<> MainTask();
	void Draw();
};
extern GameLooper gLooper;


*/
/*****************************************************************/
/*****************************************************************/
// pch.h example:
/*


#pragma once
#ifndef XX2DW_SHOOTER_DESKTOP_PCH_H_
#define XX2DW_SHOOTER_DESKTOP_PCH_H_

#include <game_looper.h>

#endif


*/
/*****************************************************************/
/*****************************************************************/
// pch.cpp example:
/*


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
	gLooper.title = "xx2dw_d1";
	gLooper.Init();
	gLooper.Run();
}
#endif


*/
