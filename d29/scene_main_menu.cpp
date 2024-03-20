#include "pch.h"
#include "looper.h"
#include "scene_main_menu.h"
#include "scene_test1.h"
#include <xx_blocklist.h>

struct Foo {
	XX_BLOCK_LIST_TO_WEAK_IMPL(Foo);
	int i{ 123 };
};

struct Bar {
	XX_BLOCK_LIST_TO_WEAK_IMPL(Bar);
	std::string name;
};


void SceneMainMenu::Init() {
	rootNode.Emplace()->Init();

	auto basePos = rootNode->size / 2;

	auto txt = rootNode->MakeChildren<Label>();
	txt->Init(2, basePos + XY{ 0, 200 }, { 3, 3 }, { 0.5f, 0.5f }, RGBA8_White, U"breakout");

	auto bg = rootNode->MakeChildren<Scale9Sprite>();
	bg->Init(1, basePos + XY{ 0, 200 }, { 5, 5 }, XY{ 0.5f, 0.5f }, txt->CalcBorderSize({ 50, 20 }) / 5, gLooper.s9cfg);

	rootNode->MakeChildren<Button>()->Init(3, basePos + XY{ 0, 0 }, { 0.5f, 0.5f }, gLooper.s9cfg, U"test1", [&]() {
		gLooper.DelaySwitchTo<Test1::Scene>();
	}, 3);


	xx::BlockList<Foo> foos;
	auto& foo = foos.Emplace();
	auto ptr = foo.ToWeak();
	if (ptr) {
		xx::CoutN("foo exists. foo.i = ", ptr().i);
	}
	foos.Remove(foo);
	if (!ptr) {
		xx::CoutN("foo does not exists.");
	}

	// todo: more test
	// todo: BlockLists
}

void SceneMainMenu::Draw() {
	gLooper.DrawNode(rootNode);
};
