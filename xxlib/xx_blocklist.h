#pragma once
#include "xx_list.h"

namespace xx {

	struct BlockListNodeBase {
		uint32_t version;
		int32_t next, prev, index;
	};

	struct BlockListVI {
		uint32_t version{};
		int32_t index{ -1 };

		void Reset() {
			version = {};
			index = -1;
		}

		bool operator==(BlockListVI const& o) const {
			return index == o.index && version == o.version;
		}
	};

	template<typename T>
	struct BlockListNode : BlockListNodeBase {
		T value;
	};

	template<typename T, template<typename...> typename BlockNode = BlockListNode>
	struct BlockListWeak {
		using Node = BlockNode<T>;
		static_assert(std::is_base_of_v<BlockListNodeBase, Node>);

		T* pointer{};
		uint32_t version{};

		BlockListWeak() = default;
		BlockListWeak(BlockListWeak const&) = default;
		BlockListWeak& operator=(BlockListWeak const&) = default;
		BlockListWeak(BlockListWeak &&) = default;
		BlockListWeak& operator=(BlockListWeak &&) = default;

		BlockListWeak(T const& v) {
			operator=(v);
		}
		BlockListWeak& operator=(T const& v) {
			auto& o = *container_of(&v, Node, value);
			pointer = (T*)&v;
			version = o.version;
			return *this;
		}

		XX_FORCE_INLINE Node& RefNode() const {
			return (Node&)*container_of(pointer, Node, value);
		}

		XX_FORCE_INLINE bool Exists() const noexcept {
			return pointer && version && version == RefNode().version;
		}

		XX_FORCE_INLINE T& operator()() const {
			assert(Exists());
			return (T&)*pointer;
		}

		XX_FORCE_INLINE void Reset() {
			pointer = {};
			version = 0;
		}
	};

	template<typename T, template<typename...> typename BlockNode = BlockListNode>
	struct BlockList;


	template<typename T, template<typename...> typename BlockNode = BlockListNode>
	struct BlockListHolder {
		using OT = BlockList<T, BlockNode>;
		using WT = BlockListWeak<T, BlockNode>;

		OT* owner{};
		WT weak;

		BlockListHolder() = default;
		BlockListHolder(BlockListHolder const&) = default;
		BlockListHolder& operator=(BlockListHolder const&) = default;
		BlockListHolder(BlockListHolder&&) = default;
		BlockListHolder& operator=(BlockListHolder&&) = default;

		~BlockListHolder();

		template<typename U>
		BlockListHolder(OT& owner_, U&& u) {
			Set(owner_, u);
		}

		XX_FORCE_INLINE BlockListHolder& Set(OT& owner_, T const& v) {
			owner = &owner_;
			weak = v;
			return *this;
		}

		XX_FORCE_INLINE BlockListHolder& Set(OT& owner_, WT const& w) {
			owner = &owner_;
			weak = w;
			return *this;
		}

		XX_FORCE_INLINE BlockListHolder& Set(OT& owner_) {
			owner = &owner_;
			return *this;
		}

		XX_FORCE_INLINE BlockListHolder& Set(T const& v) {
			assert(owner);
			weak = v;
			return *this;
		}

		XX_FORCE_INLINE BlockListHolder& Set(WT const& w) {
			assert(owner);
			weak = w;
			return *this;
		}

		XX_FORCE_INLINE void Reset() {
			owner = {};
		}

		XX_FORCE_INLINE bool Exists() const noexcept {
			return owner && weak.Exists();
		}

		XX_FORCE_INLINE T& operator()() const {
			assert(Exists());
			return weak();
		}
	};

	// T has member: static constexpr int32_t cTypeId{ ?? };
	template<typename T, template<typename...> typename BlockNode>
	struct BlockList {
		using HolderType = BlockListHolder<T, BlockNode>;
		using WeakType = BlockListWeak<T, BlockNode>;
		using Node = BlockNode<T>;
		static_assert(std::is_base_of_v<BlockListNodeBase, Node>);

		struct Block {
			uint64_t flags;
			std::array<Node, 64> buf;
		};

		BlockList() = default;
		BlockList(BlockList const&) = delete;
		BlockList& operator=(BlockList const&) = delete;
		BlockList(BlockList&& o) {
			operator=(std::move(o));
		}

		BlockList& operator=(BlockList&& o) {
			std::swap(blocks, o.blocks);
			std::swap(cap, o.cap);
			std::swap(len, o.len);
			std::swap(freeHead, o.freeHead);
			std::swap(freeCount, o.freeCount);
			std::swap(head, o.head);
			std::swap(tail, o.tail);
			std::swap(version, o.version);
			return *this;
		}

