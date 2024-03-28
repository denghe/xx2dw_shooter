#include "pch.h"
#include "looper.h"
#include "scene_main_menu.h"
#include "scene_test1.h"
#include "xx_spaceab.h"

struct Foo {
	xx::FromTo<XY> aabb;

	void Init(XY const& pos, XY const& size) {
		auto s2 = size / 2;
		aabb.from = pos - s2;
		aabb.to = pos + s2;
	}

	void MoveBy(XY const& offset) {
		aabb.from += offset;
		aabb.to += offset;
	}
};

template<typename T, typename SizeType = int32_t>
struct FixedArray {
	static_assert(sizeof(SizeType) >= 4);

	struct Core {
		SizeType len, cap;
		T buf[0];
	} *core{};

	void Construct(int32_t cap) {
		assert(!core);
		core = (Core*)malloc(sizeof(SizeType) * 2 + sizoef(T) * cap);
		core->len = 0;
		core->cap = cap;
	}

	void Destroy() {
		if (!core) return;
		for (SizeType i = 0; i < len; ++i) {
			std::destroy_at(&core->buf[i]);
		}
		free(core);
		core = {};
	}

	struct Iter {
		T* ptr;
		bool operator!=(Iter const& other) noexcept { return ptr != other.ptr; }
		Iter& operator++() noexcept { ++ptr; return *this; }
		T& operator*() noexcept { return *ptr; }
	};
	Iter begin() noexcept { return Iter{ core->buf }; }
	Iter end() noexcept { return Iter{ core->buf + core->len }; }
	Iter begin() const noexcept { return Iter{ core->buf }; }
	Iter end() const noexcept { return Iter{ core->buf + core->len }; }

	T const& operator[](SizeType idx) const noexcept {
		assert(core && idx >= 0 && idx < len);
		return core->buf[idx];
	}

	T& operator[](SizeType idx) noexcept {
		assert(core && idx >= 0 && idx < core->len);
		return core->buf[idx];
	}

	template<typename...Args>
	T& Emplace(Args&&...args) {
		assert(core && core->len < core->cap);
		return std::construct_at(&core->buf[len++], std::forward<Args>(args)...);
	}

	template<typename ...TS>
	void Add(TS&&...vs) {
		(Emplace(std::forward<TS>(vs)), ...);
	}

	void RemoveAt(SizeType idx) {
		assert(core);
		auto& len = core->len;
		assert(core && idx >= 0 && idx < core->len);
		--core->len;
		if constexpr (IsPod_v<T>) {
			std::destroy_at(&core->buf[idx]);
			::memmove(core->buf + idx, core->buf + idx + 1, (core->len - idx) * sizeof(T));
		} else {
			for (SizeType i = idx; i < len; ++i) {
				core->buf[i] = (T&&)core->buf[i + 1];
			}
			std::destroy_at(&core->buf[len]);
		}
	}

	void SwapRemoveAt(SizeType idx) {
		assert(idx >= 0 && idx < len);
		buf[idx].~T();
		--len;
		if (len != idx) {
			if constexpr (IsPod_v<T>) {
				::memcpy(&buf[idx], &buf[len], sizeof(T));
			} else {
				new (&buf[idx]) T((T&&)buf[len]);
			}
		}
	}

	void Clear() {
		if (!cap) return;
		if (len) {
			for (SizeType i = 0; i < len; ++i) {
				buf[i].~T();
			}
			len = 0;
		}
	}
};

void SceneMainMenu::Init() {

	xx::SpaceABGrid<Foo> sg;
	sg.Init(10, 10, 10);

	XY pos{ 5,5 }, siz{ 10, 10 };
	auto& f = sg.EmplaceInit(pos, siz);
	auto& o = xx::SpaceABNode<Foo>::From(f);
	xx::CoutN("from ", o.crIdx.from, " to ", o.crIdx.to);

	xx::SpaceABWeak<Foo> w(f);
	xx::CoutN("w exists? ", !!w);

	f.MoveBy(siz);
	sg.Update(f);
	xx::CoutN("from ", o.crIdx.from, " to ", o.crIdx.to);

	sg.Remove(f);
	xx::CoutN("w exists? ", !!w);





	//rootNode.Emplace()->Init();

	//auto basePos = rootNode->size / 2;

	//auto txt = rootNode->MakeChildren<Label>();
	//txt->Init(2, basePos + XY{ 0, 200 }, { 3, 3 }, { 0.5f, 0.5f }, RGBA8_White, U"astar tests");

	//auto bg = rootNode->MakeChildren<Scale9Sprite>();
	//bg->Init(1, basePos + XY{ 0, 200 }, { 5, 5 }, XY{ 0.5f, 0.5f }, txt->CalcBorderSize({ 50, 20 }) / 5, gLooper.s9cfg);

	//rootNode->MakeChildren<Button>()->Init(3, basePos + XY{ 0, 0 }, { 0.5f, 0.5f }, gLooper.s9cfg, U"test1", [&]() {
	//	gLooper.DelaySwitchTo<Test1::Scene>();
	//}, 3);
}

void SceneMainMenu::Draw() {
	//gLooper.DrawNode(rootNode);
};
