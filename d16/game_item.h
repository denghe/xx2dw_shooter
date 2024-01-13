#pragma once
#include <game_looper.h>

struct Item;
struct ItemManager {
	xx::ListLink<xx::Shared<Item>, int32_t> items;
	bool Update();

	// F: void (*)( xx::Shared<T>& o )
	template<std::derived_from<Item> T, typename F>
	void Foreatch(F&& func);
};

struct Item {
	int typeId{};			// static constexpr int cTypeId{ __LINE__ };
	virtual ~Item() {
		xx::CoutN("~Item() typeId == ", typeId);
	};

	ItemManager* im{};
	XX_FORCE_INLINE void ItemInit(int typeId_, ItemManager* im_, bool drawable_ = false) {
		assert(!im);
		assert(im_);
		assert(!lineNumber);
		typeId = typeId_;
		im = im_;
		im->items.Emplace(xx::SharedFromThis(this));
		drawable = drawable_;

		xx::CoutN("ItemInit() typeId == ", typeId);
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

	// todo: more base properties like pos size / radius radians ...
};

inline bool ItemManager::Update() {
	items.Foreach([&](xx::Shared<Item>& o)->bool {
		return o->Update();
	});
	return items.Empty();
}

// F: void (*)( xx::Shared<T>& o )
template<std::derived_from<Item> T, typename F>
void ItemManager::Foreatch(F&& func) {
	items.Foreach([&](xx::Shared<Item>& o) {
		if (o->typeId == T::cTypeId) {
			func((xx::Shared<T>&)o);
		}
	});
}


/*******************************************************************************************/
/*******************************************************************************************/

// bio prototype logic likely
// timer + spawm children
// timer + link to children + timer + range attack

struct Timer : Item {
	static constexpr int cTypeId{ __LINE__ };
	xx::Weak<Item> owner;

	float cDelaySeconds{};
	float secs{};
	typedef bool (*Callback)(ItemManager* im_, xx::Weak<Item> owner_);
	Callback callback;
	
	void Init(ItemManager* im_, xx::Weak<Item> owner_, float cDelaySeconds_, Callback callback_) {
		ItemInit(cTypeId, im_);
		owner = std::move(owner_);
		cDelaySeconds = cDelaySeconds_;
		callback = callback_;
	}

	virtual int UpdateCore() override {
		if (!owner) return 0;
		COR_BEGIN
			while (true) {
				for (secs = 0; secs < cDelaySeconds; secs += gLooper.frameDelay) {
					COR_YIELD
				}
				callback(im, owner);	// todo: total count limit logic
			}
		COR_END
	}
};

struct Child : Item {
	static constexpr int cTypeId{ __LINE__ };
	xx::Weak<Item> owner;

	float cLifeSpan{};
	float life{};

	void Init(ItemManager* im_, xx::Weak<Item> owner_, float cLifeSpan_) {
		ItemInit(cTypeId, im_);
		owner = std::move(owner_);
		cLifeSpan = cLifeSpan_;
	}

	virtual int UpdateCore() override {
		if (!owner) return 0;
		COR_BEGIN
			for (; life < cLifeSpan; life += gLooper.frameDelay) {
				COR_YIELD
			}
		COR_END
	}
};

struct Linker : Item {
	static constexpr int cTypeId{ __LINE__ };
	xx::Weak<Item> linkFrom, linkTo;
	float cLifeSpan{};
	float life{};

	void Init(ItemManager* im_, xx::Weak<Item> linkFrom_, xx::Weak<Item> linkTo_, float cLifeSpan_) {
		ItemInit(cTypeId, im_);
		linkFrom = std::move(linkFrom_);
		linkTo = std::move(linkTo_);
		cLifeSpan = cLifeSpan_;
	}

	virtual int UpdateCore() override {
		if (!linkFrom || !linkTo) return 0;
		life += gLooper.frameDelay;
		if (life > cLifeSpan) return 0;
		// todo: sync pos & hit monsters
		return 1;
	}
};

struct RangeBullet : Item {
	static constexpr int cTypeId{ __LINE__ };
	float cLifeSpan{};
	float life{};

