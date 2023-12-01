#include <pch.h>
#include <all.h>

enum class BuffTags : uint32_t {						// bit flag
	Tag1 = 1, 
	Tag2 = 2,
	Tag3 = 4,
	// ... 2^n
};

struct BuffInfo {
	int32_t id{};										// unique key
	int32_t typeId{};									// group
	int32_t priority{};									// sort buffs order by
	BuffTags tags{};									// flag
	float durationSeconds{};							// total life cycle
	float intervalSeconds{};							// update tick time
	std::string name, icon, desc;						// misc
};
xx::Listi32<xx::Ref<BuffInfo>> gBuffInfos;				// global config

enum class BuffRemoveReasons : uint32_t {
	None, Clean, Replace, Conflict						// todo: more
};

struct Buffs;
struct BuffBase {
	typedef void(*OnRemove_t)(BuffBase* self, BuffBase* sender, BuffRemoveReasons reason);
	xx::Ref<BuffInfo> info;								// todo: local cache for optimize performance?
	OnRemove_t onRemove{};
	xx::Task<> mainLogic;
};

struct Buffs {
	xx::Listi32<xx::Shared<BuffBase>> data;
	bool dirty{ true };
	xx::Tasks tasks;									// for generate Buff

	void Update() {
		tasks();
		if (dirty) {
			dirty = false;
			std::sort((BuffBase**)data.buf, (BuffBase**)data.buf + data.len
				, [](auto& a, auto& b) { return a->info->priority < b->info->priority; });
		}
		for (auto i = data.len - 1; i >= 0; --i) {
			if (data[i]->mainLogic.Resume()) {
				data.SwapRemoveAt(i);
				dirty = true;
			}
		}
	}

	int32_t FindByTypeId(int32_t typeId, int32_t beginIndex = 0) {
		for (int32_t i = beginIndex, e = data.len; i < e; ++i) {
			if (data[i]->info->typeId == typeId) return i;
		}
		return -1;
	}

	void RemoveAll(BuffBase* sender, BuffRemoveReasons reason) {
		for (auto& b : data) {
			if (b->onRemove) {
				b->onRemove(b.pointer, sender, reason);
			}
		}
		data.Clear();
	}

	void Remove(int32_t index, BuffBase* sender, BuffRemoveReasons reason) {
		data[index]->onRemove(data[index].pointer, sender, reason);
		data.SwapRemoveAt(index);
	}

	template<std::derived_from<BuffBase> T = BuffBase>
	xx::Shared<T> const& At(int32_t index) {
		return (xx::Shared<T>&)data[index];
	}

	// todo: more helpers
};

struct Buff1 : BuffBase {
	float expirationLifeTime{}, expirationTickTime{};

