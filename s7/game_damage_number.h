#pragma once
#include "game_drawable_sprite.h"

struct DamageNumber : Drawable {
	using ThisType = DamageNumber;
	constexpr static char const* cResPrefix{ "number_outlined_" };
	constexpr static float cMoveUpSpeed{ 20 / gDesign.fps };
	constexpr static float cMoveUpDurationSeconds{ 1 };
	constexpr static float cFadeOutDurationSeconds{ 1 };

	std::array<UVRect, 16> data;
	int size;
	GLuint texId;
	RGBA8 color;

	XX_FORCE_INLINE int SetDamage(int i) {
		size = {};
		while (i >= 10) {
			auto a = i / 10;
			auto b = i - a * 10;
			data[size++].data = gLooper.frames_number_outlined[b]->textureRect.data;
			i = a;
		}
		data[size++].data = gLooper.frames_number_outlined[i]->textureRect.data;
	}

	void Init(XY const& pos_, RGBA8 color_, int damage) {
		InitGetYDrawUpate<ThisType>();
		pos = pos_;
		color = color_;
		SetDamage(damage);
	}

	void Draw() {
		auto& s = gEngine->shader;
		for (int i = 0; i < size; ++i) {
			auto& q = *s.Draw(texId, 1);
			q.anchor = { 0.f, 0.5f };
			q.color = color;
			q.colormulti = 1;
			q.pos = pos;								// todo: calc padding
			q.radians = {};
			q.scale = { 1, 1 };
			q.texRect.data = data[i].data;
		}
	}

	xx::Task<> MainLogic{ MainLogic_() };
	xx::Task<> MainLogic_() {
		constexpr int lifeCycle_move{ cMoveUpDurationSeconds / gDesign.frameDelay };
		constexpr int lifeCycle_fadeout{ cFadeOutDurationSeconds / gDesign.frameDelay };
		while (true) {
			// move up
			float ds{ cMoveUpDurationSeconds };
			for (int i = 0; i < lifeCycle_move; ++i) {
				pos.y -= cMoveUpSpeed;
				co_yield 0;
			}

			// fade out
			constexpr float step = 255.f / lifeCycle_fadeout;
			for (float a = 255.f; a >= 0; a -= step) {
				color.a = a;
				co_yield 0;
			}
			color.a = 0;
		}
	}

};
