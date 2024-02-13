#include <pch.h>
#include <all.h>

void ScenePlay2::Init() {
	rootNode.Emplace()->Init();

	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy7m, gDesign.xy7a, gLooper.s9cfg_btn, U"Back To Menu", [&]() {
		gLooper.DelaySwitchTo<SceneMainMenu>();
	});

}

void ScenePlay2::Update() {
	// todo
}

void ScenePlay2::Draw() {
	// todo
	//Quad().SetFrame(gLooper.frames_potion_4[0]).Draw();
	Quad().SetFrame(gLooper.frames_creature_slime[0]).Draw();

	gLooper.DrawNode(rootNode);
};
