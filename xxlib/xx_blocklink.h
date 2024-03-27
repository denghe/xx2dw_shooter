#pragma once
#include "xx_list.h"

namespace xx {

	template <class T> concept HasMember_version = requires(T) { T::version; };		// todo: more detail check?
	template <class T> concept HasMember_index = requires(T) { T::index; };

	template <class T> concept HasMember_typeId = requires(T) { T::typeId; };
	template <class T> concept HasMemberType_cTypeId = requires(T) { T::cTypeId; };

	struct BlockLinkVI {
		int32_t version{ -2 }, index{ -1 };
		bool operator==(BlockLinkVI const& o) const { return memcmp(this, &o, sizeof(o)) == 0; }
	};


	// base block node
	template<typename T>
	struct BlockLinkVIT : BlockLinkVI {
		T value;
	};

	struct BlockLinkBase_VI {
		xx::Listi32<void*> blocks;
		int32_t cap{}, len{}, freeHead{ -1 }, freeCount{}, version{ -2 };

		void Swap(BlockLinkBase_VI& o) {
			std::swap(this->blocks, o.blocks);
			std::swap(this->cap, o.cap);
			std::swap(this->len, o.len);
			std::swap(this->freeHead, o.freeHead);
			std::swap(this->freeCount, o.freeCount);
			std::swap(this->version, o.version);
		}
	};


	template <class T> concept HasMember_prev = requires(T) { T::prev; };
	template <class T> concept HasMember_next = requires(T) { T::next; };

	// double link block node
	template<typename T>
	struct BlockLinkVINPT : BlockLinkVI {
		int32_t next, prev;
		T value;
	};

	struct BlockLinkBase_VINP : BlockLinkBase_VI {
		int32_t head{ -1 }, tail{ -1 };

		void Swap(BlockLinkBase_VINP& o) {
			BlockLinkBase_VI::Swap(o);
			std::swap(this->head, o.head);
			std::swap(this->tail, o.tail);
		}
	};



	template<typename T, template<typename...> typename Node>
	struct BlockLinkWeak {
		//static_assert(HasMember_version<Node<T>>);

		T* pointer{};
		int32_t version{ -2 };

		BlockLinkWeak() = default;
		BlockLinkWeak(BlockLinkWeak const&) = default;
		BlockLinkWeak& operator=(BlockLinkWeak const&) = default;
		BlockLinkWeak(BlockLinkWeak&&) = default;
		BlockLinkWeak& operator=(BlockLinkWeak&&) = default;

		BlockLinkWeak(T const& v) {
			operator=(v);
		}
		BlockLinkWeak& operator=(T const& v) {
			auto& o = *container_of(&v, Node<T>, value);
			pointer = (T*)&v;
			version = o.version;
			return *this;
		}

		XX_FORCE_INLINE Node<T>& RefNode() const {
			return (Node<T>&)*container_of(pointer, Node<T>, value);
		}

		XX_FORCE_INLINE bool Exists() const noexcept {
			return pointer && version < -2 && version == RefNode().version;
		}
		XX_FORCE_INLINE operator bool() const noexcept {
			return Exists();
		}

		XX_FORCE_INLINE T& operator()() const {
			assert(Exists());
			return (T&)*pointer;
		}

		XX_FORCE_INLINE void Reset() {
			pointer = {};
			version = -2;
		}
	};


	template<typename T, template<typename...> typename Node, bool isDoubleLink = HasMember_prev<Node<T>>&& HasMember_next<Node<T>>, bool enableFlags = !isDoubleLink>
	struct BlockLink;

	template<typename T, template<typename...> typename Node>
	struct BlockLinkHolder {
		using OT = BlockLink<T, Node>;
		using WT = BlockLinkWeak<T, Node>;

		OT* owner{};
		WT weak;

