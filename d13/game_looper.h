#pragma once
#include <engine.h>

constexpr GDesign<1280, 720, 60> gDesign;

struct ItemBase {
	XY pos;
	float radius;
	virtual void Draw() = 0;
	virtual void FillRichLabel() = 0;
};

struct A : ItemBase {
	void Init();
	virtual void FillRichLabel() override;
	virtual void Draw() override;
};

struct B : ItemBase {
	void Init();
	virtual void FillRichLabel() override;
	virtual void Draw() override;
};

struct C : ItemBase {
	void Init();
	virtual void FillRichLabel() override;
	virtual void Draw() override;
};

struct GameLooper : Engine<GameLooper>, decltype(gDesign) {
	void Update();
	xx::Task<> MainTask();
	void Draw();

	bool ready{};
	xx::Shared<Node> node;

	xx::Listi32<xx::Shared<ItemBase>> items;

	xx::Listi32<xx::Ref<Frame>> frames_potion;
	xx::Listi32<xx::Ref<Frame>> frames_gem;
	xx::Listi32<xx::Ref<Frame>> frames_mine;
	xx::Ref<Frame> frame_button;

	xx::Weak<ItemBase> mouseOverItem;
};
extern GameLooper gLooper;
