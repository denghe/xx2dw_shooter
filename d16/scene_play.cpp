#include <pch.h>
#include <all.h>

void ScenePlay::Init() {
	rootNode.Emplace()->Init();
	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy7m, gDesign.xy7a, gLooper.s9cfg_btn, U"Back To Menu", [&]() {
		gLooper.DelaySwitchTo<SceneMainMenu>();
	});

	xx::MakeShared<Gun1<Bullet1>>()->Init(&im);
}

void ScenePlay::Update() {
	im.Update();
}

void ScenePlay::Draw() {
	im.items.Foreach([&](xx::Shared<Item>& o) {
		if (o->drawable) {	// todo: sort by y ?
			o->Draw(camera);
		}
	});

	gLooper.DrawNode(rootNode);
};
