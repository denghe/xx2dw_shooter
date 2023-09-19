#include "pch.h"

void MonsterBase::RemoveFromOwner() {
	owner->Remove(ivAtOwner);
}

void MonsterBase::GridInit() {
	SGCAdd(gLooper.sgcMonsters, gGridBasePos.MakeAdd(pos));
}

void MonsterBase::GridUpdate() {
	SGCUpdate(gGridBasePos.MakeAdd(pos));
}

MonsterBase::~MonsterBase() {
	SGCRemove();
}
