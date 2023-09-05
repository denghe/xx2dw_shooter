#include "pch.h"

void MonsterBase::RemoveFromOwner() {
	owner->Remove(ivAtOwner);
}

void MonsterBase::GridInit() {
	SGCInit(&gLooper.sgc, gGridBasePos.MakeAdd(pos));
}

void MonsterBase::GridUpdate() {
	SGCUpdate(gGridBasePos.MakeAdd(pos));
}

MonsterBase::~MonsterBase() {
	SGCTryRemove();
}
