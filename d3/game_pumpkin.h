#pragma once
#include "game_looper.h"

struct Pumpkin : CircleObj {
	static constexpr XYi cSize{ 16, 24 };
	static constexpr int cRadius{ (cSize.x + (cSize.x % 2 == 0 ? 0 : 1)) / 2 };
	//static constexpr float cScale{ 1 };
	static constexpr float cSpeed{ 3 };
	constexpr static float cFrameInc{ 8 / gDesign.fps };
	constexpr static float cFrameMaxIndex{ 4.f };

	float frameIndex{};

	void Init(XYi const& pos_) {
		radius = cRadius;
		frameIndex = gEngine->rnd.Next<float>(cFrameMaxIndex - 0.1f);
		pos = pos_;
		SGCAdd(gLooper.sgcMonsters, pos_);
		quad.SetAnchor({ 0.5, float(cSize.x) / 2 / cSize.y });
	}

	xx::Task<> mainLogic{ MainLogic() };
	xx::Task<> MainLogic() {
		while (true) {
			frameIndex += cFrameInc;					// step frame anim ( todo: by move distance ?)
			if (frameIndex >= cFrameMaxIndex) {			// loop step
				frameIndex -= cFrameMaxIndex;
			}

			// physics simulate
			// calc neighbor cross force
			XY combineForce{};
			int numCross{}, limit = 15;
			auto crIdx = _sgc->PosToCrIdx(pos);
			_sgc->Foreach9(crIdx, [&](CircleObj* o) {
				if (o == this) return false;											// skip self
				auto d = pos - o->pos;
				auto rr = (o->radius + radius) * (o->radius + radius);
				auto dd = d.x * d.x + d.y * d.y;
				if (rr > dd) {															// cross?
					++numCross;
					if (dd) {
						combineForce += d.As<float>() / std::sqrt((float)dd);			// normalize
					}
				}
				return --limit < 0;														// number limit
			});

			auto newPos = pos;
			if (numCross) {
				//printf("numCross = %d\n", numCross);
				if (combineForce.IsZero()) {											// move by random angle
					auto r = gEngine->rnd.Next<float>(float(M_PI * 2));
					newPos += (XY{ std::cos(r), std::sin(r) } + XY{ 0, 0.1f }) * cSpeed;
				} else {																// move by v
					newPos += (combineForce.Normalize() + XY{ 0, 0.1f }) * cSpeed;
				}
			}

			//// calc follow force
			//auto d = gLooper.??????->_sgcPos - _sgcPos;
			//auto dd = d.x * d.x + d.y * d.y;

			//// calc new pos
			//auto newPos = pos;
			//if (numCross) {											// cross?
			//	if (dd) {
			//		combineForce += d / std::sqrt(dd) / 100;		// weak force assign for ship follow
			//	}
			//	if (combineForce.x * combineForce.x < 0.0001 && combineForce.y * combineForce.y < 0.0001) {
			//		auto r = gEngine->rnd.Next<float>(M_PI * 2);
			//		newPos += XY{ std::cos(r), std::sin(r) } *cSpeed * 3;
			//	} else {
			//		newPos += combineForce.Normalize() * cSpeed;
			//	}
			//} else {
			//	if (dd > cSpeed * cSpeed) {							// follow shooter directly
			//		newPos += d / std::sqrt(dd) * cSpeed;			// normalize
			//	} else {
			//		newPos = gLooper.shooter->pos;
			//	}
			//}

			// get box & fix newPos
			auto& sg = gLooper.sgabWalls;
			auto minXY = newPos - radius, maxXY = newPos + radius;
			if (minXY.x < 0) minXY.x = 0;
			if (minXY.y < 0) minXY.y = 0;
			if (maxXY.x >= sg.max.x) maxXY.x = sg.max.x - 1;
			if (maxXY.y >= sg.max.y) maxXY.y = sg.max.y - 1;
			if (minXY.x < maxXY.x && minXY.y < maxXY.y) {
				//printf("ForeachAABB\n");
				sg.ForeachAABB(minXY, maxXY);
				auto guard = xx::MakeSimpleScopeGuard([&] { sg.ClearResults(); });
				for (auto& b : sg.results) {
					TranslateControl::MoveCircleIfIntersectsBox<int>(b->_sgabPos.x, b->_sgabPos.y, b->_sgabRadius.x, b->_sgabRadius.y , newPos.x, newPos.y, radius);
				}
			}

			// upgrade space grid
			if (newPos != pos) {
				pos = newPos;
				SGCUpdate(pos);
			}
			// random move ? lisinging player ?

			co_yield 0;
		}
	}

	operator YObj() const {
		return { pos.y - gCellSize.y / 2, this };
	}

	void Draw() const override {
		quad.SetFrame(gLooper.frames_pumpkin[(int32_t)frameIndex])
			.SetScale(gLooper.camera.scale)
			.SetPosition(gLooper.camera.ToGLPos(pos))
			.Draw();
	}
};
