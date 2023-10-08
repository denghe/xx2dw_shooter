#pragma once
#include "engine_node_derived.h"
#include "engine_label.h"
#include "engine_scale9sprite.h"

struct Button : MouseEventHandlerNode {
	constexpr static float cBgColormultiHighlight{ 1.5 };
	constexpr static float cBgColormultiNormal{ 1 };
	constexpr static float cBgColormultiDark{ 0.75 };
	constexpr static float cBgChangeColormultiSpeed{ 5 };
	constexpr static float cBgChangeColormultiSpeed2{ 2 };
	constexpr static XY cTextPadding{ 20, 5 };

	std::function<void()> onClicked = [] { xx::CoutN("button clicked."); };

	xx::Shared<Label> lbl;
	xx::Shared<Scale9Sprite> bg;

	// todo: color ? colormut ? anchor ? scale?
	void Init(int z_, XY const& position_, XY const& anchor_, float texScale_, xx::Shared<Frame> frame_, UVRect const& center_, RGBA8 color_, std::u32string_view const& txt_) {
		z = z_;
		position = position_;
		anchor = anchor_;

		XY cornerSize{ float(frame_->textureRect.w - center_.w), float(frame_->textureRect.h - center_.h) };
		lbl = MakeChildren<Label>();
		lbl->Init(z + 2, (cornerSize + cTextPadding) / 2, { 1,1 }, {}, RGBA8_White, txt_);
		size = lbl->size + cornerSize + cTextPadding;

		bg = MakeChildren<Scale9Sprite>();
		bg->Init(z + 1, {}, {}, texScale_, size, std::move(frame_), center_, color_);

		FillTransRecursive();
	}

	xx::TaskGuard bgChangeColormulti;
	xx::Task<> BgHighlight() {
		auto step = cBgChangeColormultiSpeed / gEngine->framePerSeconds;
	LabBegin:
		while(true) {
			if (bg->colormulti < cBgColormultiHighlight) {
				bg->colormulti += step;
			}
			if (!MousePosInArea()) {
				for (; bg->colormulti > cBgColormultiNormal; bg->colormulti -= step) {
					co_yield 0;
					if (MousePosInArea()) goto LabBegin;
				}
				bg->colormulti = cBgColormultiNormal;
				co_return;
			}
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
