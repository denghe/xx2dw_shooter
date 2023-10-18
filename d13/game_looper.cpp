#include <pch.h>

xx::Task<> GameLooper::MainTask() {
#ifdef __EMSCRIPTEN__
	auto tp = co_await AsyncLoadTexturePackerFromUrl("res/dungeon.blist");
#else
	auto tp = LoadTexturePacker("res/dungeon.blist");
#endif

	xx::List<xx::Shared<Frame>, int32_t> frames_gem;
	tp->GetToByPrefix(frames_gem, "icon_gem_");

	XY pos{ 0,0 };
	XY scale{ 1,1 };
	XY anchor{ 0.5, 0.5 };
	float w{ 500 };
	auto c = RGBA8_Red;

	node.Emplace()->FillTrans();
	node->MakeChildren<RichLabel>()->Init(1, pos, scale, anchor, w)
		.SetHAlign(HAligns::Center).AddText(U"halign center\n", {2,2})
		.SetHAlign(HAligns::Right).AddText(U"halign right\n", {0.5, 0.5})
		.SetHAlign()
		.AddPicture(frames_gem[0]).AddText(U" this is gem0. aksd fhkhsd kfhas dfjd kfljslf.\n")
		.AddPicture(frames_gem[1]).AddText(U" this is gem1. aksd fhkhsd kfhas dfjd kfljslf.\n")
		.AddPicture(frames_gem[2]).AddText(U" this is gem2. aksd fhkhsd kfhas dfjd kfljslf.\n")
		.AddPicture(frames_gem[3]).AddText(U" this is gem3. aksd fhkhsd kfhas dfjd kfljslf.\n")
		.AddPicture(frames_gem[4]).AddText(U" this is gem4. aksd fhkhsd kfhas dfjd kfljslf.\n")
		.AddPicture(frames_gem[5]).AddText(U" this is gem5. aksd fhkhsd kfhas dfjd kfljslf.\n")
		.AddPicture(frames_gem[6]).AddText(U" this is gem6. aksd fhkhsd kfhas dfjd kfljslf.\n")
		.AddPicture(frames_gem[7]).AddText(U" this is gem7. aksd fhkhsd kfhas dfjd kfljslf.\n")
		.AddPicture(frames_gem[8]).AddText(U" this is gem8. aksd fhkhsd kfhas dfjd kfljslf.\n")
		.AddPicture(frames_gem[9]).AddText(U" this is gem9. aksd fhkhsd kfhas dfjd kfljslf.\n")
		.Commit();
	//node->MakeChildren<Scale9Sprite

	co_return;
}

void GameLooper::Update() {
	if (!node) return;
}

void GameLooper::Draw() {
	if (!node) return;
	FillZNodes(tmpZNodes, node);
	OrderByZDrawAndClear(tmpZNodes);
}