		~BlockList() {
			Clear<true, true>();
		}

		// .Foreach([](T& o)->void {    });
		// .Foreach([](T& o)->xx::ForeachResult {    });
		template<bool callByClear = false, typename F>
		void Foreach(F&& func) {
			if (len <= 0) return;

			for (int32_t i = 0, n = blocks.len - 1; i <= n; ++i) {

				auto& block = *blocks[i];
				auto& flags = block.flags;
				if (!flags) continue;

				auto left = len & 0b111111;
				int32_t e = (i < n || !left) ? 64 : left;
				for (int32_t j = 0; j < e; ++j) {
					auto& o = block.buf[j];
					auto bit = uint64_t(1) << j;
					if ((flags & bit) == 0) {
						assert(!o.version);
						continue;
					}
					assert(o.version);

					if constexpr (std::is_void_v<decltype(func(o.value))>) {
						func(o.value);
						if constexpr (callByClear) {
							o.version = 0;
							o.next = -1;
							o.prev = -1;
							o.index = -1;
						}
					} else {
						auto r = func(o.value);
						switch (r) {
						case ForeachResult::Continue: break;
						case ForeachResult::RemoveAndContinue: {
							Remove(o.index);
							break;
						}
						case ForeachResult::Break: return;
						case ForeachResult::RemoveAndBreak: {
							Remove(o.index);
							return;
						}
						}
					}
				}
				if constexpr (callByClear) {
					flags = 0;
				}
			}
		}

		template<bool fromHead = true, typename F>
		void ForeachLink(F&& func, int32_t beginIdx = -1) {
			if (beginIdx == -1) {
				if constexpr (fromHead) {
					beginIdx = head;
				} else {
					beginIdx = tail;
				}
			}
			for (int32_t n, idx = beginIdx; idx != -1;) {
				auto& o = RefNode(idx);
				auto r = func(o.value);
				if constexpr (fromHead) {
					n = o.next;
				} else {
					n = o.prev;
				}
				switch (r) {
				case ForeachResult::Continue: goto LabContinue;
				case ForeachResult::RemoveAndContinue:
					Free(idx, o);
					goto LabContinue;
				case ForeachResult::Break: return;
				case ForeachResult::RemoveAndBreak:
					Free(idx, o);
					return;
				}
			LabContinue:
				idx = n;
			}
		}

		template<bool freeBuf = false, bool resetVersion = false>
		void Clear() {
			if (!cap) return;
			if constexpr (!(std::is_standard_layout_v<T> && std::is_trivial_v<T>)) {
				while (head >= 0) {
					auto& o = RefNode(head);
					o.value.~T();
					head = o.next;
				}
			}
			if constexpr (freeBuf) {
				for (auto& o : blocks) {
					::free(o);
				}
				blocks.Clear(true);
				cap = 0;
			}

			head = tail = freeHead = -1;
			freeCount = 0;
			len = 0;

			if constexpr (resetVersion) {
				version = 0;
			}
		}

		XX_FORCE_INLINE void Reserve(int32_t newCap) {
			if (newCap < cap) return;
			auto n = ((uint32_t&)newCap + 63u) >> 6;
			for (uint32_t i = 0; i < n; ++i) {
				Reserve();
			}
		}

		XX_FORCE_INLINE int32_t Count() const {
			return len - freeCount;
		}

		XX_FORCE_INLINE bool Exists(int32_t index) const {
			if (index < 0 || index >= len) return false;
			auto& o = RefNode(index);
			if (o.version) {
				assert(o.index == index);
				return true;
			}
			return false;
		}

		XX_FORCE_INLINE bool Exists(BlockListVI const& vi) const {
			if (!vi.version || vi.index < 0 || vi.index >= len) return false;
			return RefNode(vi.index).version == vi.version;
		}

		XX_FORCE_INLINE T* TryGet(BlockListVI const& vi) const {
			if (!vi.version || vi.index < 0 || vi.index >= len) return false;
			auto& o = RefNode(vi.index);
			return o.version == vi.version ? &o.value : nullptr;
		}

