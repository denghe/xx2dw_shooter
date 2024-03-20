#include "pch.h"
#include "looper.h"
#include "scene_main_menu.h"
#include "map_path.h"

ResFrames& gRes(gLooper.res);

xx::Task<> Looper::MainTask() {

	// begin load / download textures
	std::vector<std::pair<std::string, xx::Ref<Frame>*>> ffs;
	std::string resRoot = "map_td3/";
	std::string picRoot = resRoot + "pics/";

	co_await res.AsyncLoad(ffs, picRoot, 1024);

	// load stages tiled map data. layer names:  map,  fg1, fg2,  path
	std::vector<std::string> mapStagePaths = {
		resRoot + "stage1.bmx",
		resRoot + "stage2.bmx"
		// ...
	};

	for (auto& s : mapStagePaths) {
#ifdef __EMSCRIPTEN__
		map_stages.Emplace(co_await AsyncLoadTiledMapFromUrl<true>(s));
#else
		map_stages.Emplace(LoadTiledMap<true>(s));
#endif
	}

	// begin mapping frames to gidInfo

	// list to map
	std::unordered_map<std::string, xx::Ref<Frame>*> ffm;
	for (auto& o : ffs) {
		ffm.emplace(o.first.substr(resRoot.size()), o.second);		// format: "pics/xxxx.png"
	}

	// fill gi.frame
	for (auto& ms : map_stages) {
		for (auto& gi : ms->gidInfos) {
			if (!gi) continue;
			auto it = ffm.find(gi.image->source);
			assert(it != ffm.end());
			gi.frame = *it->second;
		}
	}

	// phys init
	sgrdd.Init(128, 32);

	// ui cfg init
	s9cfg.frame = res.td_ui_border;
	s9cfg.texScale = { 1, 1 };
	s9cfg.center = { 1, 1, 1, 1 };
	s9cfg.color = { 0x55, 0x55, 0x55, 0xff };

	// load first scene
	co_await AsyncSwitchTo<SceneMainMenu>();
}
