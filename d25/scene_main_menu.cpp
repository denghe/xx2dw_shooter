#include "pch.h"
#include "looper.h"
#include "scene_main_menu.h"
#include "scene_editor.h"
#include "scene_test1.h"

void SceneMainMenu::Init() {
	rootNode.Emplace()->Init();

	auto basePos = rootNode->size / 2;

	auto txt = rootNode->MakeChildren<Label>();
	txt->Init(2, basePos + XY{ 0, 200 }, { 3, 3 }, { 0.5f, 0.5f }, RGBA8_White, U"circle tower defense");

	auto bg = rootNode->MakeChildren<Scale9Sprite>();
	bg->Init(1, basePos + XY{ 0, 200 }, {1,1}, XY{ 0.5f, 0.5f }, txt->CalcBorderSize({ 50, 20 }), gLooper.s9cfg_panel);

	auto btn = rootNode->MakeChildren<Button>();
	btn->Init(3, basePos + XY{ 0, 0 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"Map Editor", [&]() {
		gLooper.DelaySwitchTo<SceneEditor>();
	});

	btn = rootNode->MakeChildren<Button>();
	btn->Init(3, basePos + XY{ 0, -100 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"100000 monster grid test", [&]() {
		gLooper.DelaySwitchTo<SceneTest1>();
	});
}

void SceneMainMenu::Draw() {
	gLooper.DrawNode(rootNode);
};
