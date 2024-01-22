#pragma once
#include "game_drawable_sprite.h"

// todo: Scientific notation ?
struct EffectNumber  {
	constexpr static float cCharPixelWidth{ 8 };

	constexpr static float cMoveDurationSeconds{ 0.5f };
	constexpr static float cMoveSpeedMin { 20 / gDesign.fps };
	constexpr static float cMoveSpeedMax { 50 / gDesign.fps };

	constexpr static float cFadeOutDurationSeconds{ 0.2f };
	constexpr static float cFadeOutInc = 255.f / (cFadeOutDurationSeconds / gDesign.frameDelay);

	GLuint texId{};					// number texture's gl id
	std::array<char, 12> buf{};		// value to string cache
	int len{};						// buf's len

	int lineNumber{};
	RGBA8 color{};
	XY pos{}, inc{};
	float f{};

	XX_FORCE_INLINE void FillBuf(int i) {
		len = {};
		while (i >= 10) {
			int a = i / 10;
			buf[len++] = i - a * 10;
			i = a;
		}
		buf[len++] = i;
	}

	// pos: original position,  dist: determine move direction
	void Init(XY const& pos_, XY const& dist_, RGBA8 color_, int value_) {
		texId = gLooper.frames_number_outlined[0]->tex->GetValue();
		FillBuf(value_);
		pos = { pos_.x - cCharPixelWidth * len / 2, pos_.y };	// calculate center point
		inc = dist_.MakeNormalize() * gEngine->rnd.Next<float>(cMoveSpeedMin, cMoveSpeedMax);	// calculate move frame inc with random speed
		color = color_;
	}

	int UpdateCore() {
		COR_BEGIN
		// move away
		for (f = 0; f < cMoveDurationSeconds; f += gDesign.frameDelay) {
			pos += inc;
			COR_YIELD
		}
		// fade out
		for (f = 255.f; f >= 0; f -= cFadeOutInc) {
			color.a = (uint8_t)f;
			COR_YIELD
		}
		color.a = 0;
		COR_END
	}

	XX_FORCE_INLINE bool Update() {
		lineNumber = UpdateCore();
		return lineNumber == 0;		// will remove by caller
	}

	void Draw(Camera const& camera) {
		auto& eb = EngineBase1::Instance();
		auto qs = eb.ShaderBegin(eb.shaderQuadInstance).Draw(texId, len);
		auto basePos = camera.ToGLPos(pos);
		auto scale = camera.scale;
		auto widthInc = cCharPixelWidth * scale;
		auto& fs = gLooper.frames_number_outlined;
		for (int i = 0; i < len; ++i) {
			auto& q = qs[i];
			q.anchor = { 0, 0.5f };
			q.color = color;
			q.colorplus = 1;
			q.pos = basePos;
			q.radians = 0;
			q.scale = { scale, scale };
			q.texRect.data = fs[buf[len - i - 1]]->textureRect.data;
			basePos.x += widthInc;
		}
	}

};

namespace xx {
	template<>
	struct IsPod<EffectNumber, void> : std::true_type {};
}

struct EffectNumberManager1 {
	xx::Queue<EffectNumber> ens;

	void Init(size_t cap) {
		ens.Reserve(cap);
	}

	void Add(XY const& pos_, XY const& dist_, RGBA8 color_, int value_) {
		ens.Emplace().Init(pos_, dist_, color_, value_);
	}

	bool Update() {
		size_t n{};
		for (size_t i = 0, e = ens.Count(); i < e; ++i) {
			if (ens[i].Update()) {
				++n;
			}
		}
		ens.PopMulti(n);
		return ens.Empty();
	}

	void Draw(Camera const& camera) {
		for (size_t i = 0, e = ens.Count(); i < e; ++i) {
			ens[i].Draw(camera);
		}
	}
};

struct EffectNumberManager2 {
	xx::ListLink<EffectNumber> ens;

	void Init(size_t cap) {
		ens.Reserve(cap);
	}

	void Add(XY const& pos_, XY const& dist_, RGBA8 color_, int value_) {
		ens.Emplace().Init(pos_, dist_, color_, value_);
	}

	bool Update() {
		ens.Foreach([](EffectNumber& en)->bool {
			return en.Update();
		});
		return ens.Empty();
	}

	void Draw(Camera const& camera) {
		ens.Foreach([&](EffectNumber& en)->void {
			en.Draw(camera);
		});
	}
};

struct EffectNumberManager3 {
	xx::ListLink<xx::Shared<EffectNumber>> ens;

	void Init(size_t cap) {
		ens.Reserve(cap);
	}

	void Add(XY const& pos_, XY const& dist_, RGBA8 color_, int value_) {
		ens.Emplace().Emplace()->Init(pos_, dist_, color_, value_);
	}

	bool Update() {
		ens.Foreach([](auto& en)->bool {
			return en->Update();
		});
		return ens.Empty();
	}

	void Draw(Camera const& camera) {
		ens.Foreach([&](auto& en)->void {
			en->Draw(camera);
			});
	}
};
