#pragma once
#include "engine_engine.h"

int32_t main();

constexpr GDesign<640, 480, 60> gDesign;

struct MouseEventHandlerNode;

struct GameLooper : Engine<GameLooper> {
	constexpr static float fps = gDesign.fps, frameDelay = 1.f / fps, maxFrameDelay = 1.f;

	void Init();
	void Update();
	xx::Task<> MainTask();
	void Draw();

	// events
	xx::Weak<MouseEventHandlerNode> mouseEventHandler{};	// current( focus )
	SpaceGridAB<MouseEventHandlerNode, XY> mouseEventHandlers;	// mouse down event receivers
	EM_BOOL OnMouseDown(EmscriptenMouseEvent const& e);
	EM_BOOL OnMouseMove(EmscriptenMouseEvent const& e);
	EM_BOOL OnMouseUp(EmscriptenMouseEvent const& e);

	// flags
	bool ready{};

	// res
	xx::Shared<Frame> frame_button;

	// objs
	xx::Shared<Node> root;

	// for sort
	xx::List<ZNode> tmpZNodes;

};
extern GameLooper gLooper;

/**********************************************************************************************/
/**********************************************************************************************/

struct MouseEventHandlerNode : Node, SpaceGridABItem<MouseEventHandlerNode, XY> {
	virtual void OnMouseDown() = 0;
	virtual void OnMouseMove() = 0;
	virtual void OnMouseUp() = 0;

	bool MousePosInArea() {
		auto mp = gLooper.mouseEventHandlers.max_2 + gLooper.mouse.pos;
		//XY basePos{ -size.x * anchor.x, size.y * (anchor.y + 0.5) };
		return Calc::Intersects::BoxPoint(_sgabMin, _sgabMax, mp);	// todo: calc at ?
	}

	XX_FORCE_INLINE void RegisterMouseEventHandler() {
		auto& hs = gLooper.mouseEventHandlers;
		SGABAdd(hs, hs.max_2 + position, size * scale);	// todo: calc at ?
	}

	XX_FORCE_INLINE void UpdateMouseEventHandler() {
		SGABUpdate(gLooper.mouseEventHandlers.max_2 + position, size * scale);	// todo: calc at ?
	}

	~MouseEventHandlerNode() {
		SGABRemove();
	}
};

struct Button : MouseEventHandlerNode {
	constexpr static float cBgColormultiHighlight{ 1.5 };
	constexpr static float cBgColormultiNormal{ 1 };
	constexpr static float cBgColormultiDark{ 0.75 };
	constexpr static float cBgChangeColormultiSpeed{ 5 };
	constexpr static float cBgChangeColormultiStep{ cBgChangeColormultiSpeed / gDesign.fps };

	xx::Shared<Label> lbl;
	xx::Shared<Scale9Sprite> bg;

	void Init(int z_, XY const& position_, float texScale_, xx::Shared<Frame> frame_, UVRect const& center_, std::u32string_view const& txt_) {
		// todo: color ? colormut ? anchor ?
		z = z_;
		position = position_;
		anchor = { 0.5, 0.5 };

		lbl = xx::Make<Label>();
		lbl->Init(z + 2, {}, {}, RGBA8_White, txt_);	// todo: scale
		children.Add(lbl);

		XY cornerSize{ float(frame_->textureRect.w - center_.w), float(frame_->textureRect.h - center_.h) };
		XY padding{ 20, 5 };
		size = lbl->size + cornerSize + padding;

		bg = xx::Make<Scale9Sprite>();
		bg->Init(z + 1, {}, texScale_, (lbl->size + cornerSize + padding) / texScale_, std::move(frame_), center_, { 0x5f, 0x15, 0xd9, 0xff });
		children.Add(bg);

		lbl->position = (cornerSize + padding) / 4;	// alignment

		RegisterMouseEventHandler();
	}

	xx::TaskGuard bgChangeColormulti;
	xx::Task<> BgHighlight() {
		while(true) {
			if (bg->colormulti < cBgColormultiHighlight) {
				bg->colormulti += cBgChangeColormultiStep;
			}
			if (!MousePosInArea()) {
				bgChangeColormulti(gLooper.tasks, BgNormal());
			}
			co_yield 0;
		}
	}
	xx::Task<> BgNormal() {
		for (; bg->colormulti > cBgColormultiNormal; bg->colormulti -= cBgChangeColormultiStep) {
			co_yield 0;
		}
	}

	virtual void OnMouseDown() override {
		xx::CoutN("mouse down");
		gLooper.mouseEventHandler = xx::WeakFromThis(this);
		bgChangeColormulti.Clear();
		bg->colormulti = cBgColormultiDark;
	}

	virtual void OnMouseMove() override {
		//xx::CoutN("mouse move");
		if (gLooper.mouseEventHandler.pointer() == this) {
			// todo: high light?
			// todo: inside outside check
		} else {
			bgChangeColormulti(gLooper.tasks, BgHighlight());
		}
	}

	virtual void OnMouseUp() override {
		xx::CoutN("mouse up");
		bg->colormulti = cBgColormultiNormal;
		// todo
	}
};
