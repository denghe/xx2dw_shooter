#include <pch.h>
#include <all.h>

#define STB_VORBIS_HEADER_ONLY
#include "extras/stb_vorbis.c"	// +20k
#define MA_NO_WAV				// -50k
#define MA_NO_FLAC				// -100k
#define MA_NO_MP3				// -50k
#define MA_NO_RESOURCE_MANAGER	// -30k
//#define MA_NO_NODE_GRAPH		// -40k	/ 1xxk
#define MA_NO_GENERATION		// -12k
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#undef STB_VORBIS_HEADER_ONLY
#include "extras/stb_vorbis.c"
#undef L
#undef C
#undef R

void SceneMainMenu::Init() {
	rootNode.Emplace()->Init();

	auto txt = rootNode->MakeChildren<Label>();
	txt->Init(2, { 0, 200 }, { 3, 3 }, { 0.5f, 0.5f }, RGBA8_White, U"XX SURVIVOR");

	auto bg = rootNode->MakeChildren<Scale9Sprite>();
	bg->Init(1, { 0, 200 }, { 1,1 }, XY{ 0.5f, 0.5f }, txt->CalcBorderSize({ 50, 20 }), gLooper.s9cfg_panel);

	auto btn = rootNode->MakeChildren<Button>();
	btn->Init(3, { 0, 0 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"New Game", [&]() {
		//gLooper.DelaySwitchTo<ScenePlay>();

		tasks.Add([]()->xx::Task<> {

			ma_engine e;
			auto r  = ma_engine_init(NULL, &e);
			if (r != MA_SUCCESS) {
				xx::CoutN("ma_engine_init failed");
			}
			ma_engine_uninit(&e);

			co_return;
		});
	});
}

void SceneMainMenu::Draw() {
	gLooper.DrawNode(rootNode);
};
