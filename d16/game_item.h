#pragma once
#include <game_looper.h>
#include <xx_ecs.h>
//#define ENABLE_ECS

// tips: handle ecs props in coroutine is unsafe. need every time GetDrawInfo()

struct DrawInfo {
	union {
		XY pos{};
		struct {
			float posX, posY;
		};
	};
	XY scale{ 1, 1 };
	float radius{}, radians{}, frameIndex{};
	bool flipX{};
};

struct ItemManagerBase;

struct ItemBase {
#ifdef ENABLE_ECS
	xx::ECSIndex<ItemBase, DrawInfo, 0> drawInfo;
#endif
};

struct Item
#ifdef ENABLE_ECS
	: ItemBase
#endif
{
	ItemManagerBase* im{};
	int typeId{};		// static constexpr int cTypeId{ 1 ~ n inc };
	int lineNumber{};

#ifndef ENABLE_ECS
	union {
		XY pos{};
		struct {
			float posX, posY;
		};
	};
	XY scale{ 1, 1 };
	float radius{}, radians{}, frameIndex{};
	bool flipX{};
#endif

#ifdef ENABLE_ECS
	DrawInfo& GetDrawInfo() const {
		return drawInfo.Get();
	}
#else
	Item& GetDrawInfo() const {
		return (Item&)*this;
	}
#endif

	virtual ~Item();

	// void Init(ItemManagerBase* im_) {  ItemInit(cTypeId, im_); ...
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
#ifdef ENABLE_ECS
	decltype(ItemBase::drawInfo)::Container ecsDrawInfo;
#endif

	xx::Listi32<xx::Listi32<xx::Shared<Item>>> itemss;	// index == typeId
	void* userData{};

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

	// im.ForeachItems<A, B, C......>([&]<typename T>(xx::Listi32<xx::Shared<T>>& items){ ...... });
	template<std::derived_from<Item>...TS, typename F>
	void ForeachItems(F&& func) {
		(func(GetItems<TS>()), ...);
	}

	void Clear() {
		for (auto& items : itemss) {
			items.Clear();
		}
	}

	// todo: remove item
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

#ifdef ENABLE_ECS
	drawInfo.Init(im->ecsDrawInfo, this);
#endif
}

inline Item::~Item() {
	//xx::CoutN("~Item() typeId == ", typeId);
};

template<std::derived_from<Item>...TS>
struct ItemManager : ItemManagerBase {

	template<typename T>
	void DumpInfoCore() {
		auto& os = GetItems<T>();
		xx::CoutN(nameof::nameof_type<T>(), " num = ", os.len);
	}
	void DumpInfo() {
		xx::CoutN("********************************************");
		(DumpInfoCore<TS>(), ...);
	}

	void Init(void* userData_ = nullptr) {
		assert(itemss.Empty());
		itemss.Resize(std::max({ TS::cTypeId... }) + 1);
		userData = userData_;
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

#ifdef ENABLE_ECS
	void SortEcsDrawInfo() {
		auto& ns = ecsDrawInfo.nodes;
		std::sort(ns.buf, ns.buf + ns.len, [](auto& a, auto& b) { return a.pos.y < b.pos.y; });
		ecsDrawInfo.UpdateIndexs();
	}
#endif

	template<typename F>
	void ForeachAll(F&& func) {
		for (auto& os : itemss) {
			for (auto& o : os) {
				func(o);
			}
		}
	}

	// im.ForeachAllItems([&]<typename T>(xx::Listi32<xx::Shared<T>>& items){ ...... });
	template<typename F>
	void ForeachAllItems(F&& func) {
		(func(GetItems<TS>()), ...);
	}

	// todo ForeachAllItemsExclude
};

// for draw order by y
struct ItemY {
	Item* item;
	float y;

	template<std::derived_from<Item> T>
	inline XX_FORCE_INLINE static void Sort(xx::Listi32<xx::Shared<T>>& items) {
		std::sort((Item**)items.buf, (Item**)items.buf + items.len, [](auto& a, auto& b) { return a->pos.y < b->pos.y; });
	}

	inline XX_FORCE_INLINE static void Sort(xx::Listi32<ItemY>& iys) {
		std::sort(iys.buf, iys.buf + iys.len, [](auto& a, auto& b) { return a.y < b.y; });
	}
};
