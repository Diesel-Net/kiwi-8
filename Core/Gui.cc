#include "Display.h"
#include "Gui.h"

Gui::Gui() {

}

Gui::~Gui() {
    ImGui_ImplSdl_Shutdown();
}

void Gui::Initialize(Display *display, 
						bool *emulation_paused, 
                        bool *load_store_quirk, 
                        bool *shift_quirk, 
                        bool *vwrap) {

	this->display = display;

	/* connect pointers to chip8 toggles */
	this->emulation_paused = emulation_paused;
    this->load_store_quirk = load_store_quirk;
	this->shift_quirk = shift_quirk;
	this->vwrap = vwrap;

	soft_reset_flag = 0;
	load_rom_flag = 0;
	quit_flag = 0;
	show_menu_flag = 1;

	

	ImGui_ImplSdl_Init(display->window);

	/* Disable imgui.ini file saving */
    ImGui::GetIO().IniFilename = NULL;
}

void Gui::ProcessEvents(SDL_Event* event) {
	ImGui_ImplSdl_ProcessEvent(event);
}

void Gui::NewFrame() {
	ImGui_ImplSdl_NewFrame(display->window);
	MainMenu();
}

void Gui::MainMenu() {

	/* Show framerate (SDL's framerate, NOT the emulator's framerate) */
	//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	bool before;

    if (show_menu_flag) {
		if (ImGui::BeginMainMenuBar()) {

			if (ImGui::BeginMenu("File")) {
				ImGui::MenuItem("Load ROM...", NULL, !!0); // TO COMPLETE
				ImGui::MenuItem("Exit", "Esc", &quit_flag);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View")) {
				ImGui::MenuItem("Show Menu", "Alt", &show_menu_flag);

				/* Fullscreen Toggle */
	            before = display->fullscreen_flag;
	            ImGui::MenuItem("Fullscreen", "Enter", &(display->fullscreen_flag));
	            if (before != display->fullscreen_flag) {
	            	display->ToggleFullscreen();
	            }

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Emulation")) {
				ImGui::MenuItem("Reset", "F5", &soft_reset_flag); // TO COMPLETE
				ImGui::MenuItem("Pause", "P", emulation_paused);
				
				/* CPU Frequency Selector - TO COMPLETE */
				//if (ImGui::BeginMenu("CPU Frequency")) {
                	//ImGui::SliderInt("Hz", &chip8_cpu_freq, 60, 5000, "%.0f");
                	//ImGui::MenuItem("Default (840Hz)", NULL, &flag_default_freq);
                	//ImGui::EndMenu();
            	//}

				ImGui::MenuItem("Load/Store Quirk", NULL, load_store_quirk);
				ImGui::MenuItem("Shift Quirk", NULL, shift_quirk);
				ImGui::MenuItem("Vertical Wrapping", NULL, vwrap);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Settings")) {

	            /* Toggle Vsync */
	            before = display->vsync_flag;
	            ImGui::MenuItem("Vsync", NULL, &(display->vsync_flag));
	            if (before != display->vsync_flag) {
	            	display->ToggleVsync();
	            }

	            if (ImGui::BeginMenu("Color")) {
	                ImGui::ColorEdit3("Background", display->background_color);
	                ImGui::ColorEdit3("Foreground", display->foreground_color);
	                ImGui::EndMenu();
	            }

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help")) {
				ImGui::MenuItem("Controls", NULL, !!0); // TO COMPLETE
				ImGui::MenuItem("License", NULL, !!0); // TO COMPLETE
				ImGui::MenuItem("About", NULL, !!0); // TO COMPLETE
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
}

void Gui::Render() {
	ImGui::Render();
}
