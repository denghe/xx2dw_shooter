#pragma once
#include "xx_list.h"

namespace xx {
	template<typename U, typename T, size_t offset>
	struct ECSIndex;

	template<typename U, typename T, size_t offset>
	struct ECSContainer {
		using Index = ECSIndex<U, T, offset>;

		// data + pointer
		struct Node : T {
			U* ptr{};
			Index& GetIndex() const {
				assert(ptr);
				return *(Index*)((char*)ptr + offset);
			}
		};

		// data + pointer array( can reserve, sort )
		xx::Listi<Node> nodes;

		// add 
		void Attach(U* o, Index* ecsi);

		// remove
		void Detach(ptrdiff_t idx);

		// refill all index ( after sort ? )
		void UpdateIndexs();
	};

	template<typename U, typename T, size_t offset>
	struct ECSIndex {
		using Container = ECSContainer<U, T, offset>;

		// ref to container & data index
		Container* ptr{};
		ptrdiff_t idx{ -1 };

		// ref to data
		T& Get() const {
			return ptr->nodes[idx];
		}

		// attach
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
	void ECSContainer<U, T, offset>::Attach(U* o, Index* ecsi) {
#ifndef NDEBUG
		auto p = (char*)ecsi - offset;
		assert(p == (char*)o);			// wrong offset value ?
#endif
		assert(o);
		assert(ecsi->ptr == nullptr);
		assert(ecsi->idx == -1);
		ecsi->ptr = this;
		ecsi->idx = nodes.len;
		auto& n = nodes.Emplace();
		n.Node::ptr = o;
	}

	template<typename U, typename T, size_t offset>
	void ECSContainer<U, T, offset>::Detach(ptrdiff_t idx) {
		nodes.SwapRemoveAt(idx);
		if (idx < nodes.len) {
			nodes[idx].GetIndex().idx = idx;
		}
	}

	template<typename U, typename T, size_t offset>
	void ECSContainer<U, T, offset>::UpdateIndexs() {
		for (ptrdiff_t i = 0, e = nodes.len; i < e; i++) {
			nodes[i].GetIndex().idx = i;
		}
	}
}

/*
// some example

struct A {
	float x, y;
}

struct B {
	std::string name, nick, desc;
}

struct Base {
	// no virtual here, ensure offset begin == 0. offset == *0, *2, *4, *6, ...
	
	xx::ECSIndex<Base, A, sizeof(size_t) * 0> a;
	xx::ECSIndex<Base, B, sizeof(size_t) * 2> b;
	// ...

	template<typename...ES>
	void Init(ES&...es) {
		a.Init( xx::GetAt<0>(es...), this );
		b.Init( xx::GetAt<1>(es...), this );
		// ...
	}
}

struct Foo : Base {
	// other member here... can virtual
	virtual ~Foo() {}
	// ...
}

inline void Test() {
	decltype(Base::a)::Container ecsA;
	decltype(Base::b)::Container ecsB;
	// ...

	xx::Listi<xx::Shared<Foo>> foos; 

	foos.Emplace().Emplace()->Init( ecsA, ecsB );
	// ...

	auto& ns = ecsA.nodes;
	std::sort(ns.buf, ns.buf + ns.len, [](auto& a, auto& b) { return a.y < b.y; });
	ecsA.UpdateIndexs();

	for (auto& node : ecsA.nodes) {
		// ...
	}

}

*/
