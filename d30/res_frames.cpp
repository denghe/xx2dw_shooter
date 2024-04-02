#include "pch.h"
#include "res_frames.h"

xx::Task<> ResFrames::AsyncLoad(std::string picRoot, int32_t texSiz) {
	std::vector<std::pair<std::string, xx::Ref<Frame>*>> ffs;
	co_return co_await AsyncLoad(ffs, picRoot, texSiz);
}

xx::Task<> ResFrames::AsyncLoad(std::vector<std::pair<std::string, xx::Ref<Frame>*>>& ffs, std::string picRoot, int32_t texSiz) {
	ffs.emplace_back(picRoot + "egg_black.png", &egg_black);
	ffs.emplace_back(picRoot + "egg_blue.png", &egg_blue);
	ffs.emplace_back(picRoot + "egg_cyan.png", &egg_cyan);
	ffs.emplace_back(picRoot + "egg_green.png", &egg_green);
	ffs.emplace_back(picRoot + "egg_orange.png", &egg_orange);
	ffs.emplace_back(picRoot + "egg_purple.png", &egg_purple);
	ffs.emplace_back(picRoot + "egg_red.png", &egg_red);
	ffs.emplace_back(picRoot + "egg_white.png", &egg_white);
	ffs.emplace_back(picRoot + "egg_yellow.png", &egg_yellow);
	ffs.emplace_back(picRoot + "ui_border.png", &ui_border);

	// load / download
#ifdef __EMSCRIPTEN__
	int32_t downloadCount{};
#endif
	for (auto& ff : ffs) {
#ifdef __EMSCRIPTEN__
		EngineBase3::Instance().tasks.Add([pff = &ff, &downloadCount, this]()->xx::Task<> {
			auto& ff = *pff;
			*ff.second = co_await EngineBase3::Instance().AsyncLoadFrameFromUrl(ff.first);
			++downloadCount;
		});
#else
		*ff.second = EngineBase3::Instance().LoadFrame(ff.first);
#endif
	}
#ifdef __EMSCRIPTEN__
	while (downloadCount < ffs.size()) co_yield 0;
#endif

	// batch combine textures
	if (texSiz) {
		auto ok = DynamicTexturePacker<>::Pack(ffs, texSiz);
		assert(ok);
	}

	co_return;
}
