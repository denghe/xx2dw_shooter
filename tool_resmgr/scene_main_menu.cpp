#include "pch.h"
#include "looper.h"
#include "scene_main_menu.h"
#include "scene_pivot_editor.h"

namespace Scene_MainMenu {
	void Scene::ImGuiUpdate() {
#if 1
		auto& io = ImGui::GetIO();
		auto mvp = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(mvp->WorkPos, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(mvp->WorkSize, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags wf{};
		wf |= ImGuiWindowFlags_NoTitleBar;
		wf |= ImGuiWindowFlags_NoScrollbar;
		wf |= ImGuiWindowFlags_MenuBar;
		wf |= ImGuiWindowFlags_NoMove;
		wf |= ImGuiWindowFlags_NoResize;
		wf |= ImGuiWindowFlags_NoCollapse;
		wf |= ImGuiWindowFlags_NoNav;
		wf |= ImGuiWindowFlags_NoBackground;
		wf |= ImGuiWindowFlags_NoBringToFrontOnFocus;
		wf |= ImGuiWindowFlags_UnsavedDocument;
		if (ImGui::Begin("Scene_MainMenu", nullptr, wf)) {
			if (ImGui::BeginMainMenuBar()) {
				if (ImGui::BeginMenu("File")) {
					if (ImGui::MenuItem("New", "Ctrl+N")) {
						OpenWindow(Windows::File_New);
					}
					if (ImGui::MenuItem("Open", "Ctrl+O")) {
						OpenWindow(Windows::File_Open);
					}
					ImGui::EndMenu();
				}
				ImGui::EndMainMenuBar();
			}
		}
		ImGui::End();

		// shortcut handlers
		if (io.KeyCtrl) {
			if (ImGui::IsKeyPressed(ImGuiKey_N, false)) {
				OpenWindow(Windows::File_New);
			}
			if (ImGui::IsKeyPressed(ImGuiKey_O, false)) {
				OpenWindow(Windows::File_Open);
			}
		}

		Draw_File_New();
		Draw_File_Open();
		// ...

#else
		static bool showDemo{ true};
		if (showDemo) {
			ImGui::ShowDemoWindow(&showDemo);
		}
#endif
	}

	void Scene::OpenWindow(Windows w) {
		if ((int)currentWindow) return;
		currentWindow = w;
	}

	void Scene::Draw_File_New() {
		if (currentWindow != Windows::File_New) return;

		auto& io = ImGui::GetIO();
		auto mvp = ImGui::GetMainViewport();
		// todo: calculate pos
		static constexpr float wndHeight = 100;
		static constexpr float lrMargin = 30;
		const auto tbMargin = (mvp->WorkSize.y - wndHeight) / 2;
		const auto wndWidth = mvp->WorkSize.x - lrMargin * 2;
		ImGui::SetNextWindowPos({ lrMargin, tbMargin }, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize({ wndWidth, wndHeight }, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags wf{};
		wf |= ImGuiWindowFlags_NoScrollbar;
		wf |= ImGuiWindowFlags_NoMove;
		wf |= ImGuiWindowFlags_NoResize;
		wf |= ImGuiWindowFlags_NoCollapse;
		wf |= ImGuiWindowFlags_NoNav;
		//wf |= ImGuiWindowFlags_NoBackground;
		//wf |= ImGuiWindowFlags_NoBringToFrontOnFocus;
		wf |= ImGuiWindowFlags_UnsavedDocument;
		if (ImGui::Begin("File_New", nullptr, wf)) {
			// todo: draw please input resmgr.sqlite3 dir:
		}
		ImGui::End();
	}

	void Scene::Draw_File_Open() {
		if (currentWindow != Windows::File_Open) return;
		xx::CoutN("File--Open");
		// todo
	}
}
