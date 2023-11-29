#include <pch.h>
#include <all.h>

// flag
enum class BuffTags : uint32_t {
	Tag1 = 1, 
	Tag2 = 2,
	Tag3 = 4,
	Tag4 = 8,
	// ... 2^n
};

struct BuffInfo {
	int32_t id{};									// unique key
	int32_t typeId{};								// group
	int32_t priority{};								// sort buffs order by
	BuffTags tags{};								// flag
	float durationSeconds{};						// total life cycle
	float intervalSeconds{};						// update tick time

	std::string name, icon, desc;
};
using BuffInfos = xx::List<xx::Ref<BuffInfo>, int32_t>;

BuffInfos gBuffInfos;								// global config

struct BuffBase {
	xx::Ref<BuffInfo> info;
	xx::Task<> mainLogic;
};

// todo: remove this
struct BuffPriority {
	BuffBase* buff;
	int32_t priority;

	inline static xx::List<BuffPriority, int32_t> tmpContainer;
	inline static bool OrderBy(BuffPriority const& a, BuffPriority const& b) {
		return a.priority < b.priority;
	}
};

struct Buffs {
	xx::List<xx::Shared<BuffBase>, int32_t> children;							// buffs store
	xx::Tasks tasks;

	void Sort() {
		// todo
	}

	void Update() {

		// execute current buffs
		if (children.len) {

			// todo: if have not new buff, skip sort

			// sort
			auto& c = BuffPriority::tmpContainer;
			for (auto& buff : children) {
				c.Emplace(buff.pointer, buff->info->priority);
			}
			std::sort(c.buf, c.buf + c.len, BuffPriority::OrderBy);

			// execute
			for (int32_t i = c.len - 1; i >= 0; --i) {
				if (c[i].buff->mainLogic.Resume()) {							// resume main logic. can't make new children here( use tasks delay create )
					c.SwapRemoveAt(i);
				} else {
					++((xx::Shared<BuffBase>&)c[i]).GetHeader()->sharedCount;	// protect
				}
			}

			// store
			if (children.len > c.len) {
				children.Clear();
				for (int32_t i = 0, e = c.len; i < e; ++i) {
					memcpy(&children[i], &c[i].buff, sizeof(BuffBase*));		// restore protected buffs
				}
				children.len = c.len;
			} else {
				for (int32_t i = 0, e = c.len; i < e; ++i) {
					--((xx::Shared<BuffBase>&)c[i]).GetHeader()->sharedCount;	// unprotect
				}
			}

			// clear
			c.len = 0;
		}

	}
};

struct Buff1 : BuffBase {
	inline static xx::Shared<Buff1> CreateTo(Buffs& bs) {
		// todo: scan exists. update? replace? create fail?

		return {};	// fail
	}
	// todo
};

struct Buff2 : BuffBase {
	inline static xx::Shared<Buff2> CreateTo(Buffs& bs) {
		return {};	// fail
	}
	// todo
};

struct Buff3 : BuffBase {
	inline static xx::Shared<Buff3> CreateTo(Buffs& bs) {
		return {};	// fail
	}
	// todo
};

struct Foo {
	Buffs bm;
	void Init() {
		Buff1::CreateTo(bm);
		//auto buf1 = bm.MakeChildren<Buff1>();
		// todo
	}
};

void SceneMainMenu::Init() {
	gBuffInfos.Clear();
	gBuffInfos.Emplace();	// placeholder
	int32_t id{};
	gBuffInfos.Add(
		xx::MakeShared<BuffInfo>(BuffInfo{ .id = ++id, .typeId = 1, .priority = 1, .name = "buff1_1" }), 
		xx::MakeShared<BuffInfo>(BuffInfo{ .id = ++id, .typeId = 1, .priority = 2, .name = "buff1_2" }),
		xx::MakeShared<BuffInfo>(BuffInfo{ .id = ++id, .typeId = 2, .priority = 3, .name = "buff2" })
	);

	Foo foo;
	foo.Init();
	// todo
}

void SceneMainMenu::Draw() {
};



//xx::Weak<Buffs> owner;
//int32_t indexAtOwner{};

	//template<std::derived_from<BuffBase> T>
	//xx::Shared<T> MakeChildren() {
	//	// todo
	//}
	//// todo

