#pragma once
#include <game_looper.h>


template<typename T, typename... Args>
struct MaxTypeId {
	static const int value = T::cTypeId > MaxTypeId<Args...>::value
		? T::cTypeId
		: MaxTypeId<Args...>::value;
};

template<typename T>
struct MaxTypeId<T> {
	static const size_t value = T::cTypeId;
};

template<typename T, typename... Args>
constexpr size_t MaxTypeId_v = MaxTypeId<T, Args...>::value;


struct ItemManagerBase;

struct Item {
	int typeId{};			// static constexpr int cTypeId{ __LINE__ };
	virtual ~Item() {
		//xx::CoutN("~Item() typeId == ", typeId);
	};

	ItemManagerBase* im{};
	XX_FORCE_INLINE void ItemInit(int typeId_, ItemManagerBase* im_, bool drawable_ = false);

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
		if (camera.InArea(pos)) return pos.y;
		else return std::numeric_limits<float>::quiet_NaN();
	}
	virtual void Draw(Camera const& camera) {
		assert(drawable);
	}

	XY pos{};
	float radius{}, radians{};
};

struct ItemManagerBase {

	// index == typeId
	xx::Listi32<xx::Listi32<xx::Shared<Item>>> itemss;

	template<std::derived_from<Item> T>
	xx::Listi32<xx::Shared<T>>& GetItems() const {
		return (xx::Listi32<xx::Shared<T>>&)itemss[T::cTypeId];
	}

	// make shared + call Init( args )
	template<std::derived_from<Item> T, typename...Args>
	xx::Shared<T> Create(Args&&...args) {
		xx::Shared<T> o;
		o = xx::MakeShared<T>();
		o->Init(this, std::forward<Args>(args)...);
		return o;
	}

	template<std::derived_from<Item> T, typename F>
	void Foreatch(F&& func) {
		auto& os = GetItems<T>();
		for (auto& o : os) {
			func(o);
		}
	}

	void Clear() {
		for (auto& items : itemss) {
			items.Clear();
		}
	}
};

inline void Item::ItemInit(int typeId_, ItemManagerBase* im_, bool drawable_) {
	assert(!typeId);
	assert(typeId_);
	typeId = typeId_;
	assert(!im);
	assert(im_);
	im = im_;
	im->itemss[typeId_].Emplace(xx::SharedFromThis(this));	
	assert(!lineNumber);
	drawable = drawable_;
	//xx::CoutN("ItemInit() typeId == ", typeId);
}


template<std::derived_from<Item>...TS>
struct ItemManager : ItemManagerBase {
	void Init() {
		constexpr int maxId = MaxTypeId_v<TS...>;
		itemss.Resize(maxId + 1);
		Clear();
	}

	bool Update() {
		int i{};
		i += (UpdateCore<TS>(), ...);
		return i == 0;
	}

	template<std::derived_from<Item> T>
	int UpdateCore() {
		auto& os = GetItems<T>();
		for (int i = os.len - 1; i >= 0; --i) {
			if (os[i]->Update()) {
				os.SwapRemoveAt(i);	// known issue: crash
			}
		}
		return os.len;
	}

	template<typename F>
	void ForeatchAll(F&& func) {
		for (auto& os : itemss) {
			for (auto& o : os) {
				func(o);
			}
		}
	}
};


/*******************************************************************************************/
/*******************************************************************************************/

// bio prototype logic likely
// timer + spawm children
// timer + link to children + timer + range attack

struct Timer : Item {
	static constexpr int cTypeId{ 1 };

	xx::Weak<Item> owner;
	float cDelaySeconds{};
	float secs{};
	typedef bool (*Callback)(ItemManagerBase* im_, xx::Weak<Item> owner_);
	Callback callback;
	
