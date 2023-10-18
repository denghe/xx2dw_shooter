#include <pch.h>

xx::Task<> GameLooper::MainTask() {

	auto frameLaser = LoadFrame("res/laser.png");
	auto frameButton = LoadFrame("res/button.png");

	auto quadLaser = xx::Make<Quad>();
	quadLaser->SetFrame(frameLaser);

	auto quadButton = xx::Make<Quad>();
	quadButton->SetFrame(frameButton);

	XY pos{ 0,0 };
	XY scale{ 1,1 };
	XY anchor{ 0.5, 0.5 };
	float z{ 0 }, w{ 120 };
	auto c = RGBA8_Red;

	//rn.Emplace()->Init(z, pos, scale, anchor, w)
	//	.AddPicture(quadButton, VAligns::Center)
	//	.AddPicture(quadLaser, VAligns::Center)
	//	.Commit();

	//rn.Emplace()->Init(z, pos, scale, anchor, w)
	//	.AddSpace({ 24,1 })
	//	.AddText(U"as\ndf")
	//	.AddText(U"asdf", c)
	//	.AddText(U"asdf")
	//	.AddText(U"asdf", c)
	//	.AddText(U"asdf")
	//	.AddText(U"asdf", c)
	//	.Commit();

	rn.Emplace()->Init(z, pos, scale, anchor, w)
		.AddSpace(24)
		.AddText(U"as\ndf")
		.AddText(U"asdf")
		.AddText(U"asdf", c)
		.AddPicture(quadButton, VAligns::Bottom)
		.AddPicture(quadLaser)
		.AddText(U"asdf")
		.AddText(U"asdf", c)
		.AddPicture(quadButton, VAligns::Bottom)
		.AddPicture(quadLaser)
		.AddText(U"asdf")
		.AddText(U"asdf", c)
		.AddPicture(quadButton, VAligns::Bottom)
		.AddPicture(quadLaser)
		.Commit();

	co_return;
}

void GameLooper::Update() {
}

void GameLooper::Draw() {
	rn->Draw();
}
