#pragma once
#include <engine_base2.h>
#include <engine_label.h>
#include <engine_button.h>

template<typename T> concept Has_Init = requires(T t) { { t.Init() } -> std::same_as<void>; };
template<typename T> concept Has_AfterInit = requires(T t) { { t.AfterInit() } -> std::same_as<void>; };
template<typename T> concept Has_Update = requires(T t) { { t.Update() } -> std::same_as<void>; };
template<typename T> concept Has_Draw = requires(T t) { { t.Draw() } -> std::same_as<void>; };
template <typename T> concept Has_MainTask = requires(T t) { { t.MainTask() } -> std::same_as<xx::Task<>>; };

template <typename T> concept Has_OnKeyPress = requires(T t) { { t.OnKeyPress(std::declval<EmscriptenKeyboardEvent const&>()) } -> std::same_as<EM_BOOL>; };
template <typename T> concept Has_OnKeyDown = requires(T t) { { t.OnKeyDown(std::declval<EmscriptenKeyboardEvent const&>()) } -> std::same_as<EM_BOOL>; };
template <typename T> concept Has_OnKeyUp = requires(T t) { { t.OnKeyUp(std::declval<EmscriptenKeyboardEvent const&>()) } -> std::same_as<EM_BOOL>; };

template <typename T> concept Has_OnMouseDown = requires(T t) { { t.OnMouseDown(std::declval<EmscriptenMouseEvent const&>()) } -> std::same_as<EM_BOOL>; };
template <typename T> concept Has_OnMouseUp = requires(T t) { { t.OnMouseUp(std::declval<EmscriptenMouseEvent const&>()) } -> std::same_as<EM_BOOL>; };
template <typename T> concept Has_OnClick = requires(T t) { { t.OnClick(std::declval<EmscriptenMouseEvent const&>()) } -> std::same_as<EM_BOOL>; };
template <typename T> concept Has_OnDblClick = requires(T t) { { t.OnDblClick(std::declval<EmscriptenMouseEvent const&>()) } -> std::same_as<EM_BOOL>; };
template <typename T> concept Has_OnMouseMove = requires(T t) { { t.OnMouseMove(std::declval<EmscriptenMouseEvent const&>()) } -> std::same_as<EM_BOOL>; };
template <typename T> concept Has_OnMouseEnter = requires(T t) { { t.OnMouseEnter(std::declval<EmscriptenMouseEvent const&>()) } -> std::same_as<EM_BOOL>; };
template <typename T> concept Has_OnMouseLeave = requires(T t) { { t.OnMouseLeave(std::declval<EmscriptenMouseEvent const&>()) } -> std::same_as<EM_BOOL>; };
template <typename T> concept Has_OnMouseOver = requires(T t) { { t.OnMouseOver(std::declval<EmscriptenMouseEvent const&>()) } -> std::same_as<EM_BOOL>; };
template <typename T> concept Has_OnMouseOut = requires(T t) { { t.OnMouseOut(std::declval<EmscriptenMouseEvent const&>()) } -> std::same_as<EM_BOOL>; };

template <typename T> concept Has_OnTouchStart = requires(T t) { { t.OnTouchStart(std::declval<EmscriptenTouchEvent const&>()) } -> std::same_as<EM_BOOL>; };
template <typename T> concept Has_OnTouchMove = requires(T t) { { t.OnTouchMove(std::declval<EmscriptenTouchEvent const&>()) } -> std::same_as<EM_BOOL>; };
template <typename T> concept Has_OnTouchEnd = requires(T t) { { t.OnTouchEnd(std::declval<EmscriptenTouchEvent const&>()) } -> std::same_as<EM_BOOL>; };
template <typename T> concept Has_OnTouchCancel = requires(T t) { { t.OnTouchCancel(std::declval<EmscriptenTouchEvent const&>()) } -> std::same_as<EM_BOOL>; };

struct EngineBase3 : EngineBase2 {
	XX_FORCE_INLINE static EngineBase3& Instance() { return *(EngineBase3*)gEngine; }

};
