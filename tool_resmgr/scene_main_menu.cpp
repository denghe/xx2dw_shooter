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
		Draw_Error();
		lastWindow = currentWindow;

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
		auto isFirst = lastWindow != currentWindow;

		auto& io = ImGui::GetIO();
		auto mvp = ImGui::GetMainViewport();
		static constexpr float wndHeight = 170;
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
		wf |= ImGuiWindowFlags_UnsavedDocument;
		if (ImGui::Begin("new db", nullptr, wf)) {
			ImGui::Text("please input the db file name:");
			ImGui::SetNextItemWidth(-FLT_MIN);
			if (isFirst) {
				ImGui::SetKeyboardFocusHere(0);
			}
			ImGui::InputText("##dbFileName", &dbFileName);
			ImGui::Dummy({ 0.0f, 5.0f });
			ImGui::Separator();
			ImGui::Dummy({ 0.0f, 5.0f });

			ImGui::Dummy({ 0.0f, 0.0f });
			ImGui::SameLine(ImGui::GetWindowWidth() - (ImGui::GetStyle().ItemSpacing.x + 120 + 10 + 120));
			if (ImGui::Button("Submit", { 120, 35 })) {
				// todo: check
			}
			ImGui::SameLine(ImGui::GetWindowWidth() - (ImGui::GetStyle().ItemSpacing.x + 120));
			if (ImGui::Button("Cancel", { 120, 35 })) {
				currentWindow = {};
			}
		}
		ImGui::End();
	}

	void Scene::Draw_File_Open() {
		if (currentWindow != Windows::File_Open) return;
		auto isFirst = lastWindow != currentWindow;
		// todo
		currentWindow = {};
		errorMessage = "File--Open todo";
	}

	void Scene::Draw_Error() {
		if (errorMessage.empty()) return;

		auto& io = ImGui::GetIO();
		auto mvp = ImGui::GetMainViewport();
		static constexpr float wndHeight = 300;
		static constexpr float lrMargin = 100;
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
		wf |= ImGuiWindowFlags_UnsavedDocument;
		if (ImGui::Begin("error message:", nullptr, wf)) {
			ImGui::InputTextMultiline("##errorMessage", &errorMessage, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 9), ImGuiInputTextFlags_ReadOnly);
			ImGui::Dummy({ 0.0f, 0.0f });
			ImGui::SameLine(ImGui::GetWindowWidth() - (ImGui::GetStyle().ItemSpacing.x + 90));
			if (ImGui::Button("OK", { 90, 35 })) {
				errorMessage.clear();
			}
		}
		ImGui::End();
	}
}
