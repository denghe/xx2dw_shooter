#pragma once
#include <game_looper.h>

//enum class ItemTypes {
//	A, B	// todo
//};

struct ItemBase;
struct ItemContainer {
	xx::List<xx::Shared<ItemBase>, int32_t> items;
	void Add(xx::Shared<ItemBase> item);
	void Remove(ItemBase* item);
	// todo: swap ...
};

struct ItemBase {
	ItemContainer* container{};
	int32_t indexAtContainer{};
	int32_t typeId{};
};

inline void ItemContainer::Add(xx::Shared<ItemBase> item) {
	assert(!item->container);
	item->container = this;
	item->indexAtContainer = items.len;
	items.Emplace(std::move(item));
}

inline void ItemContainer::Remove(ItemBase* item) {
	assert(item->container == this);
	auto idx = item->indexAtContainer;
	assert(items.len > idx);
	assert(items[idx] == item);
	items.RemoveAt(idx);
	if (auto e = items.len - 1; idx < e) {
		for (int32_t i = idx + 1, e = items.len; i < e; ++i) {
			assert(items[i]->indexAtContainer == i);
			items[i]->indexAtContainer = i - 1;
		}
		memmove(items.buf + idx, items.buf + idx + 1, sizeof(xx::Shared<ItemBase>) * (e - idx));
	}
}

struct Player {
	ItemContainer equips, bag;	// todo: more
};

struct SceneMainMenu : Scene {
	xx::Shared<Node> rootNode;

	xx::Shared<Player> player1;

	virtual void Init() override;
	virtual void Draw() override;
};