		template<bool appendToTail = true, typename...Args>
		T& EmplaceVI(BlockListVI& vi, Args&&... args) {
			auto index = Alloc();
			auto& o = RefNode(index);

			vi.index = index;
			vi.version = o.version = GenVersion();
			if constexpr (appendToTail) {
				o.prev = tail;
				o.next = -1;

				if (tail >= 0) {
					RefNode(tail).next = index;
					tail = index;
				} else {
					assert(head == -1);
					head = tail = index;
				}
			} else {
				o.prev = -1;
				o.next = head;

				if (head >= 0) {
					RefNode(head).prev = index;
					head = index;
				} else {
					assert(head == -1);
					head = tail = index;
				}
			}
			o.index = index;

			return *new (&o.value) T(std::forward<Args>(args)...);
		}

		template<bool appendToTail = true, typename...Args>
		T& Emplace(Args&&... args) {
			BlockListVI vi;
			return EmplaceVI<appendToTail>(vi, std::forward<Args>(args)...);
		}

		template<typename...Args>
		WeakType WeakEmplace(Args&&... args) {
			return { Emplace(std::forward<Args>(args)...) };
		}

		template<typename...Args>
		HolderType HolderEmplace(Args&&... args) {
			return { *this, Emplace(std::forward<Args>(args)...) };
		}

		void Remove(T const& v) {
			auto p = container_of(&v, Node, value);
			Remove(p->index);
		}

		void Remove(WeakType const& wt) {
			if (wt.Exists()) {
				Remove(wt.RefNode().index);
			}
		}

		void Remove(BlockListVI const& vi) {
			if (!vi.version || vi.index < 0 || vi.index >= Count()) return;
			auto& o = RefNode(vi.index);
			if (o.version != vi.version) return;
			Free(vi.index, o);
		}

		void Remove(int32_t index) {
			auto& o = RefNode(index);
			Free(index, o);
		}

		// for unknown type ( wrong type: T )
		void Remove(int32_t index, size_t tSiz, void(*deleter)(void*)) {
			auto& o = *(Node*)((char*)&RefBlock(index).buf + tSiz * index);
			Free<true>(index, o, deleter);
		}

	protected:
		XX_FORCE_INLINE int32_t Alloc() {
			int32_t index;
			if (freeCount > 0) {
				index = freeHead;
				freeHead = RefNode(index).next;
				freeCount--;
			} else {
				if (len == cap) {
					Reserve();
				}
				index = len;
				len++;
			}
			FlagSet(index);
			return index;
		}

		template<bool useDeleter = false>
		XX_FORCE_INLINE void Free(int32_t index, Node& o, void(*deleter)(void*) = {}) {
			if constexpr (useDeleter) {
				deleter(&o.value);
			} else {
				o.value.~T();
			}

			auto prev = o.prev;
			auto next = o.next;
			if (index == head) {
				head = next;
			}
			if (index == tail) {
				tail = prev;
			}
			if (prev >= 0) {
				RefNode(prev).next = next;
			}
			if (next >= 0) {
				RefNode(next).prev = prev;
			}

			assert(o.version);
			o.version = 0;
			o.next = freeHead;
			o.index = -1;
			o.prev = -1;
			freeHead = index;
			++freeCount;
			FlagUnset(index);
		}

		XX_FORCE_INLINE uint32_t GenVersion() {
			if (version == 0xFFFFFFFFu) {
				version = 1;
			} else {
				++version;
			}
			return version;
		}

		XX_FORCE_INLINE void Reserve() {
			auto b = blocks.Emplace((Block*)malloc(sizeof(Block)));
			b->flags = 0;
			cap += 64;
		}

		XX_FORCE_INLINE Block& RefBlock(int32_t index) {
			assert(index >= 0 && index < cap);
			return *blocks[(uint32_t&)index >> 6];
		}
		XX_FORCE_INLINE Block const& RefBlock(int32_t index) const {
			assert(index >= 0 && index < cap);
			return *blocks[(uint32_t&)index >> 6];
		}

		XX_FORCE_INLINE Node& RefNode(int32_t index) {
			return RefBlock(index).buf[index & 0b111111];
		}
		XX_FORCE_INLINE Node const& RefNode(int32_t index) const {
			return RefBlock(index).buf[index & 0b111111];
		}

		XX_FORCE_INLINE void FlagSet(int32_t index) {
			auto& block = RefBlock(index);
			auto bit = uint64_t(1) << (index & 0b111111);
			assert((block.flags & bit) == 0);
			block.flags |= bit;
		}

		XX_FORCE_INLINE void FlagUnset(int32_t index) {
			auto& block = RefBlock(index);
			auto bit = uint64_t(1) << (index & 0b111111);
			assert(((block.flags & bit) ^ bit) == 0);
			block.flags &= ~bit;
		}