	void Init(ItemManagerBase* im_, xx::Weak<Item> owner_, float cDelaySeconds_, Callback callback_) {
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
	static constexpr int cTypeId{ 2 };

	xx::Weak<Item> owner;
	float cLifeSpan{};
	float life{};
	// todo: pos, tex

	void Init(ItemManagerBase* im_, xx::Weak<Item> owner_, float cLifeSpan_) {
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
	static constexpr int cTypeId{ 3 };

	xx::Weak<Item> linkFrom, linkTo;
	float cLifeSpan{};
	float life{};
	// todo: pos, tex

	void Init(ItemManagerBase* im_, xx::Weak<Item> linkFrom_, xx::Weak<Item> linkTo_, float cLifeSpan_) {
		ItemInit(cTypeId, im_);
		linkFrom = std::move(linkFrom_);
		linkTo = std::move(linkTo_);
		cLifeSpan = cLifeSpan_;
	}

	virtual int UpdateCore() override {
		if (!linkFrom || !linkTo) return 0;
		life += gLooper.frameDelay;
		if (life > cLifeSpan) return 0;
		// todo: sync pos & hit monsters( 1 seconds hit 10 times ? )
		return 1;
	}
};

struct RangeBullet : Item {
	static constexpr int cTypeId{ 4 };

	float cLifeSpan{};
	float life{};
	// todo: pos, tex

	void Init(ItemManagerBase* im_, xx::Weak<Item> releaser, float cLifeSpan_) {
		ItemInit(cTypeId, im_);
		// todo: copy props from releaser
		cLifeSpan = cLifeSpan_;
	}

	virtual int UpdateCore() override {
		life += gLooper.frameDelay;
		if (life > cLifeSpan) return 0;
		// todo: hit monsters( 1 seconds hit 10 times ? )
		return 1;
	}
};

struct Player : Item {
	static constexpr int cTypeId{ 5 };

	// simulate read player config & create timers
	void Init(ItemManagerBase* im_) {
		ItemInit(cTypeId, im_);
		pos = {500, 500};

		// timer + spawm children ( every 1s spawn 1 child, lifecycle 11s )
		im_->Create<Timer>(xx::WeakFromThis(this), 1.f, [](ItemManagerBase* im_, xx::Weak<Item> owner_)->bool {
			assert(owner_ && owner_->typeId == Player::cTypeId);
			im_->Create<Child>(owner_, 11.f);
			return true;	// todo: total count limit
		});

		// timer + link to children( every 3s link to all child, lifecycle 5s )
		im_->Create<Timer>(xx::WeakFromThis(this), 3.f, [](ItemManagerBase* im_, xx::Weak<Item> owner_)->bool {
			assert(owner_ && owner_->typeId == Player::cTypeId);
			im_->Foreatch<Child>([&](xx::Shared<Child>& c) {
				auto&& l = im_->Create<Linker>(owner_, c.ToWeak(), 5.f);
				// linker + timer + range attack( every 2s fire bullet, lifecycle 3s )
				im_->Create<Timer>(l.ToWeak(), 2.f, [](ItemManagerBase* im_, xx::Weak<Item> owner_)->bool {
					im_->Create<RangeBullet>(owner_, 3.f);
					return true;	// todo: total count limit
				});
			});
			return true;	// todo: total count limit
		});
	}

	virtual int UpdateCore() override {
		// todo: handle player move input ? frame anim ?
		return 1;
	}
};

struct Monster : Item {
	static constexpr int cTypeId{ 6 };

	void Init(ItemManagerBase* im_) {
		ItemInit(cTypeId, im_, true);
		pos.x = gLooper.rnd.Next<float>(0, 1000);
		pos.y = gLooper.rnd.Next<float>(0, 1000);
		radians = {};
		radius = 5.f;
	}

	virtual int UpdateCore() override {
		// todo: move to player pos ? hit player ?
		return 1;
	}

	virtual void Draw(Camera const& camera) override {
		Quad()
			.SetFrame(gLooper.frame_no)
			.SetPosition(camera.ToGLPos(pos))
			.SetRotate(radians)
			.Draw();
	}
};


struct Env {
	ItemManager<Timer, Child, Linker, RangeBullet, Player, Monster> im;

	// spawm some player & monsters
	void Init() {
		im.Init();

		for (size_t i = 0; i < 1; i++) {
			im.Create<Player>();
		}
	}

	// return true mean im is empty
	bool Update() {
		for (size_t i = 0; i < 10; i++) {
			im.Create<Monster>();
		}

		return im.Update();
	}

	// for draw order by y
	struct ItemY {
		Item* item;
		float y;
	};
	xx::Listi32<ItemY> iys;

	void Draw(Camera const& camera) {
		im.ForeatchAll([&](xx::Shared<Item>& o) {
			if (o->drawable) {
				if (auto y = o->GetY(camera); !std::isnan(y)) {
					iys.Emplace(o.pointer, y);
				}
			}
		});

		std::sort(iys.buf, iys.buf + iys.len, [](auto& a, auto& b) { return a.y < b.y; });
		for (auto& iy : iys) {
			iy.item->Draw(camera);
		}
		iys.Clear();

		//im.items.Foreach([&](xx::Shared<Item>& o) {
		//	if (o->drawable) {
		//		if (auto y = o->GetY(camera); !std::isnan(y)) {
		//			o->Draw(camera);
		//		}
		//	}
		//});
	}

	void DumpInfo() {
		std::unordered_map<int, std::string> keys;
		keys[Timer::cTypeId] = "Timer";
		keys[Child::cTypeId] = "Child";
		keys[Linker::cTypeId] = "Linker";
		keys[RangeBullet::cTypeId] = "RangeBullet";
		keys[Player::cTypeId] = "Player";
		keys[Monster::cTypeId] = "Monster";
		// ...

		std::map<int, int> counters;
		im.ForeatchAll([&](xx::Shared<Item>& o) {
			++counters[o->typeId];
		});

		xx::CoutN("vvvvvvvvvvvvvvvv Env DumpInfo Begin");
		for (auto&[k,v] : counters) {
			xx::CoutN(keys[k], " num = ", v);
		}
		xx::CoutN("^^^^^^^^^^^^^^^^ Env DumpInfo End");
	}
};


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
