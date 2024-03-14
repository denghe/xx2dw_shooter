#include "pch.h"
#include "looper.h"
#include "scene_main_menu.h"
#include "scene_test1.h"
#include "scene_test2.h"

void SceneMainMenu::Init() {
	rootNode.Emplace()->Init();

	auto basePos = rootNode->size / 2;

	auto txt = rootNode->MakeChildren<Label>();
	txt->Init(2, basePos + XY{ 0, 200 }, { 3, 3 }, { 0.5f, 0.5f }, RGBA8_White, U"circle tower defense");

	auto bg = rootNode->MakeChildren<Scale9Sprite>();
	bg->Init(1, basePos + XY{ 0, 200 }, {1,1}, XY{ 0.5f, 0.5f }, txt->CalcBorderSize({ 50, 20 }), gLooper.s9cfg);

	rootNode->MakeChildren<Button>()->Init(3, basePos + XY{ 0, 0 }, { 0.5f, 0.5f }, gLooper.s9cfg, U"test1", [&]() {
		gLooper.DelaySwitchTo<SceneTest1>();
	});

	rootNode->MakeChildren<Button>()->Init(3, basePos + XY{ 0, -100 }, { 0.5f, 0.5f }, gLooper.s9cfg, U"test2", [&]() {
		gLooper.DelaySwitchTo<SceneTest2>();
	});

}

void SceneMainMenu::Draw() {
	gLooper.DrawNode(rootNode);
};
