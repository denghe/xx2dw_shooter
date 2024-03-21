#include "pch.h"
#include "looper.h"
#include "scene_main_menu.h"
#include "scene_test1.h"
#include <xx_blocklist.h>

struct Base {
	Base() = default;
	Base(Base const&) = delete;
	Base& operator=(Base const&) = delete;
	virtual ~Base() {}
};

struct Foo : Base {
	static constexpr uint32_t cTypeId{ 0 };
	int i{ 123 };

	XX_BLOCK_LIST_TO_WEAK_IMPL(Foo);
};

struct Bar : Base {
	static constexpr uint32_t cTypeId{ 1 };
	std::string name{ "asdf" };

	XX_BLOCK_LIST_TO_WEAK_IMPL(Bar);
};


struct Level {
	int i{};

	xx::Task<> main = Main();
	xx::Task<> Main() {
		do {
			play();
			co_yield 0;
		} while (i < 10);

		xx::CoutN(i);

		do {
			score();
			co_yield 0;
		} while (i > 0);

		xx::CoutN(i);
	}

	xx::Task<> play = Play();
	xx::Task<> Play() {
		while (true) {
			++i;
			co_yield 0;
		}
	}

	xx::Task<> score = Score();
	xx::Task<> Score() {
		while (true) {
			--i;
			co_yield 0;
		}
	}

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


	//xx::BlockList<Foo> foos;
	//auto& foo = foos.Emplace();
	//auto ptr = foo.ToWeak();
	//if (ptr) {
	//	xx::CoutN("foo exists. foo.i = ", ptr().i);
	//}
	//foos.Foreach([](Foo& o) {
	//	xx::CoutN("Foreach foo.i = ", o.i);
	//});
	//foos.Remove(foo);
	//if (!ptr) {
	//	xx::CoutN("foo does not exists.");
	//}
	//foos.Foreach([](Foo& o) {
	//	xx::CoutN("foo.i = ", o.i);
	//});


	//Level level;
	//while (!level.main.Resume()) {}

	xx::BlockLists<Base, xx::BlockListNodeBase, Foo, Bar> bls;
	auto& foo = bls.Get<Foo>().Emplace();
	auto& bar = bls.Get<Bar>().Emplace();
	auto fooPtr = xx::BlockListsWeak<Base, xx::BlockListNodeBase>::Make(foo);
	auto barPtr = xx::BlockListsWeak<Base, xx::BlockListNodeBase>::Make(bar);
	if (fooPtr) {
		xx::CoutN("foo exists. foo.i = ", fooPtr.As<Foo>().i);
	}
	if (barPtr) {
		xx::CoutN("bar exists. bar.name = ", barPtr.As<Bar>().name);
	}
	bls.Remove(foo);
	bls.Remove(barPtr);
	if (fooPtr) {
		xx::CoutN("foo exists. foo.i = ", fooPtr.As<Foo>().i);
	}
	if (barPtr) {
		xx::CoutN("bar exists. bar.name = ", barPtr.As<Bar>().name);
	}


}

void SceneMainMenu::Draw() {
	gLooper.DrawNode(rootNode);
};
