#pragma once
#include <game_looper.h>

struct Foo : GridItemBase {
	static constexpr uint32_t cTypeId{ 3 };
	float radius{};
	void Init(float radius_) {
		radius = radius_;
	}
};

struct A : GridItemBase {
	static constexpr uint32_t cTypeId{ 0 };
	int aaa{};
	void Init() {
		xx::CoutN("A.Init()");
	}
	~A() {
		xx::CoutN("~A()");
	}
};

struct B : GridItemBase {
	static constexpr uint32_t cTypeId{ 1 };
	std::string sss;
	void Init() {
		xx::CoutN("B.Init()");
	}
	~B() {
		xx::CoutN("~B()");
	}
};

struct C : GridItemBase {
	static constexpr uint32_t cTypeId{ 2 };
	void Init() {
		xx::CoutN("C.Init()");
	}
	~C() {
		xx::CoutN("~C()");
	}
};

struct D : GridItemBase {
	static constexpr uint32_t cTypeId{ 0 };
	int val{ 1 };
	void Update() {};
};


struct SceneTest1 : Scene {
	inline static SceneTest1* instance{};			// init by Init()
	xx::Shared<Node> rootNode;
	Camera camera;

	Grid<Foo> grid;

	Grids<A, B, C> grids;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
