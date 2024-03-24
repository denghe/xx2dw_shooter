﻿#pragma once
#include "xx_list.h"

namespace xx {
	template<typename T>
	concept Has_cTypeId = requires { typename T::cTypeId; };

	struct VersionNextIndexTypeId {
		uint32_t version;
		int32_t next, index, typeId;
	};

	template<typename T>
	struct BlockListNodeBase : VersionNextIndexTypeId {
		T value;
	};

	template<typename T, template<typename...> typename BlockNode = BlockListNodeBase>
	struct BlockList;


	template<typename T, template<typename...> typename BlockNode = BlockListNodeBase>
	struct BlockListWeak {
		using Node = BlockNode<T>;
		static_assert(std::is_base_of_v<VersionNextIndexTypeId, Node>);

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


	template<typename T, template<typename...> typename BlockNode = BlockListNodeBase>
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
		static_assert(std::is_base_of_v<VersionNextIndexTypeId, Node>);

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
			std::swap(version, o.version);
			return *this;
		}
		~BlockList() {
			Foreach([](T& v) {
				v.~T();
				});
			for (auto& o : blocks) {
				::free(o);
			}
		}

		// .Foreach([](T& o)->xx::ForeachResult {    });
		template<bool callByClear = false, typename F>
		void Foreach(F&& func) {
			using R = xx::FuncR_t<F>;
			static_assert(std::is_same_v<R, ForeachResult> || std::is_same_v<R, bool> || std::is_void_v<R>);
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

					if constexpr (std::is_void_v<R>) {
						func(o.value);
						if constexpr (callByClear) {
							o.version = 0;
							o.next = -1;
							o.index = -1;
							o.typeId = -1;
						}
					} else {
						auto r = func(o.value);
						if constexpr (std::is_same_v<R, bool>) {
							if (r) return;
						} else {
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
				}
				if constexpr (callByClear) {
					static_assert(std::is_void_v<R>);
					flags = 0;
				}
			}
		}

		// todo: shrink

		template<bool resetVersion = false>
		void Clear() {
			if (!Count()) return;
			Foreach<true>([](T& v) {
				v.~T();
			});
			len = 0;
			freeHead = -1;
			freeCount = 0;
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

		template<typename...Args>
		T& Emplace(Args&&... args) {
			auto index = Alloc();
			auto& o = RefNode(index);
			o.version = GenVersion();
			o.next = -1;						// todo: fill ? order by create time ?
			o.index = index;
			if constexpr (Has_cTypeId<T>) {
				o.typeId = T::cTypeId;
			} else {
				o.typeId = -1;
			}
			return *new (&o.value) T(std::forward<Args>(args)...);
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

		void Remove(int32_t index) {
			auto& o = RefNode(index);
			assert(o.version);
			o.value.~T();
			o.version = 0;
			o.next = freeHead;
			o.index = -1;
			o.typeId = -1;
			freeHead = index;
			++freeCount;
			FlagUnset(index);
		}

		// for unknown type ( wrong type: T )
		void Remove(int32_t index, size_t tSiz, void(*deleter)(void*)) {
			auto& o = *(Node*)((char*)&RefBlock(index).buf + tSiz * index);
			deleter(&o.value);
			o.version = 0;
			o.next = freeHead;
			o.index = -1;
			o.typeId = -1;
			freeHead = index;
			++freeCount;
			FlagUnset(index);
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

		XX_FORCE_INLINE Node& RefNode(int32_t index) {
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
		int32_t cap{}, len{}, freeHead{ -1 }, freeCount{};
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
//template<template<typename...> typename BlockNode = xx::BlockListNodeBase>	\
//xx::BlockListWeak<Foo, BlockNode> ToWeak() {	\
//	return xx::BlockListWeak<Foo, BlockNode>::Make(*this);	\
//}
//
//
//namespace xx {
//
//	template<typename BT, template<typename...> typename BlockNode = BlockListNodeBase>
//	struct BlockListsWeak {
//		BT* pointer{};
//		uint32_t offset{};
//		uint32_t version{};
//
//		XX_FORCE_INLINE VersionNextIndexTypeId& RefVNIT() const {
//			return *(VersionNextIndexTypeId*)((char*)pointer - offset);
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
//			static_assert(std::is_base_of_v<VersionNextIndexTypeId, Node>);
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
//	template<typename BT, template<typename...> typename BlockNode = BlockListNodeBase, std::derived_from<BT>...TS>
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
