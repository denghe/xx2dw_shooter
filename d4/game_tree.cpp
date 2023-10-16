#include "pch.h"

void Tree::Init(float x, float y) {
	mainLogic = MainLogic();

	pos.x = x + cXOffset;
	pos.y = y + gLooper.tileHeight - cYOffset;

	SGABAdd(gLooper.sgabTrees, pos.As<int32_t>(), Vec2<>(cBoxWidth, cBoxHeight));

	auto ay = cYOffset / gLooper.frame_tree->spriteSize.y;
	body.SetFrame(gLooper.frame_tree).SetAnchor({ 0.5, ay });
}

void Tree::Draw() {
	auto& c = gLooper.camera;
	if (c.InArea(pos)) {
		body.SetScale(c.scale)
			.SetRotate(radians)
			.SetPosition(c.ToGLPos(pos))
			.Draw();
	}
}

Tree::~Tree() {
	SGABRemove();
}

xx::Task<> Tree::MainLogic() {
	// shake effect
	constexpr float r1 = -0.07f, r2 = 0.07f, step = 0.03f * 60 / gDesign.fps;
	while (true) {	// repeat forever
		for (radians = r1; radians < r2; radians += step) {
			co_yield 0;
		}
		for (radians = r2; radians >= r1; radians -= step) {
			co_yield 0;
		}
	}
}
