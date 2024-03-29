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


void SceneMainMenu::ImGuiUpdate() {
	if (showDemoWindow) {
		ImGui::ShowDemoWindow(&showDemoWindow);
	}
}

void SceneMainMenu::Init() {
	gLooper.imguiUpdate = [this] { this->ImGuiUpdate(); };

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

}

void SceneMainMenu::Draw() {
}

SceneMainMenu::~SceneMainMenu() {
	gLooper.imguiUpdate = {};
}
