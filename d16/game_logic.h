#pragma once
#include <game_item.h>

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
		owner = std::move(owner_);
		cDelaySeconds = cDelaySeconds_;
		callback = callback_;
		ItemInit(cTypeId, im_);
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
		owner = std::move(owner_);
		cLifeSpan = cLifeSpan_;
		ItemInit(cTypeId, im_);
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
		linkFrom = std::move(linkFrom_);
		linkTo = std::move(linkTo_);
		cLifeSpan = cLifeSpan_;
		ItemInit(cTypeId, im_);
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
		// todo: copy props from releaser
		cLifeSpan = cLifeSpan_;
		ItemInit(cTypeId, im_);
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
	static constexpr int cSpeed{ 5 };
	float speed{};

	// simulate read player config & create timers
	void Init(ItemManagerBase* im_) {
		pos = gLooper.windowSize_2;
		speed = cSpeed;
		ItemInit(cTypeId, im_);

		// timer + spawm children ( every 1s spawn 1 child, lifecycle 11s )
		im_->Create<Timer>(xx::WeakFromThis(this), 1.f, [](ItemManagerBase* im_, xx::Weak<Item> owner_)->bool {
			assert(owner_ && owner_->typeId == Player::cTypeId);
			im_->Create<Child>(owner_, 11.f);
			return true;	// todo: total count limit
		});

		// timer + link to children( every 3s link to all child, lifecycle 5s )
		im_->Create<Timer>(xx::WeakFromThis(this), 3.f, [](ItemManagerBase* im_, xx::Weak<Item> owner_)->bool {
			assert(owner_ && owner_->typeId == Player::cTypeId);
			im_->Foreach<Child>([&](xx::Shared<Child>& c) {
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
		if (auto r = gLooper.GetKeyboardMoveInc(); r.has_value()) {
			pos += r->second * speed;
			if (flipX && ((int)r->first & (int)MoveDirections::Right)) {
				flipX = false;
			} else if ((int)r->first & (int)MoveDirections::Left) {
				flipX = true;
			}
			//FrameControl::Forward(frameIndex, cFrameInc * (speed / cSpeed), cFrameMaxIndex);
		} else {
			//idle();
		}

		return 1;
	}

	virtual void Draw(Camera const& camera) override {
		auto& q = Quad::DrawOnce(gLooper.frame_yes);
		q.pos = camera.ToGLPos(pos);
		q.anchor = { 0.5f, 0.5f };
		q.scale = { flipX ? -2.f : 2.f, 2 };
		q.radians = radians;
		q.colorplus = 1;
		q.color = { 255, 255, 255, 255 };
	}
};

struct Monster : Item {
	static constexpr int cTypeId{ 6 };

	void Init(ItemManagerBase* im_) {
		pos.x = gLooper.rnd.Next<float>(0, gLooper.windowSize.x);
		pos.y = gLooper.rnd.Next<float>(0, gLooper.windowSize.y);
		radians = {};
		radius = 5.f;
		ItemInit(cTypeId, im_);
	}

	virtual int UpdateCore() override {
		// todo: move to player pos ? hit player ?
		posY += 0.1f;
		return 1;
	}

	virtual void Draw(Camera const& camera) override {
		//Quad().SetFrame<true>(gLooper.frame_no)					// faster than member visit
		//	.SetPosition(camera.ToGLPos(pos))
		//	.SetRotate(radians)
		//	.Draw();
		auto& q = Quad::DrawOnce(gLooper.frame_no);	// fastest
		q.pos = camera.ToGLPos(pos);
		q.anchor = { 0.5f, 0.5f };
		q.scale = { 1, 1 };
		q.radians = radians;
		q.colorplus = 1;
		q.color = { 255, 255, 255, 255 };
	}
};


struct Env {
	ItemManager<Timer, Child, Linker, RangeBullet, Player, Monster> im;

	// for draw order by y
	struct ItemY {
		Item* item;
		float y;
	};
	xx::Listi32<ItemY> iys;

	template<std::derived_from<Item> T>
	inline XX_FORCE_INLINE static void Sort(xx::Listi32<xx::Shared<T>>& items) {
		std::sort((Item**)items.buf, (Item**)items.buf + items.len, [](auto& a, auto& b) { return a->pos.y < b->pos.y; });
	}

	inline XX_FORCE_INLINE static void Sort(xx::Listi32<ItemY>& iys) {
		std::sort(iys.buf, iys.buf + iys.len, [](auto& a, auto& b) { return a.y < b.y; });
	}

	// spawm some player & monsters
	void Init() {
		im.Init();

		for (size_t i = 0; i < 1; i++) {
			im.Create<Player>();
		}
	}

	// return true mean im is empty
	bool Update() {
		for (size_t i = 0; i < 100; i++) {
			im.Create<Monster>();
		}

		return im.Update();
	}

	void Draw(Camera const& camera) {

		im.ForeachItems<Child, Linker, RangeBullet, Player, Monster>([&]<typename T>(xx::Listi32<xx::Shared<T>>& items){
			Sort(items);
			for (auto& o : items) {
				if (camera.InArea(o->pos)) {
					iys.Emplace(o.pointer, o->pos.y);
				}
			}
		});

		Sort(iys);
		for (auto& iy : iys) {
			iy.item->Draw(camera);
		}
		iys.Clear();

		//im.ForeachItems<Child, Linker, RangeBullet, Player, Monster>([&]<typename T>(xx::Listi32<xx::Shared<T>>& items) {
		//	for (auto& o : items) {
		//		if (camera.InArea(o->pos)) {
		//			o->Draw(camera);	// o->T::Draw maybe faster
		//		}
		//	}
		//});
	}
};


//enum class WeaponTypes {
//	Unknown,
//	Particle,		// fly instance
//	Wave,			// close combat shape? explosion? sound? ( fan diffusion attack )
//	Lightning,		// teleport electronic explosion ( teleport range attack )
//	Laser,			// energy / high brightness light ( line attack )
//	Burn,			// fire dot attack
//	Curse,			/Draw/ debuff attack
//	Summon			// summon friendly forces or resurrect corpses?
//};

//// 3 types: emitter, transfer, effector
