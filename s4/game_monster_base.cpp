#include "pch.h"

void MonsterBase::RemoveFromOwner() {
	owner->Remove(ivAtOwner);
}

void MonsterBase::GridInit() {
	SGCInit(gLooper.sgcMonsters);
	SGCSetPos(gGridBasePos.MakeAdd(pos));
	SGCAdd();
}

void MonsterBase::GridUpdate() {
	SGCSetPos(gGridBasePos.MakeAdd(pos));
	SGCUpdate();
}

MonsterBase::~MonsterBase() {
	SGCTryRemove();
}
