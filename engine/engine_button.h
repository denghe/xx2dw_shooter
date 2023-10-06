#pragma once
#include "engine_node_derived.h"
#include "engine_label.h"
#include "engine_scale9sprite.h"

struct Button : MouseEventHandlerNode {
	constexpr static float cBgColormultiHighlight{ 1.5 };
	constexpr static float cBgColormultiNormal{ 1 };
	constexpr static float cBgColormultiDark{ 0.75 };
	constexpr static float cBgChangeColormultiSpeed{ 5 };

	std::function<void()> onClicked = [] { xx::CoutN("button clicked."); };

	xx::Shared<Label> lbl;
	xx::Shared<Scale9Sprite> bg;

	// todo: color ? colormut ? anchor ?
	void Init(int z_, XY const& position_, float texScale_, xx::Shared<Frame> frame_, UVRect const& center_, std::u32string_view const& txt_) {
		z = z_;
		position = position_;
		anchor = { 0.5, 0.5 };

		XY cornerSize{ float(frame_->textureRect.w - center_.w), float(frame_->textureRect.h - center_.h) };
		static constexpr XY padding{ 20, 5 };
		lbl = MakeChildren<Label>();
		lbl->Init(z + 2, (cornerSize + padding) / 2, {}, RGBA8_White, txt_);
		size = lbl->size + cornerSize + padding;

		bg = MakeChildren<Scale9Sprite>();
		bg->Init(z + 1, {}, texScale_, size / texScale_, std::move(frame_), center_, { 0x5f, 0x15, 0xd9, 0xff });

		FillTransRecursive();
	}

	xx::TaskGuard bgChangeColormulti;
	xx::Task<> BgHighlight() {
		auto step = cBgChangeColormultiSpeed / gEngine->fps;
		while(true) {
			if (bg->colormulti < cBgColormultiHighlight) {
				bg->colormulti += step;
			}
			if (!MousePosInArea()) {
				bgChangeColormulti(gEngine->tasks, BgNormal());
			}
			co_yield 0;
		}
	}
	xx::Task<> BgNormal() {
		auto step = cBgChangeColormultiSpeed / gEngine->fps;
		for (; bg->colormulti > cBgColormultiNormal; bg->colormulti -= step) {
			co_yield 0;
		}
	}

	virtual void OnMouseDown() override {
		gEngine->mouseEventHandler = xx::WeakFromThis(this);
		bgChangeColormulti.Clear();
		bg->colormulti = cBgColormultiDark;
	}

	virtual void OnMouseMove() override {
		if (gEngine->mouseEventHandler.pointer() != this) {
			bgChangeColormulti(gEngine->tasks, BgHighlight());
		}
	}

	virtual void OnMouseUp() override {
		bg->colormulti = cBgColormultiNormal;
		gEngine->mouseEventHandler.Reset();
		if (MousePosInArea()) {
			onClicked();
		}
	}
};