		xx::Listi32<Block*> blocks;
		int32_t cap{}, len{}, freeHead{ -1 }, freeCount{}, head{ -1 }, tail{ -1 };
		uint32_t version{};
	};


	template<typename T, template<typename...> typename BlockNode>
	BlockListHolder<T, BlockNode>:: ~BlockListHolder() {
		if (!owner) return;
		owner->Remove(weak);
	}

}






///*
//struct Foo {
//	XX_BLOCK_LIST_TO_WEAK_IMPL_FOR_T(Foo);
//*/
//#define XX_BLOCK_LIST_TO_WEAK_IMPL(T) \
//template<template<typename...> typename BlockNode = xx::BlockListNode>	\
//xx::BlockListWeak<Foo, BlockNode> ToWeak() {	\
//	return xx::BlockListWeak<Foo, BlockNode>::Make(*this);	\
//}
//
//
//namespace xx {
//
//	template<typename BT, template<typename...> typename BlockNode = BlockListNode>
//	struct BlockListsWeak {
//		BT* pointer{};
//		uint32_t offset{};
//		uint32_t version{};
//
//		XX_FORCE_INLINE BlockListNodeBase& RefVNIT() const {
//			return *(BlockListNodeBase*)((char*)pointer - offset);
//		}
//
//		XX_FORCE_INLINE bool Exists() const noexcept {
//			auto& vi = RefVNIT();
//			return pointer && version && version == vi.version;
//		}
//
//		XX_FORCE_INLINE BT& operator()() const {
//			assert(Exists());
//			return (BT&)*pointer;
//		}
//
//		XX_FORCE_INLINE void Reset() {
//			pointer = {};
//			offset = 0;
//			version = 0;
//		}
//
//		template<std::derived_from<BT> T>
//		static BlockListsWeak Make(T const& v) {
//			using Node = BlockNode<T>;
//			static_assert(std::is_base_of_v<BlockListNodeBase, Node>);
//
//			auto o = container_of(&v, Node, value);
//			auto b = &(BT&)v;
//			return { b, uint32_t((char*)b - (char*)o), o->version };
//		}
//
//		template<std::derived_from<BT> T>
//		T& As() const {
//			auto& bt = (*this)();
//			assert(dynamic_cast<T*>(&bt) == static_cast<T*>(&bt));
//			return (T&)bt;
//		}
//	};
//
//	template<typename BT, template<typename...> typename BlockNode = BlockListNode, std::derived_from<BT>...TS>
//	struct BlockLists {
//		using Tup = std::tuple<TS...>;
//		static constexpr std::array<size_t, sizeof...(TS)> ts{ xx::TupleTypeIndex_v<TS, Tup>... };
//		static constexpr std::array<size_t, sizeof...(TS)> is{ TS::cTypeId... };
//		static_assert(ts == is);
//
//		// container
//		xx::SimpleTuple<BlockList<TS, BlockNode>...> bls;
//		static_assert(sizeof(bls.value) * sizeof...(TS) == sizeof(bls));
//
//		// for Weak
//		static constexpr std::array<size_t, sizeof...(TS)> sizes{ sizeof(TS)... };
//		typedef void(*Deleter)(void*);
//		std::array<Deleter, sizeof...(TS)> deleters;
//
//		BlockLists() {
//			ForEachType<Tup>([&]<typename T>() {
//				deleters[xx::TupleTypeIndex_v<T, Tup>] = [](void* o) { ((T*)o)->~T(); };
//			});
//		}
//		BlockLists(BlockLists const&) = delete;
//		BlockLists& operator=(BlockLists const&) = delete;
//
//		template<typename T>
//		BlockList<T, BlockNode>& Get() {
//			return xx::Get<BlockList<T, BlockNode>>(bls);
//		}
//
//		template<typename T>
//		BlockList<T, BlockNode> const& Get() const {
//			return xx::Get<BlockList<T, BlockNode>>(bls);
//		}
//
//		template<typename T>
//		void Remove(T const& v) {
//			Get<T>().Remove(v);
//		}
//
//		BlockList<BT, BlockNode>& Get(int32_t index) {
//			return ((BlockList<BT, BlockNode>*)&bls.value)[index];
//		}
//
//		void Remove(BlockListsWeak<BT, BlockNode> const& w) {
//			if (!w.pointer || !w.version) return;
//			auto& vnit = w.RefVNIT();
//			if (w.version != vnit.version) return;
//			Get(vnit.typeId).Remove(vnit.index, sizes[vnit.typeId], deleters[vnit.typeId]);
//		}
//
//	};
//
//}
