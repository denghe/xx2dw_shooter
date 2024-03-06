#pragma once
#ifndef XX2DW_SHOOTER_D23_PCH_H_
#define XX2DW_SHOOTER_D23_PCH_H_

#include <engine.h>
#include <xx_ecs.h>
template<typename T>
using LDL = xx::ListDoubleLink<T, int32_t, uint32_t>;
using LDLIV = xx::ListDoubleLinkIndexAndVersion<int32_t, uint32_t>;

#endif
