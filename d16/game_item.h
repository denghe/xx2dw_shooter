#pragma once
#include <game_looper.h>

struct ItemManagerBase;

struct Item {
	ItemManagerBase* im{};
	int typeId{};		// static constexpr int cTypeId{ ??? };

	union {
		XY pos{};
		struct {
			float posX, posY;
		};
	};
	float radius{}, radians{};
	bool flipX{};

	int lineNumber{};

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
	xx::Listi32<xx::Listi32<xx::Shared<Item>>> itemss;	// index == typeId

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
