#pragma once
#include "game_hero.h"

template<typename Owner>
struct MagicWeapon : Sprite {
	constexpr static XY cAnchor{ 0.5f, 0.5f };
	constexpr static float cRadius{ 6.f };
	constexpr static float cSpeedMin{ 50.f / gDesign.fps };
	constexpr static float cSpeedMax{ 200.f / gDesign.fps };
	constexpr static float cSpeedInc{ 10.f / gDesign.fps };
	constexpr static float cFlyRadianMin{ M_PI * 1 / gDesign.fps };
	constexpr static float cFlyRadianMax{ M_PI * 3 / gDesign.fps };
	constexpr static float cAroundRadiansInc{ M_PI * 2 / gDesign.fps };

	float speed{ cSpeedMin }, flyRadian{};
	bool following{};
	xx::Weak<Owner> owner;
	xx::Weak<Sprite> target;

	void Init(int index, xx::Shared<Owner> const& owner_, XY const& bornPos) {
		mainLogic = MainLogic();
		radius = cRadius;
		flyRadian = gLooper.rnd.Next<float>(cFlyRadianMin, cFlyRadianMax);
		owner = owner_;
		pos = bornPos;
		frames = &gLooper.frames_magicWeapon;
		frameIndex = index;
		body.SetRotate(radians).SetAnchor(cAnchor);
	}

	xx::Task<> MainLogic() {
		XY tarPos;
	LabBegin:
		following = {};
		if (target) {											// begin attack
			tarPos = pos;										// backup owner's pos
			auto d = target->pos - pos;
			auto r = -std::atan2(-d.y, d.x);					// aim target
			while (!StepRadians(r, cFlyRadianMax)) co_yield 0;
			while (target) {
				d = target->pos - pos;
				auto dd = d.x * d.x + d.y * d.y;
				if (dd <= cRadius * cRadius) {
					// todo: hit effect ? return ?
					//break;
				}
				// continue fly
				auto r = -std::atan2(-d.y, d.x);
				StepRadians(r, flyRadian);						// step change angle
				r = -radians;
				pos += XY{ std::cos(r), -std::sin(r) } * speed;
				//printf("%f %f\n", inc.x, inc.y);
				// todo: change speed
				// todo: first n seconds only +speed to cSpeedMax
				// todo: calc - speed when angle from less than PI/2  change to greater than
				//float lastRadians = radians;
				co_yield 0;
			}
		}
		if (owner) {											// follow mode
		LabRetry:
			auto d = owner->pos - pos;
			auto r = -std::atan2(-d.y, d.x);					// aim owner
			while (!StepRadians(r, cFlyRadianMax)) co_yield 0;
			auto dd = d.x * d.x + d.y * d.y;
			auto n = owner->radius * 3;
			if (dd <= n * n) {									// in catch area
				float rb = -std::atan2(d.y, d.x);
				tarPos = owner->pos + XY{ std::cos(-rb), -std::sin(-rb) } * n;
				while (true) {									// step by step change current pos to rb pos
					if (target) goto LabBegin;
					if (!owner) goto LabEnd;
					d = tarPos - pos;
					dd = d.x * d.x + d.y * d.y;
					if (dd > speed * speed) {
						pos += d.MakeNormalize() * speed;
					} else {
						pos = tarPos;
						break;
					}
					StepRadians(rb, cFlyRadianMin);
					co_yield 0;
				};

				while (!StepRadians(rb, cFlyRadianMin)) {		// step by step change current radians to rb
					if (target) goto LabBegin;
					if (!owner) goto LabEnd;
					co_yield 0;
				}

				rb = -rb;
				following = true;
				while (true) {									// begin rotate fly around the owner
					for (float r = rb, re = rb + M_PI * 2; r < re; r += cAroundRadiansInc) {
						if (target) goto LabBegin;
						if (!owner) goto LabEnd;
						radians = -r;
						pos = owner->pos + XY{ std::cos(r), -std::sin(r) } * n;
						co_yield 0;
					}
				}
			} else {											// be close the owner
				pos += d.MakeNormalize() * speed;
				if (speed < cSpeedMax) {
					speed += cSpeedInc;
				}
				co_yield 0;
				goto LabRetry;
			}
		}

	LabEnd:
		while (true) {											// lost owner: step by step change current pos to bak pos
			auto d = tarPos - pos;
			auto dd = d.x * d.x + d.y * d.y;
			if (dd <= speed * speed) {							// close target pos
				pos = tarPos;
				co_return;
			} else {
				pos += d.MakeNormalize() * speed;
			}
			co_yield 0;
		}
		// todo: stay on the floor? register to item container wait hero or monster pick it up ( limit by owner's slots? )
	}
};
