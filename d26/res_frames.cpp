#include "pch.h"
#include "res_frames.h"

xx::Task<> ResFrames::AsyncLoad(std::string picRoot, int32_t texSiz) {
	std::vector<std::pair<std::string, xx::Ref<Frame>*>> ffs;
	co_return co_await AsyncLoad(ffs, picRoot, texSiz);
}

xx::Task<> ResFrames::AsyncLoad(std::vector<std::pair<std::string, xx::Ref<Frame>*>>& ffs, std::string picRoot, int32_t texSiz) {
	ffs.emplace_back(picRoot + "bug_head1.png", &bug_head1);
	ffs.emplace_back(picRoot + "bug_body.png", &bug_body);
	ffs.emplace_back(picRoot + "bug_tail.png", &bug_tail);
	ffs.emplace_back(picRoot + "button.png", &button);

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
