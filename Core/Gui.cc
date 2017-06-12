#include "Gui.h"

Gui::Gui() {

}

Gui::~Gui() {
    ImGui_ImplSdl_Shutdown();
}

void Gui::Initialize(SDL_Window *window, float *background_color, float *foreground_color) {
	this->background_color = background_color;
	this->foreground_color = foreground_color;
	ImGui_ImplSdl_Init(window);

	/* Disable imgui.ini file saving */
    ImGui::GetIO().IniFilename = NULL;
}

void Gui::ProcessEvents(SDL_Event* event) {
	ImGui_ImplSdl_ProcessEvent(event);
}

void Gui::NewFrame(SDL_Window *window) {
	ImGui_ImplSdl_NewFrame(window);
}

void Gui::ProcessMenu() {
    if (show_menu_flag) {
		if (ImGui::BeginMainMenuBar()) {

			if (ImGui::BeginMenu("File")) {
				ImGui::MenuItem("Load ROM...", NULL, !!0);
				ImGui::MenuItem("Exit", "Esc", &quit_flag);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View")) {
				ImGui::MenuItem("Show Menu", "Alt", &show_menu_flag);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Emulation")) {
				ImGui::MenuItem("Load/Store Quirk", NULL, !!1);
				ImGui::MenuItem("Shift Quirk", NULL, !!1);
				ImGui::MenuItem("Vertical Wrapping", NULL, !!1);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Settings")) {

				if (ImGui::BeginMenu("Color")) {
	                ImGui::ColorEdit3("Background", background_color);
	                ImGui::ColorEdit3("Foreground", foreground_color);
	                ImGui::EndMenu();
	            }
	            ImGui::MenuItem("Fullscreen", NULL, !!1);
	            ImGui::MenuItem("Vsync", NULL, !!1);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help")) {
				ImGui::MenuItem("About", NULL, !!0);
				ImGui::MenuItem("License", NULL, !!0);
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
}

void Gui::Render() {
	ImGui::Render();
}
