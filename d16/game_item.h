#pragma once
#include <game_looper.h>

struct Item;
struct ItemManager {
	xx::ListLink<xx::Shared<Item>, int32_t> items;
	bool Update();
};

struct Item {
	virtual ~Item() {};

	ItemManager* im{};
	XX_FORCE_INLINE void ItemInit(ItemManager* im_, bool drawable_ = false) {
		assert(!im);
		assert(im_);
		assert(!lineNumber);
		im = im_;
		im->items.Emplace(xx::SharedFromThis(this));
		drawable = drawable_;
	}

	int lineNumber{};
	virtual int UpdateCore() {
		return 0;	// COR_BEGIN COR_YIELD COR_EXIT COR_END 
	};
	XX_FORCE_INLINE bool Update() {
		lineNumber = UpdateCore();
		return lineNumber == 0;		// will remove by caller
	}

	bool drawable{};
	virtual float GetY(Camera const& camera) const {
		assert(drawable);
		//if (camera.InArea(pos)) return pos.y; else
		return std::numeric_limits<float>::quiet_NaN();
	}
	virtual void Draw(Camera const& camera) {
		assert(drawable);
	}
};

inline bool ItemManager::Update() {
	items.Foreach([&](xx::Shared<Item>& o)->bool {
		return o->Update();
	});
	return items.Empty();
}

/*******************************************************************************************/
/*******************************************************************************************/

struct Trigger : Item {
	xx::Weak<Item> owner;	// Conditions for existence

	float delaySeconds{}, openSeconds{}, closeSeconds{};
	int repeatTimes{};
	float secs{};
	bool isOpen{};

	xx::Weak<Trigger> Init(ItemManager* im_, xx::Weak<Item> owner_, float delaySeconds_ = 0, float openSeconds_ = 9999999, float closeSeconds_ = 0, int repeatTimes_ = std::numeric_limits<int>::max()) {
		ItemInit(im_);

		owner = std::move(owner_);
		delaySeconds = delaySeconds_;
		openSeconds = openSeconds_;
		closeSeconds = closeSeconds_;
		repeatTimes = repeatTimes_;
		xx::CoutN("Trigger.Init()");

		return xx::WeakFromThis(this);
	}

	virtual int UpdateCore() override {
		if (!owner) {
			xx::CoutN("Trigger.UpdateCore() end");
			return 0;
		} else {
			xx::CoutN("Trigger.UpdateCore()");
		}
		COR_BEGIN
		for (secs = 0; secs < delaySeconds; secs += gLooper.frameDelay) {
			COR_YIELD
		}
		do {
			isOpen = true;
			for (secs = 0; secs < openSeconds; secs += gLooper.frameDelay) {
				COR_YIELD
			}
			isOpen = false;
			for (secs = 0; secs < closeSeconds; secs += gLooper.frameDelay) {
				COR_YIELD
			}
		} while (--repeatTimes);
		COR_END
	}
};

/*******************************************************************************************/
/*******************************************************************************************/

struct Bullet1 : Item {
	// gLooper.frames_fireball_white
	constexpr static XY cAnchor{ 0.75f, 0.5f };
	constexpr static float cRadius{ 5.f };
	constexpr static xx::FromTo<float> cFrameIndexRange = { 0.f, 8.f };	// 0 ~ 7
	constexpr static float cFrameInc{ 12.f / gDesign.fps };
	constexpr static float cLifeCycle{ 0.5f };

	XY pos{};
	float radians{};
	float damage{};
	float lifeCycle{};
	float frameIndex{};
	// todo: speed? inc?

	void Init(ItemManager* im_, XY const& pos_, float radians_, float damage_) {
		ItemInit(im_, true);
		pos = pos_;
		radians = radians_;
		damage = damage_;
		frameIndex = cFrameIndexRange.from;
		xx::CoutN("Bullet1.Init()");
	}

	virtual int UpdateCore() override {
		lifeCycle += gDesign.frameDelay;
		if (lifeCycle > cLifeCycle) return 0;
		frameIndex += cFrameInc;
		if (frameIndex >= cFrameIndexRange.to) {
			frameIndex -= cFrameIndexRange.to - cFrameIndexRange.from;
		}
		// todo: move pos ?
		// todo: hit check ?
		return 1;
	}

	virtual void Draw(Camera const& camera) override{
		// todo
		//Quad().SetFrame(gLooper.frames_staff[60]).Draw();
		// todo: camera
		Quad().SetFrame(gLooper.frames_fireball_white[(int)frameIndex]).SetPosition(pos).Draw();
	}
};

/*******************************************************************************************/
/*******************************************************************************************/

template<typename BT = Bullet1>
struct Gun1 : Item {
	static constexpr int cQuanitity{ 5 };
	static constexpr float cMinCastDelay{ 0.1f };

	// todo: pos, radians, shot pos ...
	xx::Weak<Trigger> trigger;	// emit condition
	int quanitity{};
	float secs{};

	void Init(ItemManager* im_) {
		ItemInit(im_);

		trigger = xx::MakeShared<Trigger>()->Init(im, xx::WeakFromThis(this), 0.2f, 0.01f, 0.15f);
		quanitity = cQuanitity;
		xx::CoutN("Gun1.Init()");
	}

	virtual int UpdateCore() override {
		COR_BEGIN
		while (true) {
			assert(trigger);
			if (trigger->isOpen) {
				if (quanitity > 0) {
					--quanitity;
					{
						auto e = xx::MakeShared<BT>();
						e->Init(im);
					}
					for (secs = 0; secs < cMinCastDelay; secs += gDesign.frameDelay) {
						COR_YIELD
					}
				} else {
					xx::CoutN("Gun1.UpdateCore() end");
					COR_EXIT;
				}
			}
			COR_YIELD
		}
		COR_END
	}
};

///*******************************************************************************************/
///*******************************************************************************************/
//
//inline void TestGun1() {
//	ItemManager im;
//	xx::MakeShared<Gun1<Bullet1>>()->Init(&im);
//
//	for (int i = 0; i < gDesign.fps * 2; i++) {
//		xx::CoutN("i = ", i);
//		if (im.Update()) 
//			break;
//	}
//	xx::CoutN("after for");
//}

//enum class WeaponTypes {
//	Unknown,
//	Particle,		// fly instance
//	Wave,			// close combat shape? explosion? sound? ( fan diffusion attack )
//	Lightning,		// teleport electronic explosion ( teleport range attack )
//	Laser,			// energy / high brightness light ( line attack )
//	Burn,			// fire dot attack
//	Curse,			// debuff attack
//	Summon			// summon friendly forces or resurrect corpses?
//};

//// 3 types: emitter, transfer, effector
