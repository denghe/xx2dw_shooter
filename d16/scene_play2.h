#pragma once
#include <game_looper.h>
#include <game_item.h>

struct Human : Item {
	static constexpr int cTypeId{ 1 };

	constexpr static float cIdleScaleYFrom{ 0.9f };
	constexpr static float cIdleScaleYTo{ 1.f };
	constexpr static float cIdleScaleYStep{ (cIdleScaleYTo - cIdleScaleYFrom) * 2 / gDesign.fps };

	constexpr static XY cAnchor{ 0.5f, 0.15f };
	constexpr static float cRadius{ 6.f };
	constexpr static std::array<float, 5> cFrameIndexRanges = { 0.f, 3.f, 6.f, 9.f, 12.f };
	constexpr static float cFrameInc{ 12.f / gDesign.fps };
	constexpr static float cSpeed{ 30.f / gDesign.fps };

	float speed{};
	MoveDirections direction{};
	float frameIndexFrom{}, frameIndexTo{};
	void SetDirection(MoveDirections d);

	xx::Task<> mainTask;
	xx::Task<> MainTask();
	xx::Task<> idleTask;
	xx::Task<> IdleTask();

	void Init(ItemManagerBase* im_);
	virtual int UpdateCore() override;
	virtual void Draw(Camera const& camera) override;
};

struct Slime : Item {
	static constexpr int cTypeId{ 2 };
	xx::Task<> mainTask;
	xx::Task<> MainTask();
	void Init(ItemManagerBase* im_);
	virtual int UpdateCore() override;
	virtual void Draw(Camera const& camera) override;
};

// for draw order by y
struct ItemY {
	Item* item;
	float y;
};

struct ScenePlay2 : Scene {
	xx::Shared<Node> rootNode;

	ItemManager<Human, Slime> im;
	Camera camera;
	xx::Listi32<ItemY> iys;

	template<std::derived_from<Item> T>
	inline XX_FORCE_INLINE static void Sort(xx::Listi32<xx::Shared<T>>& items) {
		std::sort((Item**)items.buf, (Item**)items.buf + items.len, [](auto& a, auto& b) { return a->pos.y < b->pos.y; });
	}

	inline XX_FORCE_INLINE static void Sort(xx::Listi32<ItemY>& iys) {
		std::sort(iys.buf, iys.buf + iys.len, [](auto& a, auto& b) { return a.y < b.y; });
	}

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
