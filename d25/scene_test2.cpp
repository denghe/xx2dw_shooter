#include "pch.h"
#include "cfg.h"
#include "scene_test2.h"
#include "scene_main_menu.h"

#pragma region Monster2

void Monster2::Init(double hp_, int32_t mapPathIndex_) {
	auto& rnd = gSceneTest2->rnd;
	hpBak = hp = hp_;
	radius = (float)std::sqrt(cRadius * cRadius / cHP * hp_);
	speed = cSpeed * cRadius / radius;
	radians = {};

	mapPathIndex = mapPathIndex_;
	auto& mp = gSceneTest2->mapPaths[mapPathIndex_];

	auto range = (float)(gCfg.unitSize * (1 - hp / cHP) / 2);
	offset.x = gCfg.unitSize / 2 + rnd.Next<float>(-range, range);
	offset.y = gCfg.unitSize / 2 + rnd.Next<float>(-range, range);
	assert(offset.x > 0 && offset.y > 0);
	pos.x = mp.beginCRIdx.x * gCfg.unitSize + 0.00001f + offset.x;
	pos.y = mp.beginCRIdx.y * gCfg.unitSize + 0.00001f + offset.y;

	mt = mp.mapMoveTips[mp.beginIdx];
	//hp *= (double)rnd.Next<float>(0.01f, 0.99f);	// simulate not full blood
}

bool Monster2::Update() {
	auto rowIdx1 = (int32_t)(pos.y / gCfg.unitSize);
	auto colIdx1 = (int32_t)(pos.x / gCfg.unitSize);

	auto p = pos - offset;
	auto rowIdx2 = (int32_t)(p.y / gCfg.unitSize);
	auto colIdx2 = (int32_t)(p.x / gCfg.unitSize);

	if (rowIdx1 == rowIdx2 && colIdx1 == colIdx2) {
		auto mtIdx = rowIdx1 * gSceneTest2->map->width + colIdx1;
		mt = gSceneTest2->mapPaths[mapPathIndex].mapMoveTips[mtIdx];
	}

	if ((uint16_t)mt.from || (uint16_t)mt.to) {
		switch (mt.to) {
		case MoveTips::Left: {
			gSceneTest2->grid.Update(*this, { pos.x - speed, pos.y });
			break;
		}
		case MoveTips::Right: {
			gSceneTest2->grid.Update(*this, { pos.x + speed, pos.y });
			break;
		}
		case MoveTips::Up: {
			gSceneTest2->grid.Update(*this, { pos.x, pos.y - speed });
			break;
		}
		case MoveTips::Down: {
			gSceneTest2->grid.Update(*this, { pos.x, pos.y + speed });
			break;
		}
		case MoveTips::End: {
			// todo: end pos check
			switch (mt.from) {
			case MoveTips::Left: {
				gSceneTest2->grid.Update(*this, { pos.x + speed, pos.y });
				break;
			}
			case MoveTips::Right: {
				gSceneTest2->grid.Update(*this, { pos.x - speed, pos.y });
				break;
			}
			case MoveTips::Up: {
				gSceneTest2->grid.Update(*this, { pos.x, pos.y + speed });
				break;
			}
			case MoveTips::Down: {
				gSceneTest2->grid.Update(*this, { pos.x, pos.y - speed });
				break;
			}
			}
			break;
		}
		}
	} else {
		// todo: free attack mode?
		return true;
	}
	return false;
}

void Monster2::Draw() {
	auto& camera = gSceneTest2->camera;
	auto& q = Quad::DrawOnce(gLooper.frame_circle);
	q.pos = camera.ToGLPos(pos);
	q.anchor = cAnchor;
	q.scale = XY::Make(camera.scale) * (radius / cRadius);
	q.radians = radians;
	q.colorplus = 1;
	q.color = { cColor.r, cColor.g, cColor.b, uint8_t(40 + 160 * (hp / hpBak)) };
}

#pragma endregion

#pragma region SceneTest2

