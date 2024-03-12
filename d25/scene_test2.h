#pragma once
#include "looper.h"
#include "game_monster_base.h"

// tower defense 2

enum class MoveTips : uint16_t {
	__begin, Left, Right, Up, Down, Begin, End, __end
};
constexpr std::array<std::string_view, (size_t)MoveTips::__end> MoveTips_txt{
	""sv, "left"sv, "right"sv,"up"sv,"down"sv,"begin"sv,"end"sv,
};

struct Monster2 : MonsterBase {
	static constexpr uint32_t cTypeId{ 0 };
	static constexpr XY cAnchor{ 0.5f, 0.5f };

	xx::FromTo<MoveTips> mt{};
	XY offset;
	int32_t mapPathIndex{};
	double hpBak{};

	void Init(double hp_, int32_t mapPathIndex_);
	bool Update();
	void Draw();
};

struct MapPath {
	xx::Listi32<xx::FromTo<MoveTips>> mapMoveTips;
	int32_t beginIdx{-1}, endIdx{-1};
	Vec2<> beginCRIdx{-1,-1}, endCRIdx{-1,-1};
	std::string_view name;
};

struct SceneTest2 : Scene {
	xx::Shared<Node> rootNode;
	Camera camera;
	Rnd rnd;

	Grid<Monster2> grid;

	xx::Ref<TMX::Map> map;
	xx::Listi32<xx::Ref<Frame>> mapFrames;
	xx::Listi32<MapPath> mapPaths;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
inline SceneTest2* gSceneTest2;		// init by SceneTest2::Init()
