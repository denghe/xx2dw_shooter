#pragma once
#include <game_looper.h>


template<typename U, typename T, size_t offset>
struct ECSIndex;

template<typename U, typename T, size_t offset>
struct ECSContainer {
	using Index = ECSIndex<U, T, offset>;

	struct Node : T {
		U* ptr{};
		Index& GetIndex() const {
			assert(ptr);
			return *(Index*)((char*)ptr + offset);
		}
	};
	xx::Listi32<Node> nodes;

	void Attach(U* o, Index* ecsi) {
#ifndef NDEBUG
		auto p = (char*)ecsi - offset;
		assert(p == (char*)o);
#endif
		assert(o);
		assert(ecsi->ptr == nullptr);
		assert(ecsi->idx == -1);
		ecsi->ptr = this;
		ecsi->idx = nodes.len;
		auto& n = nodes.Emplace();
		n.Node::ptr = o;
	}

	void Detach(int idx);

	void SyncIndexs();
};

template<typename U, typename T, size_t offset>
struct ECSIndex {
	using Container = ECSContainer<U, T, offset>;
	Container* ptr{};
	int idx{ -1 };

	T& Get() const {
		return ptr->nodes[idx];
	}

	void Init(Container& c, U* o) {
		c.Attach(o, this);
	}

	ECSIndex() = default;
	ECSIndex(ECSIndex const&) = delete;
	ECSIndex(ECSIndex&& o)
		: ptr(std::exchange(o.ptr, nullptr))
		, idx(std::exchange(o.idx, -1)) {}
	~ECSIndex() {
		if (ptr) {
			ptr->Detach(idx);
		}
		ptr = {};
		idx = -1;
	}
};

template<typename U, typename T, size_t offset>
void ECSContainer<U, T, offset>::Detach(int idx) {
	nodes.SwapRemoveAt(idx);
	if (idx < nodes.len) {
		nodes[idx].GetIndex().idx = idx;
	}
}

template<typename U, typename T, size_t offset>
void ECSContainer<U, T, offset>::SyncIndexs() {
	for (int i = 0, e = nodes.len; i < e; i++) {
		nodes[i].GetIndex().idx = i;
	}
}

#define ENABLE_ECS

#ifdef ENABLE_ECS
struct DrawInfo {
	union {
		XY pos{};
		struct {
			float posX, posY;
		};
	};
	float radius{}, radians{};
	bool flipX{};
};
#endif

struct ItemManagerBase;

struct ItemBase {
#ifdef ENABLE_ECS
	ECSIndex<ItemBase, DrawInfo, 0> drawInfo;
#endif
};

struct Item : ItemBase {
	ItemManagerBase* im{};
	int typeId{};		// static constexpr int cTypeId{ ??? };

#ifndef ENABLE_ECS
	union {
		XY pos{};
		struct {
			float posX, posY;
		};
	};
	float radius{}, radians{};
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
#ifdef ENABLE_ECS
	decltype(ItemBase::drawInfo)::Container ecsDrawInfo;
#endif

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

	void Init() {
		assert(itemss.Empty());
		itemss.Resize(std::max({ TS::cTypeId... }) + 1);
	}

	bool Update() {
		int i{};
		i += (UpdateCore<TS>(), ...);
#ifdef ENABLE_ECS
		auto& ns = ecsDrawInfo.nodes;
		std::sort(ns.buf, ns.buf + ns.len, [](auto& a, auto& b) { return a.pos.y < b.pos.y; });
		ecsDrawInfo.SyncIndexs();
#endif
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