		BlockLinkHolder() = default;
		BlockLinkHolder(BlockLinkHolder const&) = default;
		BlockLinkHolder& operator=(BlockLinkHolder const&) = default;
		BlockLinkHolder(BlockLinkHolder&&) = default;
		BlockLinkHolder& operator=(BlockLinkHolder&&) = default;

		~BlockLinkHolder();

		template<typename U>
		BlockLinkHolder(OT& owner_, U&& u) {
			Set(owner_, u);
		}

		XX_FORCE_INLINE BlockLinkHolder& Set(OT& owner_, T const& v) {
			owner = &owner_;
			weak = v;
			return *this;
		}

		XX_FORCE_INLINE BlockLinkHolder& Set(OT& owner_, WT const& w) {
			owner = &owner_;
			weak = w;
			return *this;
		}

		XX_FORCE_INLINE BlockLinkHolder& Set(OT& owner_) {
			owner = &owner_;
			return *this;
		}

		XX_FORCE_INLINE BlockLinkHolder& Set(T const& v) {
			assert(owner);
			weak = v;
			return *this;
		}

		XX_FORCE_INLINE BlockLinkHolder& Set(WT const& w) {
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

	template<typename NT>
	struct BlockLinkBlock {
		std::array<NT, 64> buf;
	};

	template<typename NT>
	struct BlockLinkBlockWithFlags {
		uint64_t flags;
		std::array<NT, 64> buf;
	};


	template<typename T, template<typename...> typename Node = BlockLinkVIT, bool isDoubleLink, bool enableFlags>
	struct BlockLink : std::conditional_t<isDoubleLink, BlockLinkBase_VINP, BlockLinkBase_VI> {
		using NodeType = Node<T>;

		static_assert(HasMember_version<Node<T>>);
		static_assert(HasMember_index<Node<T>>);
		static_assert(!HasMember_typeId<Node<T>> || HasMemberType_cTypeId<T>);

		using Block = std::conditional_t<isDoubleLink, BlockLinkBlock<Node<T>>, BlockLinkBlockWithFlags<Node<T>>>;

		using HolderType = BlockLinkHolder<T, Node>;
		using WeakType = BlockLinkWeak<T, Node>;

		BlockLink() = default;
		BlockLink(BlockLink const&) = delete;
		BlockLink& operator=(BlockLink const&) = delete;

		BlockLink(BlockLink&& o) {
			this->Swap(o);
		}
		BlockLink& operator=(BlockLink&& o) {
			this->Swap(o);
			return *this;
		}

		~BlockLink() {
			Clear<true, true>();
		}

		template<bool freeBuf = false, bool resetVersion = false>
		void Clear() {
			if constexpr (!(std::is_standard_layout_v<T> && std::is_trivial_v<T>)) {
				if constexpr (isDoubleLink) {
					ForeachLink([](auto&) { return ForeachResult::RemoveAndContinue; });
				} else {
					ForeachFlags<true>([](auto&){});
				}
			}
			if constexpr (freeBuf) {
				for (auto& o : this->blocks) {
					::free(o);
				}
				this->blocks.Clear(true);
				this->cap = 0;
			}

			if constexpr (isDoubleLink) {
				this->head = this->tail = -1;
			}

			this->freeHead = -1;
			this->freeCount = this->len = 0;

			if constexpr (resetVersion) {
				this->version = -2;
			}
		}

		// foreach by flags
		// .Foreach([](T& o)->void {    });
		// .Foreach([](T& o)->xx::ForeachResult {    });
		template<bool callByClear = false, typename F, typename R = std::invoke_result_t<F, T&>>
		void ForeachFlags(F&& func) requires enableFlags {
			if (this->len <= 0) return;

			for (int32_t i = 0, n = this->blocks.len - 1; i <= n; ++i) {

				auto& block = *(Block*)this->blocks[i];
				auto& flags = block.flags;
				if (!flags) continue;

				auto left = this->len & 0b111111;
				int32_t e = (i < n || !left) ? 64 : left;
				for (int32_t j = 0; j < e; ++j) {
					auto& o = block.buf[j];
					auto bit = uint64_t(1) << j;
					if ((flags & bit) == 0) {
						assert(o.version >= -2);
						continue;
					}
					assert(o.version < -2);

					if constexpr (callByClear) {
						o.value.~T();
						o.version = -2;		// for weak check
					} else if constexpr (std::is_void_v<R>) {
						func(o.value);
					} else {
						auto r = func(o.value);
						switch (r) {
						case ForeachResult::Continue: break;
						case ForeachResult::RemoveAndContinue:
							Free(o);
							break;
						case ForeachResult::Break: return;
						case ForeachResult::RemoveAndBreak:
							Free(o);
							return;
						}
					}
				}
				if constexpr (callByClear) {
					flags = 0;
				}
			}
		}

		// foreach by head / tail link
		// .Foreach([](T& o)->void {    });
		// .Foreach([](T& o)->xx::ForeachResult {    });
		template<bool fromHead = true, typename F, typename R = std::invoke_result_t<F, T&>>
		void ForeachLink(F&& func, int32_t beginIdx = -1) requires isDoubleLink {
			if (beginIdx == -1) {
				if constexpr (fromHead) {
					beginIdx = this->head;
				} else {
					beginIdx = this->tail;
				}
			}
			for (int32_t idx = beginIdx; idx != -1;) {
				auto& o = RefNode(idx);
				if constexpr (std::is_void_v<R>) {
					func(o.value);
				} else {
					auto r = func(o.value);
					switch (r) {
					case ForeachResult::Continue: goto LabContinue;
					case ForeachResult::RemoveAndContinue:
						Free(o);
						goto LabContinue;
					case ForeachResult::Break: return;
					case ForeachResult::RemoveAndBreak:
						Free(o);
						return;
					}
				}
			LabContinue:
				if constexpr (fromHead) {
					idx = o.next;
				} else {
					idx = o.prev;
				}
			}
		}

		XX_FORCE_INLINE void Reserve(int32_t newCap) {
			if (newCap < this->cap) return;
			auto n = ((uint32_t&)newCap + 63u) >> 6;
			for (uint32_t i = 0; i < n; ++i) {
				Reserve();
			}
		}

		XX_FORCE_INLINE int32_t Count() const {
			return this->len - this->freeCount;
		}

		XX_FORCE_INLINE Node<T>* TryGet(BlockLinkVI const& vi) const {
			if (vi.version >= -2 || vi.index < 0 || vi.index >= this->len) return nullptr;
			auto& o = RefNode(vi.index);
			return o.version == vi.version ? (Node<T>*)&o : nullptr;
		}

		void Remove(T const& v) {
			auto o = container_of(&v, Node<T>, value);
			Free(*o);
		}

		bool Remove(BlockLinkVI const& vi) {
			if (vi.version >= -2 || vi.index < 0 || vi.index >= this->len) return false;
			auto& o = RefNode(vi.index);
			if (o.version != vi.version) return false;
			Free(o);
			return true;
		}

		void Remove(WeakType const& wt) {
			if (wt.pointer && wt.version < -2) {
				auto o = container_of(wt.pointer, Node<T>, value);
				if (wt.version == o->version) {
					Free(*o);
				}
			}
		}

		template<bool appendToTail = true, typename...Args>
		Node<T>& EmplaceNode(Args&&... args) {
			return EmplaceCore<appendToTail>(std::forward<Args>(args)...);
		}

		template<bool appendToTail = true, typename...Args>
		T& Emplace(Args&&... args) {
			return EmplaceCore<appendToTail>(std::forward<Args>(args)...).value;
		}

		template<bool appendToTail = true, typename...Args>
		WeakType WeakEmplace(Args&&... args) {
			return { EmplaceCore<appendToTail>(std::forward<Args>(args)...).value };
		}

		template<bool appendToTail = true, typename...Args>
		HolderType HolderEmplace(Args&&... args) {
			return { *this, EmplaceCore<appendToTail>(std::forward<Args>(args)...).value };
		}

	protected:

		XX_FORCE_INLINE int32_t GenVersion() {
			if (--this->version >= -1) {
				this->version = -3;				// -2 is "default / empty"
			}
			return this->version;
		}

		XX_FORCE_INLINE Block& RefBlock(int32_t index) const {
			assert(index >= 0 && index < this->cap);
			return *(Block*)this->blocks[(uint32_t&)index >> 6];
		}

		XX_FORCE_INLINE Node<T>& RefNode(int32_t index) const {
			auto& block = RefBlock(index);
			auto& node = block.buf[index & 0b111111];
			assert(node.index == index);
			return node;
		}

		template<bool value = true>
		XX_FORCE_INLINE void FlagSet(int32_t index) requires enableFlags {
			auto& block = RefBlock(index);
			auto bit = uint64_t(1) << (index & 0b111111);
			if constexpr (value) {
				assert((block.flags & bit) == 0);
				block.flags |= bit;
			} else {
				assert(((block.flags & bit) ^ bit) == 0);
				block.flags &= ~bit;
			}
		}

		XX_FORCE_INLINE void Reserve() {
			auto len = this->blocks.len;
			this->cap += 64;
			auto b = (Block*)this->blocks.Emplace(malloc(sizeof(Block)));
			if constexpr (!isDoubleLink) {
				b->flags = 0;
			}
			for (int32_t i = 0; i < 64; ++i) {
				b->buf[i].index = 64 * len + i;
				if constexpr (HasMember_typeId<Node<T>>) {
					b->buf[i].typeId = T::cTypeId;
				}
			}
		}

		template<bool useDeleter = false>
		XX_FORCE_INLINE void Free(Node<T>& o, void(*deleter)(void*) = {}) {
			auto index = o.index;
			if constexpr (useDeleter) {
				deleter(&o.value);
			} else {
				o.value.~T();
			}
			if constexpr (isDoubleLink) {
				auto prev = o.prev;
				auto next = o.next;
				if (index == this->head) {
					this->head = next;
				}
				if (index == this->tail) {
					this->tail = prev;
				}
				if (prev >= 0) {
					RefNode(prev).next = next;
				}
				if (next >= 0) {
					RefNode(next).prev = prev;
				}
			}
			assert(o.version < -2);
			o.version = this->freeHead;
			this->freeHead = index;
			++this->freeCount;
			if constexpr (!isDoubleLink) {
				FlagSet<false>(index);
			}
		}

		template<bool appendToTail = true, typename...Args>
		XX_FORCE_INLINE Node<T>& EmplaceCore(Args&&... args) {
			int32_t index;
			if (this->freeCount > 0) {
				index = this->freeHead;
				this->freeHead = RefNode(index).version;
				this->freeCount--;
			} else {
				if (this->len == this->cap) {
					Reserve();
				}
				index = this->len;
				this->len++;
			}
			if constexpr (!isDoubleLink) {
				FlagSet(index);
			}

			auto& o = RefNode(index);
			o.version = GenVersion();

			if constexpr (isDoubleLink) {
				if constexpr (appendToTail) {
					o.prev = this->tail;
					o.next = -1;

					if (this->tail >= 0) {
						RefNode(this->tail).next = index;
						this->tail = index;
					} else {
						assert(this->head == -1);
						this->head = this->tail = index;
					}
				} else {
					o.prev = -1;
					o.next = this->head;

					if (this->head >= 0) {
						RefNode(this->head).prev = index;
						this->head = index;
					} else {
						assert(this->head == -1);
						this->head = this->tail = index;
					}
				}
			}

			new (&o.value) T(std::forward<Args>(args)...);
			return o;
		}

	};


	template<typename T, template<typename...> typename BlockNode>
	BlockLinkHolder<T, BlockNode>:: ~BlockLinkHolder() {
		if (!owner) return;
		owner->Remove(weak);
	}
}
