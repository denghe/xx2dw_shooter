#pragma once
#include "game_hero.h"

template<typename Owner>
struct MagicWeapon : Sprite {
	constexpr static XY cAnchor{ 0.5f, 0.1f };
	constexpr static float cRadius{ 6.f };
	constexpr static float cSpeedMin{ 50.f / gDesign.fps };
	constexpr static float cSpeedMax{ 200.f / gDesign.fps };
	constexpr static float cSpeedInc{ 10.f / gDesign.fps };
	constexpr static float cFlyRadianMin{ float(M_PI * 2 / gDesign.fps) };
	constexpr static float cFlyRadianMax{ float(M_PI * 20 / gDesign.fps) };
	constexpr static float cAroundRadiansInc{ float(M_PI * 2 / gDesign.fps) };

	float speed{ cSpeedMin };
	xx::Weak<Owner> owner;
	xx::Weak<Sprite> target;

	void Init(int index, xx::Shared<Owner> const& owner_, XY const& bornPos) {
		mainLogic = MainLogic();
		radius = cRadius;
		owner = owner_;
		pos = bornPos;
		frames = &gLooper.frames_magicWeapon;
		frameIndex = (float)index;
		body.SetRotate(radians).SetAnchor(cAnchor);
	}

	xx::Task<> MainLogic() {
		XY tarPos;
		//float lastRadians = radians;
	LabBegin:
		//if (target) {											// begin attack
		//	tarPos = pos;										// backup owner's pos
		//	// todo: first n seconds only +speed to cSpeedMax
		//	do {
		//		auto d = target->pos - pos;
		//		auto dd = d.x * d.x + d.y * d.y;
		//		if (dd <= cRadius * cRadius) {
		//			// todo: hit effect ? return ?
		//			break;
		//		}
		//		// continue fly
		//		float r = -std::atan2(d.y, d.x);
		//		RotateControl::Step(radians, r, cFlyRadianMin);
		//		r = -radians;
		//		pos += XY{ std::cos(r), -std::sin(r) } * speed;
		//		//	// todo: change speed
		//		// todo: calc - speed when angle from less than PI/2  change to greater than
		//		co_yield 0;
		//	} while (target);
		//}
		if (owner) {											// follow mode
		LabRetry:
			auto d = owner->pos - pos;
			auto dd = d.x * d.x + d.y * d.y;
			if (dd <= owner->radius * owner->radius) {			// in catch area
				float rb = -std::atan2(d.y, d.x);
				tarPos = owner->pos + XY{ std::cos(-rb), -std::sin(-rb) } * owner->radius;
				while (true) {									// step by step change current pos to rb pos
					if (target) goto LabBegin;
					if (!owner) goto LabEnd;
					d = tarPos - pos;
					dd = d.x * d.x + d.y * d.y;
					if (dd > speed * speed) {
						pos += d.Normalize() * speed;
					} else {
						pos = tarPos;
						break;
					}
					RotateControl::Step(radians, rb, cFlyRadianMin);
					co_yield 0;
				};

				while (!RotateControl::Step(radians, rb, cFlyRadianMin)) {		// step by step change current radians to rb
					if (target) goto LabBegin;
					if (!owner) goto LabEnd;
					co_yield 0;
				}

				rb = -rb;
				while (true) {									// begin rotate fly around the owner
					for (float r = rb, re = rb + float(M_PI * 2); r < re; r += cAroundRadiansInc) {
						if (target) goto LabBegin;
						if (!owner) goto LabEnd;
						radians = -r;
						pos = owner->pos + XY{ std::cos(r), -std::sin(r) } *owner->radius;
						co_yield 0;
					}
				}
			} else {											// be close the owner
				pos += d.Normalize() * speed;
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
				pos += d.Normalize() * speed;
			}
			co_yield 0;
		}
		// todo: stay on the floor? register to item container wait hero or monster pick it up ( limit by owner's slots? )
	}
};
