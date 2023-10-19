#include <pch.h>
#include <all.h>

xx::Task<> GameLooper::MainTask() {
	{
#ifdef __EMSCRIPTEN__
		auto tp = co_await AsyncLoadTexturePackerFromUrl
#else
		auto tp = LoadTexturePacker
#endif
		("res/dungeon.blist");
		tp->GetToByPrefix(frames_gem, "icon_gem_");
		frame_button = tp->Get("button_1");
		// ...
	}

	co_await AsyncSwitchTo<SceneMainMenu>();
}
