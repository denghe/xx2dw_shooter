#pragma once
#include "game_drawable_sprite.h"

struct DamageNumber : Drawable {
	constexpr static char const* cResPrefix{ "number_outlined_" };
	constexpr static float cMoveSpeedMin { 20 / gDesign.fps };
	constexpr static float cMoveSpeedMax { 50 / gDesign.fps };
	constexpr static float cMoveDurationSeconds{ 0.5 };
	constexpr static float cFadeOutDurationSeconds{ 0.2 };
	constexpr static float cCharPixelWidth{ 8 };

	std::array<UVRect, 12> rects{};									// todo: store pos ?
	int size{};
	GLuint texId{};
	RGBA8 color{};
	XY d{};

	XX_FORCE_INLINE void SetDamage(int i) {
		size = {};
		while (i >= 10) {
			int a = i / 10;
			int b = i - a * 10;
			rects[size++].data = gLooper.frames_number_outlined[b]->textureRect.data;
			i = a;
		}
		rects[size++].data = gLooper.frames_number_outlined[i]->textureRect.data;
	}

	void Init(XY const& pos_, XY const& d_, RGBA8 color_, int damage) {
		mainLogic = MainLogic_();
		color = color_;
		texId = gLooper.frames_number_outlined[0]->tex->GetValue();
		SetDamage(damage);
		pos = { pos_.x - cCharPixelWidth * size / 2, pos_.y };
		d = d_;
	}

	virtual void Draw() override {
		auto scale = gLooper.camera.scale;
		auto beginPos = gLooper.camera.ToGLPos(pos);
		auto widthInc = cCharPixelWidth * scale;
		auto qs = EngineBase1::Instance().ShaderBegin(EngineBase1::Instance().shaderQuadInstance).Draw(texId, size);
		for (int i = 0; i < size; ++i) {
			auto& q = qs[i];
			q.anchor = { 0, 0.5f };
			q.color = color;
			q.colorplus = 1;
			q.pos = beginPos;
			q.radians = 0;
			q.scale = { scale, scale };
			q.texRect.data = rects[size - i - 1].data;
			beginPos.x += widthInc;
		}
	}

	xx::Task<> MainLogic_() {
		// move away
		float ds{ cMoveDurationSeconds };
		auto inc = d.MakeNormalize() * gEngine->rnd.Next<float>(cMoveSpeedMin, cMoveSpeedMax);
		for (int i = 0; i < cMoveDurationSeconds / gDesign.frameDelay; ++i) {
			pos += inc;
			co_yield 0;
		}

		// fade out
		constexpr int lifeCycle_fadeout{ cFadeOutDurationSeconds / gDesign.frameDelay };
		constexpr float step = 255.f / lifeCycle_fadeout;
		for (float a = 255.f; a >= 0; a -= step) {
			color.a = a;
			co_yield 0;
		}
		color.a = 0;
	}

};
