#pragma once
#include <game_looper.h>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
using boost::multi_index_container;
using namespace boost::multi_index;

struct ItemManagerBase;

struct ItemBase {
	ItemManagerBase* im{};
	// static constexpr int cTypeId{ ??? };
	int typeId{};

	union {
		XY pos{};
		struct {
			float posX, posY;
		};
	};
	float radius{}, radians{};
	bool flipX{};

	int lineNumber{};
};

namespace tags {
	struct posY {};
}

typedef multi_index_container<ItemBase*, indexed_by<
	ordered_non_unique<tag<tags::posY>, BOOST_MULTI_INDEX_MEMBER(ItemBase, float, posY)>
>> ItemPosYs;

struct Item : ItemBase {
	ItemPosYs::iterator sortIter;

	virtual ~Item();

	XX_FORCE_INLINE void ItemInit(int typeId_, ItemManagerBase* im_);

	virtual int UpdateCore() {
		return 0;	// COR_BEGIN COR_YIELD COR_EXIT COR_END 
	};
	XX_FORCE_INLINE bool Update() {
		lineNumber = UpdateCore();
		return lineNumber == 0;		// will remove by caller
	}

	// if (camera.InArea(o->pos)) {
	virtual void Draw(Camera const& camera) {}
};

struct ItemManagerBase {

	// index == typeId
	xx::Listi32<xx::Listi32<xx::Shared<Item>>> itemss;

	// sort by y
	ItemPosYs sortContainer;

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
	void Foreach(F&& func) {
		auto& os = GetItems<T>();
		for (auto& o : os) {
			func(o);
		}
	}

	// im.ForeachEx<A, B, C......>([&]<typename T>(xx::Shared<T>& o){ ...... });
	template<std::derived_from<Item>...TS, typename F>
	void ForeachEx(F&& func) {
		(Foreach<TS>(std::forward<F>(func)), ...);
	}

	void Clear() {
		for (auto& items : itemss) {
			items.Clear();
		}
	}
};

inline void Item::ItemInit(int typeId_, ItemManagerBase* im_) {
	assert(!typeId);
	assert(typeId_);
	typeId = typeId_;
	assert(!im);
	assert(im_);
	im = im_;
	assert(!lineNumber);
	//xx::CoutN("ItemInit() typeId == ", typeId);
	im->itemss[typeId_].Emplace(xx::SharedFromThis(this));	
	auto [iter, ok] = im->sortContainer.insert(this);
	assert(ok);
	sortIter = std::move(iter);
}

inline Item::~Item() {
	//xx::CoutN("~Item() typeId == ", typeId);
	assert(sortIter != decltype(sortIter){});
	assert(im);
	im->sortContainer.erase(sortIter);
};


template<std::derived_from<Item>...TS>
struct ItemManager : ItemManagerBase {

	template<typename T>
	void DumpInfoCore() {
		auto& os = GetItems<T>();
		xx::CoutN(nameof::nameof_type<T>(), " num = ", os.len);
	}
	void DumpInfo() {
		xx::CoutN("ItemManager DumpInfo Begin vvvvvvvvvvvvvvvv");
		(DumpInfoCore<TS>(), ...);
		xx::CoutN("ItemManager DumpInfo End   ^^^^^^^^^^^^^^^^");
	}

	void Init() {
		assert(itemss.Empty());
		itemss.Resize(std::max({ TS::cTypeId... }) + 1);
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
	void ForeachAll(F&& func) {
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

	// spawm some player & monsters
	void Init() {
		im.Init();

		for (size_t i = 0; i < 1; i++) {
			im.Create<Player>();
		}

		for (size_t i = 0; i < 10000; i++) {
			im.Create<Monster>();
		}

		im.Clear();

		//auto& c = (xx::Listi32<Monster*>&)im.GetItems<Monster>();
		//std::sort(c.buf, c.buf + c.len, [](auto& a, auto& b) { return a->pos.y < b->pos.y; });
	}

	// return true mean im is empty
	bool Update() {
		return im.Update();
	}

	// for draw order by y
	struct ItemY {
		Item* item;
		float y;
	};
	xx::Listi32<ItemY> iys;

	void Draw(Camera const& camera) {

		auto&& idxs = im.sortContainer.get<tags::posY>();
		for (auto iter = idxs.begin(); iter != idxs.end(); ++iter) {
			auto ptr = (Item*)*iter;
			if (camera.InArea(ptr->pos)) {
				ptr->Draw(camera);
			}
		}

		//im.ForeachEx<Child, Linker, RangeBullet, Player, Monster>([&]<typename T>(xx::Shared<T>&o){
		//	if (camera.InArea(o->pos)) {
		//		iys.Emplace(o.pointer, o->pos.y);
		//	}
		//});
		//std::sort(iys.buf, iys.buf + iys.len, [](auto& a, auto& b) { return a.y < b.y; });
		//for (auto& iy : iys) {
		//	iy.item->Draw(camera);
		//}
		//iys.Clear();

		// todo: use boost multi index container sort by y ?

		//auto& c = (xx::Listi32<Monster*>&)im.GetItems<Monster>();
		//std::sort(c.buf, c.buf + c.len, [](auto& a, auto& b) { return a->pos.y < b->pos.y; });

		//im.ForeachEx<Child, Monster>([&]<typename T>(xx::Shared<T>&o){
		//	if (camera.InArea(o->pos)) {
		//		o->Draw(camera);	// o->T::Draw maybe faster
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
