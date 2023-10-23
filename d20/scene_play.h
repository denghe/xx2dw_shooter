#pragma once
#include <game_looper.h>
#include <hero.h>

struct HpBarCache : protected DynamicTexturePacker<4096> {

	std::unordered_map<uint32_t, xx::Ref<Frame>> bars;

	XX_FORCE_INLINE uint32_t GetKey(uint16_t width, uint16_t value) {
		return uint32_t((width << 16) | value);
	}

	// all hp value bar -> textures -> frames -> dynamic pack
	void Init(float height, std::initializer_list<int> widths) {
		FrameBuffer fb;
		fb.Init();
		std::vector<xx::Ref<Frame>> fs;
		Scale9Sprite s9;	// bg
		auto& cfg = gLooper.s9cfg_hp;
		Quad q;				// hp
		q.SetFrame(gLooper.frame_dot_1_22).SetColor({ 255,0,0,255 }).SetAnchor({ 0, 0.5 }); // 204 ?
		for (auto w : widths) {
			s9.Init(0, {}, { 0.5, 0.5 }, cfg.GetCornerSize() + XY{ (float)w, height }, cfg);
			s9.FillTrans();
			for (int i = 0; i < w; ++i) {
				auto tex = FrameBuffer::MakeTexture({ (uint32_t) w + 2, (uint32_t)height + 2 });
				fb.DrawTo(tex, {}, [&] {
					auto w_2 = i / 2.f;
					q.SetScale({ w_2, height / 2 }).SetPosition({ -w_2, 0 }).Draw();
					s9.Draw();
				});
				fs.emplace_back(Frame::Create(tex))->ud = GetKey(w, i);
			}
		}
		xx_assert(Fill(fs));
		for (auto& f : fs) {
			bars.insert(std::make_pair((uint32_t)f->ud, f));
		}
	}

	// get bar by width + value
	xx::Ref<Frame> GetFrame(uint16_t width, uint16_t value) {
		if (auto iter = bars.find(GetKey(width, value)); iter != bars.end()) {
			return iter->second;
		}
		return {};
	}

};


struct Grass : Quad {
	static constexpr float cScale{ 4 };
	FrameBuffer fb;
	void Init() {
		auto tex = FrameBuffer::MakeTexture(gLooper.windowSize.As<uint32_t>());
		fb.Init().DrawTo(tex, RGBA8{ 0x6a, 0xbe, 0x30, 0xff }, [&] {
			xx::List<Quad, int32_t> qs;
			auto len = gLooper.frames_grass.len;
			qs.Resize(len);
			for (int32_t i = 0; i < len; i++) {
				qs[i].SetFrame(gLooper.frames_grass[i]).SetScale(cScale);
			}
			auto& r = gLooper.rnd;
			XY wh_2 = gLooper.windowSize_2;
			auto to = len - 2;
			for (size_t i = 0; i < 10000; i++) {
				auto& q = qs[r.Next<int32_t>(to)];
				XY pos{ r.Next<float>(-wh_2.x, wh_2.x), r.Next<float>(-wh_2.y, wh_2.y) };
				q.SetPosition(pos).Draw();
			}
		});
		SetFrame(Frame::Create(tex));
	}
	// todo: blood effect frame 1-8 draw to tex
};

struct ScenePlay : Scene {
	xx::Shared<Node> rootNode;

	HpBarCache hbc;
	Grass grass;
	xx::List<xx::Ref<Hero>, int32_t> heroes;

	virtual void Init() override;
	virtual void Draw() override;
};
