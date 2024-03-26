#include "pch.h"

void MonsterBase::RemoveFromOwner() {
	owner->Remove(ivAtOwner);
}

void MonsterBase::GridInit() {
	SGCAdd(gLooper.sgcMonsters, gGridBasePos + pos);
}

void MonsterBase::GridUpdate() {
	SGCUpdate(gGridBasePos + pos);
}

MonsterBase::~MonsterBase() {
	SGCRemove();
}
