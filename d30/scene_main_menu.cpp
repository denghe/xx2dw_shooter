#include "pch.h"
#include "looper.h"
#include "scene_main_menu.h"
#include "scene_test1.h"
#include "xx_spaceab.h"

struct Foo {
	xx::FromTo<XY> aabb;

	void Init(XY const& pos, XY const& size) {
		auto s2 = size / 2;
		aabb.from = pos - s2;
		aabb.to = pos + s2;
	}

	void MoveBy(XY const& offset) {
		aabb.from += offset;
		aabb.to += offset;
	}
};


void SceneMainMenu::Init() {

	xx::SpaceABGrid<Foo> sg;
	sg.Init(10, 10, 10);

	XY pos{ 5,5 }, siz{ 10, 10 };
	auto& f = sg.EmplaceInit(pos, siz);
	auto& o = xx::SpaceABNode<Foo>::From(f);
	xx::CoutN("from ", o.crIdx.from, " to ", o.crIdx.to);

	xx::SpaceABWeak<Foo> w(f);
	xx::CoutN("w exists? ", !!w);

	f.MoveBy(siz);
	sg.Update(f);
	xx::CoutN("from ", o.crIdx.from, " to ", o.crIdx.to);

	sg.Remove(f);
	xx::CoutN("w exists? ", !!w);





	//rootNode.Emplace()->Init();

	//auto basePos = rootNode->size / 2;

	//auto txt = rootNode->MakeChildren<Label>();
	//txt->Init(2, basePos + XY{ 0, 200 }, { 3, 3 }, { 0.5f, 0.5f }, RGBA8_White, U"astar tests");

	//auto bg = rootNode->MakeChildren<Scale9Sprite>();
	//bg->Init(1, basePos + XY{ 0, 200 }, { 5, 5 }, XY{ 0.5f, 0.5f }, txt->CalcBorderSize({ 50, 20 }) / 5, gLooper.s9cfg);

	//rootNode->MakeChildren<Button>()->Init(3, basePos + XY{ 0, 0 }, { 0.5f, 0.5f }, gLooper.s9cfg, U"test1", [&]() {
	//	gLooper.DelaySwitchTo<Test1::Scene>();
	//}, 3);
}

void SceneMainMenu::Draw() {
	//gLooper.DrawNode(rootNode);
};