void SceneTest2::Init() {
	gSceneTest2 = this;

	rootNode.Emplace()->Init();
	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy7m, gDesign.xy7a, gLooper.s9cfg_btn, U"Back To Menu", [&]() {
		gLooper.DelaySwitchTo<SceneMainMenu>();
	});

	// ...

	// fill map context. layers name: platforms blocks path1...

	map = gLooper.map2;
	auto w = map->width;
	auto h = map->height;
	auto e = int(w * h);
	mapFrames.Resize(w * h);

	grid.Init(map->height, map->width, (int32_t)gCfg.unitSize);

	// fill default frame
	for (int i = 0, e = int(w * h); i < e; ++i) {
		mapFrames[i] = gLooper.frame_td_path;
	}

	// override blocks frame
	auto layer = map->FindLayer<TMX::Layer_Tile>("blocks");
	for (int i = 0; i < e; ++i) {
		auto gid = layer->gids[i];
		auto& gi = map->gidInfos[gid];
		auto& f = map->gidInfos[gid].frame;
		if (f) {
			mapFrames[i] = f;
		}
	}

	// override platforms frame
	layer = map->FindLayer<TMX::Layer_Tile>("platforms");
	for (int i = 0; i < e; ++i) {
		auto gid = layer->gids[i];
		auto& f = map->gidInfos[gid].frame;
		if (f) {
			mapFrames[i] = f;
		}
	}

	// remove prefix & suffix, convert name to MoveTips and fill to gi.ud
	static constexpr auto prefix = "td_path_"sv;
	static constexpr auto suffix = ".png"sv;
	for (auto& gi : map->gidInfos) {
		if (!gi) continue;
		if (gi.image->source.starts_with(prefix)) {
			auto& mt = (xx::FromTo<MoveTips>&)gi.ud;
			std::string_view sv(gi.image->source.c_str() + prefix.size(), gi.image->source.size() - prefix.size() - suffix.size());
			auto idx = sv.find('_');
			auto sv1 = sv.substr(0, idx);
			auto sv2 = sv.substr(idx + 1, sv.size() - idx - 1);

			auto it = std::find(MoveTips_txt.begin(), MoveTips_txt.end(), sv1);
			assert(it != MoveTips_txt.end());
			mt.from = (MoveTips)std::distance(MoveTips_txt.begin(), it);
			
			it = std::find(MoveTips_txt.begin(), MoveTips_txt.end(), sv2);
			assert(it != MoveTips_txt.end());
			mt.to = (MoveTips)std::distance(MoveTips_txt.begin(), it);

			//xx::CoutN(sv, " ", MoveTips_txt[(uint16_t)mt.from], " ", MoveTips_txt[(uint16_t)mt.to]);
		}
	}

	// search all layer prefix == "path" create MapPath
	for (auto& ly : map->flatLayers) {
		if (ly->name.starts_with("path")) {
			assert(ly->type == TMX::LayerTypes::TileLayer);
			layer = (TMX::Layer_Tile*)ly;
			auto& mp = mapPaths.Emplace();
			mp.name = layer->name;
			mp.mapMoveTips.Resize((int)layer->gids.size());
			for (int i = 0, e = (int)layer->gids.size(); i < e; ++i) {
				if (auto gid = layer->gids[i]) {
					auto& gi = map->gidInfos[gid];
					auto& mt = (xx::FromTo<MoveTips>&)gi.ud;
					mp.mapMoveTips[i] = mt;
					if (mt.from == MoveTips::Begin) {
						mp.beginIdx = i;
						mp.beginCRIdx.y = i / map->width;
						mp.beginCRIdx.x = i - mp.beginCRIdx.y * map->width;
					} else if (mt.to == MoveTips::End) {
						mp.endIdx = i;
						mp.endCRIdx.y = i / map->width;
						mp.endCRIdx.x = i - mp.endCRIdx.y * map->width;
					}
					assert((uint16_t)mt.from > (uint16_t)MoveTips::__begin);
					assert((uint16_t)mt.to < (uint16_t)MoveTips::__end);
				}
			}
			assert(mp.beginIdx >= 0 && mp.endIdx >= 0);
		}
	}

	camera.SetScale(2.f);
	camera.SetOriginal({ gCfg.unitSize * map->width / 2, gCfg.unitSize * map->height / 2 });
	camera.SetMaxFrameSize({ (float)gCfg.unitSize, (float)gCfg.unitSize });

	tasks.Add([this]()->xx::Task<> {
		//co_await gLooper.AsyncSleep(2);
		while (true)
		{
			//for (size_t i = 0; i < 10; i++)
			{
				//if (grid.Count() >= gCfg.unitLimit) break;
				grid.MakeInit(rnd.Next<double>(gCfg.hpRange2.from, gCfg.hpRange2.to), 0);
			}
			co_yield 0;
		}
	});
}

void SceneTest2::Update() {
	// scale control
	if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
		camera.IncreaseScale(0.1f, 5);
	} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
		camera.DecreaseScale(0.1f, 0.1f);
	}
	camera.Calc();

	grid.BufForeach([](Monster2& o)->GridForeachResult {
		return o.Update() ? GridForeachResult::RemoveAndContinue : GridForeachResult::Continue;
	});
}

void SceneTest2::Draw() {
	camera.Calc();

	for (int i = 0, ie = map->height; i < ie; ++i) {
		for (int j = 0, je = map->width; j < je; ++j) {
			Quad().SetFrame(mapFrames[i * je + j]).SetAnchor({ 0, 1 })
				.SetScale(camera.scale)
				.SetPosition(camera.ToGLPos(XY{ 32.f * j, 32.f * i }))
				.Draw();
		}
	}

	grid.BufForeach([camera = &camera](Monster2& o)->void {
		if (camera->InArea(o.pos)) {
			o.Draw();
		}
	});

	auto str = xx::ToString("total monster count = ", grid.Count());// , "  total blood text count = ", enm.ens.Count());
	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, str, RGBA8_Green, { 0.5f, 1 });

	gLooper.DrawNode(rootNode);
}

#pragma endregion