	inline static xx::Shared<Buff1> CreateTo(Buffs& bs) {
		if (auto idx = bs.FindByTypeId(0); idx != -1) {
			auto&& b = bs.At<Buff1>(idx);
			b->expirationLifeTime = (float)gEngine->nowSecs + b->info->durationSeconds;
		} else {
			auto b = xx::MakeShared<Buff1>();
			b->info = gBuffInfos[0];
		}

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

struct Foo {
	Buffs bm;
	void Init() {
		//Buff1::CreateTo(bm);
		//auto buf1 = bm.MakeChildren<Buff1>();
		// todo
	}
};

void SceneMainMenu::Init() {
	gBuffInfos.Clear();
	gBuffInfos.Emplace();	// placeholder
	gBuffInfos.Add(
		xx::MakeShared<BuffInfo>(BuffInfo{ .id = 0, .typeId = 1, .priority = 1, .name = "buff1" }), 
		xx::MakeShared<BuffInfo>(BuffInfo{ .id = 1, .typeId = 2, .priority = 2, .name = "buff2" })
	);

	Foo foo;
	foo.Init();
	// todo

	{
		std::u32string s;
		auto secs = xx::NowEpochSeconds();
		for (int i = 0; i < 10000000; i++) {
			xx::IntToStringTo<true, 10>(s, i);
		}
		xx::CoutN(xx::NowEpochSeconds(secs), " u32string xx::IntToStringTo. s = ", s);
	}
	{
		std::u32string s;
		auto secs = xx::NowEpochSeconds();
		for (int i = 0; i < 10000000; i++) {
			s.clear();
			auto tmp = std::to_string(i);
			if (tmp.size() < 10) {
				s.append(10 - tmp.size(), '0');
			}
			s.append(xx::StringU8ToU32(tmp));
		}
		xx::CoutN(xx::NowEpochSeconds(secs), " u32string std::to_string. s = ", s);
	}
	{
		std::string s;
		auto secs = xx::NowEpochSeconds();
		for (int i = 0; i < 10000000; i++) {
			xx::IntToStringTo<true, 10>(s, i);
		}
		xx::CoutN(xx::NowEpochSeconds(secs), " string xx::IntToStringTo. s = ", s);
	}
	{
		std::string s;
		auto secs = xx::NowEpochSeconds();
		for (int i = 0; i < 10000000; i++) {
			s.clear();
			auto tmp = std::to_string(i);
			if (tmp.size() < 10) {
				s.append(10 - tmp.size(), '0');
			}
			s.append(tmp);
		}
		xx::CoutN(xx::NowEpochSeconds(secs), " string std::to_string. s = ", s);
	}
	{
		std::string s;
		auto secs = xx::NowEpochSeconds();
		for (int i = 0; i < 10000000; i++) {
			s.clear();
			char buf[32];
			auto len = sprintf_s(buf, "%d", i);
			if (len < 10) {
				s.append(10 - len, '0');
			}
			s.append(std::string_view(buf, len));
		}
		xx::CoutN(xx::NowEpochSeconds(secs), " string sprintf. s = ", s);
	}
	{
		std::string s;
		auto secs = xx::NowEpochSeconds();
		for (int i = 0; i < 10000000; i++) {
			s.clear();
			char buf[32];
			itoa(i, buf, 10);
			auto len = strlen(buf);
			if (len < 10) {
				s.append(10 - len, '0');
			}
			s.append(std::string_view(buf, len));
		}
		xx::CoutN(xx::NowEpochSeconds(secs), " string itoa. s = ", s);
	}
}

void SceneMainMenu::Draw() {
};


//xx::Weak<Buffs> owner;
//int32_t indexAtOwner{ -1 };							// fill by update, after sort for easy remove
			//for (int32_t i = 0, e = data.len; i < e; ++i) {
			//	data[i]->indexAtOwner = i;
			//}

//xx::Weak<Buffs> owner;
//int32_t indexAtOwner{};

	//template<std::derived_from<BuffBase> T>
	//xx::Shared<T> MakeChildren() {
	//	// todo
	//}
	//// todo

//// todo: remove this
//struct BuffPriority {
//	BuffBase* buff;
//	int32_t priority;
//
//	inline static xx::Listi32<BuffPriority> tmpContainer;
//	inline static bool OrderBy(BuffPriority const& a, BuffPriority const& b) {
//		return a.priority < b.priority;
//	}
//};

	//void Update() {

	//	// execute current buffs
	//	if (data.len) {

	//		// todo: if have not new buff, skip sort

	//		// sort
	//		auto& c = BuffPriority::tmpContainer;
	//		for (auto& buff : data) {
	//			c.Emplace(buff.pointer, buff->info->priority);
	//		}
	//		std::sort(c.buf, c.buf + c.len, BuffPriority::OrderBy);

	//		// execute
	//		for (int32_t i = c.len - 1; i >= 0; --i) {
	//			if (c[i].buff->mainLogic.Resume()) {							// resume main logic. can't make new data here( use tasks delay create )
	//				c.SwapRemoveAt(i);
	//			} else {
	//				++((xx::Shared<BuffBase>&)c[i]).GetHeader()->sharedCount;	// protect
	//			}
	//		}

	//		// store
	//		if (data.len > c.len) {
	//			data.Clear();
	//			for (int32_t i = 0, e = c.len; i < e; ++i) {
	//				memcpy(&data[i], &c[i].buff, sizeof(BuffBase*));		// restore protected buffs
	//			}
	//			data.len = c.len;
	//		} else {
	//			for (int32_t i = 0, e = c.len; i < e; ++i) {
	//				--((xx::Shared<BuffBase>&)c[i]).GetHeader()->sharedCount;	// unprotect
	//			}
	//		}

	//		// clear
	//		c.len = 0;
	//	}

	//}