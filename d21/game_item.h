#pragma once
#include <engine_camera.h>
#include <xx_ptr.h>
#include <xx_list.h>

struct ItemManagerBase;
struct Item {
	ItemManagerBase* im{};

	// static constexpr int cTypeId{ 1 ~ n inc };
	int typeId{};

	union {
		XY pos{};
		struct {
			float posX, posY;
		};
	};
	XY scale{ 1, 1 };
	float radius{}, radians{}, frameIndex{};
	bool flipX{};

	virtual ~Item() {
		//xx::CoutN("~Item() typeId == ", typeId);
	}

	// void Init(ItemManagerBase* im_) {  ItemInit(cTypeId, im_); ...
	XX_FORCE_INLINE void ItemInit(int typeId_, ItemManagerBase* im_);

	// return true mean "dead"
	virtual bool Update() {
		return false;
	};

	// if (camera.InArea(o->pos)) {
	virtual void Draw(Camera const& camera) {}
};

struct ItemManagerBase {

	xx::Listi32<xx::Listi32<xx::Shared<Item>>> itemss;	// index == typeId
	void* userData{};

	// make shared + call Init( args )
	template<std::derived_from<Item> T, typename...Args>
	xx::Shared<T> Create(Args&&...args) {
		xx::Shared<T> o;
		o = xx::MakeShared<T>();
		o->Init(this, std::forward<Args>(args)...);
		return o;
	}

	void Clear() {
		for (auto& items : itemss) {
			items.Clear();
		}
	}

	template<std::derived_from<Item> T>
	xx::Listi32<xx::Shared<T>>& Get() const {
		return (xx::Listi32<xx::Shared<T>>&)itemss[T::cTypeId];
	}

	// im.Foreach<A, B, C......>([&]<typename T>(xx::Listi32<xx::Shared<T>>& items){ ...... });
	template<std::derived_from<Item>...TS, typename F>
	void Foreach(F&& func) {
		(func(Get<TS>()), ...);
	}
};

inline void Item::ItemInit(int typeId_, ItemManagerBase* im_) {
	assert(!typeId);
	assert(typeId_);
	typeId = typeId_;
	assert(!im);
	assert(im_);
	im = im_;
	im->itemss[typeId_].Emplace(xx::SharedFromThis(this));	
	//xx::CoutN("ItemInit() typeId == ", typeId);
}

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

template<std::derived_from<Item>...TS>
struct ItemManager : ItemManagerBase {

	void Init(void* userData_ = nullptr) {
		assert(itemss.Empty());
		itemss.Resize(std::max({ TS::cTypeId... }) + 1);
		userData = userData_;
	}

	// im.ForeachAllItems([&]<typename T>(xx::Listi32<xx::Shared<T>>& items){ ...... });
	template<typename F>
	void ForeachAll(F&& func) {
		(func(Get<TS>()), ...);
	}

	// return alive num items
	int Update() {
		int i{};
		i += (UpdateCore<TS>(), ...);
		return i;
	}

	template<std::derived_from<Item> T>
	int UpdateCore() {
		auto& os = Get<T>();
		for (int i = os.len - 1; i >= 0; --i) {
			if (os[i]->Update()) {
				os.SwapRemoveAt(i);
			}
		}
		return os.len;
	}

	// dump helpers
	template<typename T>
	void DumpInfoCore() {
		auto& os = Get<T>();
		xx::CoutN(nameof::nameof_type<T>(), " num = ", os.len);
	}
	void DumpInfo() {
		xx::CoutN("********************************************");
		(DumpInfoCore<TS>(), ...);
	}

	int GetSize() {
		return (Get<TS>().len + ...);
	}

	// Draw helpers
	xx::Listi32<ItemY> iys;

	// default impl
	template<std::derived_from<Item>...US>
	void Draw(Camera const& camera) {
		if constexpr (sizeof...(US) > 0) {
			Foreach<US...>([&]<typename T>(xx::Listi32<xx::Shared<T>>&items) {
				for (auto& o : items) {
					if (camera.InArea(o->pos)) {
						iys.Emplace(o, o->posY);
					}
				}
			});
		}
		ItemY::Sort(iys);
		for (auto& iy : iys) {
			iy.item->Draw(camera);
		}
		iys.Clear();
	}

	void DrawAll(Camera const& camera) {
		Draw<TS...>(camera);
	}

	// no sort
	template<std::derived_from<Item>...US>
	void DirectDraw(Camera const& camera) {
		if constexpr (sizeof...(US) > 0) {
			Foreach<US...>([&]<typename T>(xx::Listi32<xx::Shared<T>>&items) {
				for (auto& o : items) {
					if (camera.InArea(o->pos)) {
						o->Draw(camera);
					}
				}
			});
		}
	}

	void DirectDrawAll(Camera const& camera) {
		DirectDraw<TS...>(camera);
	}
};

/*

struct SceneTest1;
using BaseItem = SceneItem<SceneTest1>;

*/
template<typename SceneType>
struct SceneItemBase : Item {
	SceneType* scene{};
	void SceneItemInit(int typeId_, ItemManagerBase* im_) {
		ItemInit(typeId_, im_);
		scene = (SceneType*)im->userData;
	}
};



/*
#pragma region XXXXXXXXXXXXX
#pragma endregion
*/
