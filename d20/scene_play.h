#pragma once
#include <game_looper.h>
#include <hero.h>

struct Grass {
	static constexpr float cScale{ 4 };

	struct D {
		int32_t idx;
		XY pos;
	};
	xx::List<Quad, int32_t> qs;
	xx::List<D, int32_t> ds;

	void Init() {
		auto len = gLooper.frames_grass.len;
		qs.Resize(len);
		for (int32_t i = 0; i < len; i++) {
			qs[i].SetFrame(gLooper.frames_grass[i]).SetScale(cScale);
		}

		auto& r = gLooper.rnd;
		XY wh_2 = gLooper.windowSize_2;
		auto to = len - 2;
		for (size_t i = 0; i < 10000; i++) {
			ds.Emplace(D{ r.Next<int32_t>(to)
				, r.Next<float>(-wh_2.x, wh_2.x), r.Next<float>(-wh_2.y, wh_2.y) });
		}
	}

	void Draw() {
		// todo: grid cut?
		for (auto& d : ds) {
			auto& q = qs[d.idx];
			q.SetPosition(d.pos).Draw();
		}
	}
};

struct ScenePlay : Scene {
	xx::Shared<Node> rootNode;

	Grass grass;
	xx::List<xx::Ref<Hero>, int32_t> heroes;

	virtual void Init() override;
	virtual void Draw() override;
};