	void Init(ItemManager* im_, xx::Weak<Item> releaser, float cLifeSpan_) {
		ItemInit(cTypeId, im_);
		// todo: copy props from releaser
		cLifeSpan = cLifeSpan_;
	}

	virtual int UpdateCore() override {
		life += gLooper.frameDelay;
		if (life > cLifeSpan) return 0;
		// todo: hit monsters
		return 1;
	}
};

struct Player : Item {
	static constexpr int cTypeId{ __LINE__ };

	// simulate read player config & create timers
	void Init(ItemManager* im_) {
		ItemInit(cTypeId, im_);

		// timer + spawm children ( every 1s spawn 1 child, lifecycle 11s )
		xx::MakeShared<Timer>()->Init(im_, xx::WeakFromThis(this), 1.f, [](ItemManager* im_, xx::Weak<Item> owner_)->bool {
			assert(owner_ && owner_->typeId == Player::cTypeId);
			xx::MakeShared<Child>()->Init(im_, owner_, 11.f);
			return true;	// todo: total count limit
		});

		// timer + link to children( every 3s link to all child, lifecycle 5s )
		xx::MakeShared<Timer>()->Init(im_, xx::WeakFromThis(this), 3.f, [](ItemManager* im_, xx::Weak<Item> owner_)->bool {
			assert(owner_ && owner_->typeId == Player::cTypeId);
			im_->Foreatch<Child>([&](xx::Shared<Child>& c) {
				auto l = xx::MakeShared<Linker>();
				l->Init(im_, owner_, c.ToWeak(), 5.f);
				// linker + timer + range attack( every 2s fire bullet, lifecycle 3s )
				xx::MakeShared<Timer>()->Init(im_, l.ToWeak(), 2.f, [](ItemManager* im_, xx::Weak<Item> owner_)->bool {
					xx::MakeShared<RangeBullet>()->Init(im_, owner_, 3.f);
					return true;	// todo: total count limit
				});
			});
			return true;	// todo: total count limit
		});
	}

	virtual int UpdateCore() override {
		return 1;
	}
};

struct Monster : Item {
	static constexpr int cTypeId{ __LINE__ };

	void Init(ItemManager* im_) {
		ItemInit(cTypeId, im_);
	}

	virtual int UpdateCore() override {
		return 1;
	}
};

struct Env {
	ItemManager im;

	// spawm some player & monsters
	void Init() {
		for (size_t i = 0; i < 1; i++) {
			xx::MakeShared<Player>()->Init(&im);
		}
		for (size_t i = 0; i < 100; i++) {
			xx::MakeShared<Monster>()->Init(&im);
		}
	}

	// return true mean im is empty
	bool Update() {
		im.Update();

		// dump im info
		if (gLooper.frameNumber % (int)gDesign.fps == 0) {
			std::map<int, int> counters;
			im.items.Foreach([&](xx::Shared<Item>& o) {
				++counters[o->typeId];
				});
			xx::CoutN("counters = ", counters);
		}

		return !im.items.Empty();
	}

