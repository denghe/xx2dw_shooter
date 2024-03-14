#include "pch.h"
#include "scene_test2.h"
#include "scene_main_menu.h"

#pragma region SceneTest2

void SceneTest2::Init() {
	gSceneTest2 = this;

	rootNode.Emplace()->Init();
	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy7m, gDesign.xy7a, gLooper.s9cfg, U"Back To Menu", [&]() {
		gLooper.DelaySwitchTo<SceneMainMenu>();
	});

	// ...

	// fill map context. layers name: platforms blocks path1...

	map = gLooper.map2;
	auto w = map->width;
	auto h = map->height;
	auto e = int(w * h);
	mapFrames.Resize(w * h);
	mapMaxX = gCfg.unitSize * w;
	mapMaxY = gCfg.unitSize * h;

	grids.InitAll(map->height, map->width, (int32_t)gCfg.unitSize);

	//// fill default frame
	//for (int i = 0, e = int(w * h); i < e; ++i) {
	//	mapFrames[i] = gLooper.frame_td_path;
	//}

	//// override blocks frame
	//auto layer = map->FindLayer<TMX::Layer_Tile>("blocks");
	//for (int i = 0; i < e; ++i) {
	//	auto gid = layer->gids[i];
	//	auto& gi = map->gidInfos[gid];
	//	auto& f = map->gidInfos[gid].frame;
	//	if (f) {
	//		mapFrames[i] = f;
	//	}
	//}

	//// override platforms frame
	//layer = map->FindLayer<TMX::Layer_Tile>("platforms");
	//for (int i = 0; i < e; ++i) {
	//	auto gid = layer->gids[i];
	//	auto& f = map->gidInfos[gid].frame;
	//	if (f) {
	//		mapFrames[i] = f;

	//		// make some Tower
	//		grids.MakeInit<::Tower::Arrow>(i - i / w * w, i / w);
	//	}
	//}

	// search all layer prefix == "path" create MapPath
	for (auto& ly : map->flatLayers) {
		if (ly->name.starts_with("path")) {
			if (ly->type == TMX::LayerTypes::TileLayer) {
				mapPaths.Emplace().Init(map, (TMX::Layer_Tile*)ly, gCfg.unitSize);
			} else if (ly->type == TMX::LayerTypes::ObjectLayer) {
				mapPaths.Emplace().Init(map, (TMX::Layer_Object*)ly, gCfg.unitSize);
			} else {
				assert(false);
			}
		}
	}

	camera.SetScale(2.f);
	camera.SetOriginal({ gCfg.unitSize * map->width / 2, gCfg.unitSize * map->height / 2 });
	camera.SetMaxFrameSize({ (float)gCfg.unitSize, (float)gCfg.unitSize });


	tasks.Add([this]()->xx::Task<> {
		//co_await gLooper.AsyncSleep(2);
		while (true)
		{
			for (size_t i = 0; i < 20; i++)
			{
				//if (grid.Count() >= gCfg.unitLimit) break;
				grids.MakeInit<::Enemy::Monster2>(rnd.Next<double>(gCfg.hpRange2.from, gCfg.hpRange2.to), 0);
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

	grids.ForeachAll([&]<typename T>(Grid<T>&grid) {
		grid.BufForeach([](T& o)->GridForeachResult {
			return o.Update() ? GridForeachResult::RemoveAndContinue : GridForeachResult::Continue;
		});
	});

	enm.Update();
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

	grids.ForeachAll([&]<typename T>(Grid<T>&grid) {
		grid.BufForeach([camera = &camera](T& o)->void {
			if (camera->InArea(o.pos)) {
				o.Draw();
			}
		});
	});

	enm.Draw(camera);

	auto str = xx::ToString("total item count = ", grids.Count());// , "  total blood text count = ", enm.ens.Count());
	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, str, RGBA8_Green, { 0.5f, 1 });

	gLooper.DrawNode(rootNode);
}

#pragma endregion

#pragma region Enemy.Monster2

namespace Enemy {
	void Monster2::Init(double hp_, int32_t mapPathIndex_) {
		hpBak = hp = hp_;
		hp *= (double)gSceneTest2->rnd.Next<float>(0.01f, 0.99f);
		radius = (float)std::sqrt(cRadius * cRadius / cHP * hp_);
		mapPathIndex = mapPathIndex_;
		auto& tm = gSceneTest2->mapPaths[mapPathIndex_].tm;
		assert(radius <= tm.totalWidth);
		assert(radius >= tm.trackMargin);
		auto numTrackCovered = int32_t(radius * 2 / tm.trackMargin);
		auto range = (tm.trackCount - numTrackCovered);
		trackIndex = numTrackCovered / 2 + gSceneTest2->rnd.Next<int32_t>(range);
		pointIndex = {};
		speed = cSpeed * cRadius / radius;
		radians = tm.GetRadians((int)pointIndex);
		pos = tm.GetPoint(trackIndex, (int)pointIndex);
	}

	bool Monster2::Update() {
		// todo: hit tower logic?
		auto& tm = gSceneTest2->mapPaths[mapPathIndex].tm;
		pointIndex += speed;
		if (auto c = tm.GetPointCount(); pointIndex >= c) {
			// todo: damage player? switch to another state? change to another monster?
			return true;
		}
		gSceneTest2->grids.Get<::Enemy::Monster2>().Update(*this, tm.GetPoint(trackIndex, (int)pointIndex));
		radians = tm.GetRadians((int)pointIndex);
		return false;
	}

	void Monster2::Draw() {
		auto& camera = gSceneTest2->camera;
		auto& q = Quad::DrawOnce(gLooper.frame_td_shape_circle);
		q.pos = camera.ToGLPos(pos);
		q.anchor = cAnchor;
		q.scale = XY::Make(camera.scale) * (radius / cRadius);
		q.radians = radians;
		q.colorplus = 1;
		q.color = { cColor.r, cColor.g, cColor.b, uint8_t(40 + 160 * (hp / hpBak)) };
	}
}

#pragma endregion

namespace Tower {
#pragma region Arrow

void Arrow::Init(int32_t colIdx, int32_t rowIdx) {
	pos.x = colIdx * gCfg.unitSize + gCfg.unitSize / 2;
	pos.y = rowIdx * gCfg.unitSize + gCfg.unitSize / 2;
	damage = cDamage;
}

bool Arrow::Update() {
	// check cast delay
	if (nextFireFrame < gLooper.frameNumber) {
		nextFireFrame = gLooper.frameNumber + cFireAfterDelayFrame;

		// find most dangerous enemy in attack area
		::Enemy::Monster2* tar{};
		gSceneTest2->grids.Get<::Enemy::Monster2>().ForeachByRange(gLooper.sgrdd, pos, cAttackRange, [&](::Enemy::Monster2& o) {
			if (!tar) {
				tar = &o;
			} else {
				if (o.pointIndex > tar->pointIndex) {
					tar = &o;
				}
			}
		});

		// fire
		if (tar) {
			gSceneTest2->grids.MakeInit<::Bullet::Tower::Arrow>(*this, *tar);
		}
	}
	return false; 
}

void Arrow::Draw() {
	auto& camera = gSceneTest2->camera;
	{
		auto& q = Quad::DrawOnce(gLooper.frame_td_tower_base);
		q.pos = camera.ToGLPos(pos);
		q.anchor = cAnchor;
		q.scale = XY::Make(camera.scale);
		q.radians = 0;
		q.colorplus = 1;
		q.color = RGBA8_White;
	}
	{
		auto& q = Quad::DrawOnce(gLooper.frame_td_icon_arrow1);
		q.pos = camera.ToGLPos(pos);
		q.anchor = cAnchor;
		q.scale = XY::Make(camera.scale);
		q.radians = 0;
		q.colorplus = 1;
		q.color = RGBA8_White;
	}
}

#pragma endregion

#pragma region Cannon

void Cannon::Init(int32_t colIdx, int32_t rowIdx) {
	// todo
}

bool Cannon::Update() { return false; }

void Cannon::Draw() {
	auto& camera = gSceneTest2->camera;
	{
		auto& q = Quad::DrawOnce(gLooper.frame_td_tower_base);
		q.pos = camera.ToGLPos(pos);
		q.anchor = cAnchor;
		q.scale = XY::Make(camera.scale);
		q.radians = 0;
		q.colorplus = 1;
		q.color = RGBA8_White;
	}
	{
		auto& q = Quad::DrawOnce(gLooper.frame_td_icon_cannon1);
		q.pos = camera.ToGLPos(pos);
		q.anchor = cAnchor;
		q.scale = XY::Make(camera.scale);
		q.radians = 0;
		q.colorplus = 1;
		q.color = RGBA8_White;
	}
}

#pragma endregion
}

namespace Bullet::Tower {
#pragma region Arrow

void Arrow::Init(::Tower::Arrow& owner, MonsterBase& tar) {
	damage = owner.damage;
	deathFrameNumber = gLooper.frameNumber + (int32_t)(cTimeSpan / gDesign.frameDelay);
	radius = cRadius * cScale;
	speed = cSpeed;
	pos = owner.pos;

	// calc target's pos
	auto dist = Calc::Distance(owner.pos, tar.pos);
	auto pointIndex = int(tar.pointIndex + tar.speed * (dist / cSpeed));
	auto& tm = gSceneTest2->mapPaths[tar.mapPathIndex].tm;
	if (auto c = tm.GetPointCount(); pointIndex >= c) {
		pointIndex = c - 1;
	}
	auto tarPos = tm.GetPoint(tar.trackIndex, pointIndex);
	auto d = tarPos - owner.pos;
	radians = std::atan2(d.y, d.x);
	inc.x = std::cos(radians) * speed;
	inc.y = std::sin(radians) * speed;
}

bool Arrow::Update() {
	// life span check
	if (deathFrameNumber <= gLooper.frameNumber) return true;
	
	// hit check
	bool death = false;
	gSceneTest2->grids.Get<::Enemy::Monster2>().Foreach9(pos, [&](::Enemy::Monster2& o)->GridForeachResult {
		// intersects ?
		if (Calc::Intersects::CircleCircle<float>(
			pos.x, pos.y, radius, o.pos.x, o.pos.y, o.radius)) {
			death = true;
			gSceneTest2->enm.Add(pos, pos - o.pos, {255,0,0,127}, (int32_t)damage);
			o.hp -= damage;
			if (o.hp <= 0) {
				return GridForeachResult::RemoveAndBreak;
			}
			return GridForeachResult::Break;
		}
		return GridForeachResult::Continue;
	});
	if (death) return true;

	// calc move pos
	auto newPos = pos + inc;

	// edge check
	if (newPos.x < 0 || newPos.x >= gSceneTest2->mapMaxX ||
		newPos.y < 0 || newPos.y >= gSceneTest2->mapMaxY) return true;

	// move
	gSceneTest2->grids.Update(*this, newPos);

	return false;
}

void Arrow::Draw() {
	auto& camera = gSceneTest2->camera;
	{
		// tail
		auto& q = Quad::DrawOnce(gLooper.frame_td_shape_trangle);
		q.pos = camera.ToGLPos(pos);
		q.anchor = { 0.5f, 1.f };
		q.scale = XY::Make(camera.scale) * cScale * XY { 1.f, cTailRatio };
		q.radians = radians + gPI / 2;
		q.colorplus = 1;
		q.color = RGBA8_White;
		q.color.a = 127;
	}
	{
		// body
		auto& q = Quad::DrawOnce(gLooper.frame_td_shape_circle);
		q.pos = camera.ToGLPos(pos);
		q.anchor = cAnchor;
		q.scale = XY::Make(camera.scale) * cScale;
		q.radians = radians;
		q.colorplus = 1;
		q.color = RGBA8_White;
	}
}

#pragma endregion

#pragma region Cannon

void Cannon::Init(::Tower::Cannon& owner, MonsterBase& tar) {
	// todo
	//maxHitCount = cMaxHitCount;
	//hitBlackList.Reserve(maxHitCount);
}

bool Cannon::Update() { 

	//// hit check	( lambda return false mean foreach next m )
	//gSceneTest2->grids.Get<::Enemy::Monster2>().Foreach9([&](::Enemy::Monster2& m)->bool {
	//	// intersects ?
	//	if (!Calc::Intersects::CircleCircle<float>(
	//		pos.x, pos.y, radius, m.pos.x, m.pos.y, m.radius)) return false;

	//	// black list check
	//	for (int i = hitBlackList.len - 1; i >= 0; --i) {
	//		auto& o = hitBlackList[i];

	//		if (!o.Is<BigMonster>()) {
	//			if (/*!o.Exists2() || */o.value <= scene->frameNumber) {
	//				hitBlackList.SwapRemoveAt(i);
	//			}
	//			continue;
	//		}

	//		if (!o.Exists<BigMonster>()) {
	//			hitBlackList.SwapRemoveAt(i);
	//			continue;
	//		}

	//		if (o.iv == m.iv) {
	//			// not timeout ?
	//			if (o.value > scene->frameNumber) return false;

	//			// renew timestamp
	//			o.value = scene->frameNumber + cHitDelayFrames;
	//			goto LabHit;
	//		}
	//	}

	//	// add to black list 
	//	hitBlackList.Emplace(PointerInt{ {m.typeId, m.iv}, scene->frameNumber + cHitDelayFrames });

	//LabHit:
	//	// hit effect
	//	scene->enm.Add(pos, XY{ 0, -1 }, RGBA8_Red, int(radius * 1000));
	//	--maxHitCount;
	//	return maxHitCount == 0;
	//});

	//if (maxHitCount == 0) COR_EXIT;		// todo: fade out ?
	//pos += inc;

	return false;
}
void Cannon::Draw() {}

#pragma endregion
}
