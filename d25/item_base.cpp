#include "pch.h"
#include "scene_play.h"

bool ItemBase::IsFocus() const {
	auto& f = gScenePlay->focus;
	return f.typeId == this->typeId && f.idx == this->__grid_idx && f.version == this->__grid_version;
}

void TowerBase::InitFocusAnim() {
	case_focusScale = -1;
	focusScale = 1.f;
}

void TowerBase::ResetFocusAnim() {
	case_focusScale = 0;
	focusScale = 1.f;
}

void TowerBase::StepFocusAnim() {
	switch (case_focusScale) {
	case 0:
		for (; focusScale < 1.05f; focusScale += 1.f / gDesign.fps) {
			case_focusScale = 1;
			return;
	case 1:;
		}
		for (; focusScale > 1.f; focusScale -= 1.f / gDesign.fps) {
			case_focusScale = 2;
			return;
	case 2:;
		}
		InitFocusAnim();
	default:;
	}
}

void TowerBase::InitShootAnim() {
	case_shootScale = -1;
	shootScale = 1.f;
}

void TowerBase::ResetShootAnim() {
	case_shootScale = 0;
	shootScale = 1.f;
}

void TowerBase::StepShootAnim() {
	switch (case_shootScale) {
	case 0:
		for (; shootScale > 0.7f; shootScale -= 3.f / gDesign.fps) {
			case_shootScale = 1;
			return;
	case 1:;
		}
		for (; shootScale < 1.f; shootScale += 3.f / gDesign.fps) {
			case_shootScale = 2;
			return;
	case 2:;
		}
		InitShootAnim();
	default:;
	}
}