	void Draw() {
		// todo
	}
};









//struct Trigger : Item {
//	xx::Weak<Item> owner;	// Conditions for existence
//
//	float delaySeconds{}, openSeconds{}, closeSeconds{};
//	int repeatTimes{};
//	float secs{};
//	bool isOpen{};
//
//	xx::Weak<Trigger> Init(ItemManager* im_, xx::Weak<Item> owner_, float delaySeconds_ = 0, float openSeconds_ = 9999999, float closeSeconds_ = 0, int repeatTimes_ = std::numeric_limits<int>::max()) {
//		ItemInit(im_);
//
//		owner = std::move(owner_);
//		delaySeconds = delaySeconds_;
//		openSeconds = openSeconds_;
//		closeSeconds = closeSeconds_;
//		repeatTimes = repeatTimes_;
//		xx::CoutN("Trigger.Init()");
//
//		return xx::WeakFromThis(this);
//	}
//
//	virtual int UpdateCore() override {
//		if (!owner) {
//			xx::CoutN("Trigger.UpdateCore() end");
//			return 0;
//		} else {
//			xx::CoutN("Trigger.UpdateCore()");
//		}
//		COR_BEGIN
//		for (secs = 0; secs < delaySeconds; secs += gLooper.frameDelay) {
//			COR_YIELD
//		}
//		do {
//			isOpen = true;
//			for (secs = 0; secs < openSeconds; secs += gLooper.frameDelay) {
//				COR_YIELD
//			}
//			isOpen = false;
//			for (secs = 0; secs < closeSeconds; secs += gLooper.frameDelay) {
//				COR_YIELD
//			}
//		} while (--repeatTimes);
//		COR_END
//	}
//};
//
///*******************************************************************************************/
///*******************************************************************************************/
//
//struct Bullet1 : Item {
//	// gLooper.frames_fireball_white
//	constexpr static XY cAnchor{ 0.75f, 0.5f };
//	constexpr static float cRadius{ 5.f };
//	constexpr static xx::FromTo<float> cFrameIndexRange = { 0.f, 8.f };	// 0 ~ 7
//	constexpr static float cFrameInc{ 12.f / gDesign.fps };
//	constexpr static float cLifeCycle{ 0.5f };
//
//	XY pos{};
//	float radians{};
//	float damage{};
//	float lifeCycle{};
//	float frameIndex{};
//	// todo: speed? inc?
//
//	void Init(ItemManager* im_, XY const& pos_, float radians_, float damage_) {
//		ItemInit(im_, true);
//		pos = pos_;
//		radians = radians_;
//		damage = damage_;
//		frameIndex = cFrameIndexRange.from;
//		xx::CoutN("Bullet1.Init()");
//	}
//
//	virtual int UpdateCore() override {
//		lifeCycle += gDesign.frameDelay;
//		if (lifeCycle > cLifeCycle) return 0;
//		frameIndex += cFrameInc;
//		if (frameIndex >= cFrameIndexRange.to) {
//			frameIndex -= cFrameIndexRange.to - cFrameIndexRange.from;
//		}
//		// todo: move pos ?
//		// todo: hit check ?
//		return 1;
//	}
//
//	virtual void Draw(Camera const& camera) override{
//		// todo
//		//Quad().SetFrame(gLooper.frames_staff[60]).Draw();
//		// todo: camera
//		Quad().SetFrame(gLooper.frames_fireball_white[(int)frameIndex]).SetPosition(pos).Draw();
//	}
//};
//
///*******************************************************************************************/
///*******************************************************************************************/
//
//template<typename BT = Bullet1>
//struct Gun1 : Item {
//	static constexpr int cQuanitity{ 5 };
//	static constexpr float cMinCastDelay{ 0.1f };
//
//	// todo: pos, radians, shot pos ...
//	xx::Weak<Trigger> trigger;	// emit condition
//	int quanitity{};
//	float secs{};
//
//	void Init(ItemManager* im_) {
//		ItemInit(im_);
//
//		trigger = xx::MakeShared<Trigger>()->Init(im, xx::WeakFromThis(this), 0.2f, 0.01f, 0.15f);
//		quanitity = cQuanitity;
//		xx::CoutN("Gun1.Init()");
//	}
//
//	virtual int UpdateCore() override {
//		COR_BEGIN
//		while (true) {
//			assert(trigger);
//			if (trigger->isOpen) {
//				if (quanitity > 0) {
//					--quanitity;
//					{
//						auto e = xx::MakeShared<BT>();
//						e->Init(im);
//					}
//					for (secs = 0; secs < cMinCastDelay; secs += gDesign.frameDelay) {
//						COR_YIELD
//					}
//				} else {
//					xx::CoutN("Gun1.UpdateCore() end");
//					COR_EXIT;
//				}
//			}
//			COR_YIELD
//		}
//		COR_END
//	}
//};

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
