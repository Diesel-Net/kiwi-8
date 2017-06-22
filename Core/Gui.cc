#include "Chip8.h"
#include "Display.h"
#include "Gui.h"

Gui::Gui() {
    soft_reset_flag = 0;
    load_rom_flag = 0;
    quit_flag = 0;
    show_menu_flag = 1;
    show_fps_flag = 0;

    show_controls = 0;
    show_license = 0;
    show_about = 0;
    show_usage = 0; 
}

Gui::~Gui() {
    ImGui_ImplSdl_Shutdown();
}

void Gui::Initialize(Display *display, 
                     int *cycles,
                     bool *paused, 
                     bool *load_store_quirk, 
                     bool *shift_quirk, 
                     bool *vwrap,
                     bool *mute) {

    this->display = display;

    /* connect pointers to chip8 toggles */
    this->cycles = cycles;
    this->paused = paused;
    this->load_store_quirk = load_store_quirk;
    this->shift_quirk = shift_quirk;
    this->vwrap = vwrap;
    this->mute = mute;

    ImGui_ImplSdl_Init(display->window);

    /* disable imgui.ini file saving */
    ImGui::GetIO().IniFilename = NULL;
}

void Gui::ProcessEvents(SDL_Event *event) {
    ImGui_ImplSdl_ProcessEvent(event);
}

void Gui::NewFrame() {
    ImGui_ImplSdl_NewFrame(display->window);
    MainMenu();
}

void Gui::MainMenu() {
    bool before;

    if (show_menu_flag) {
        if (ImGui::BeginMainMenuBar()) {

            if (ImGui::BeginMenu("File")) {
                ImGui::MenuItem("Load ROM...", NULL, &load_rom_flag);
                ImGui::MenuItem("Exit", "Esc", &quit_flag);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View")) {
                ImGui::MenuItem("Show Menu", "L-Alt", &show_menu_flag);
                ImGui::MenuItem("Show FPS", "R-Alt", &show_fps_flag);

                /* fullscreen toggle */
                before = display->fullscreen_flag;
                ImGui::MenuItem("Fullscreen", "Enter", &(display->fullscreen_flag));
                if (before != display->fullscreen_flag) display->ToggleFullscreen();

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Emulation")) {
                ImGui::MenuItem("Reset", "F5", &soft_reset_flag);
                ImGui::MenuItem("Pause", "P", paused);
                
                /* CPU frequency */
                if (ImGui::BeginMenu("CPU Frequency")){
                    ImGui::MenuItem("", "PageDown/PageUp", !!0);
                    int cpu_frequency = *cycles * TICKS;
                    ImGui::SliderInt("Hz", &cpu_frequency, TICKS, TICKS * MAX_CYCLES_PER_STEP, "%.f");
                    *cycles = cpu_frequency / TICKS;
                    before = (*cycles == CYCLES_PER_STEP);
                    ImGui::MenuItem("Default", "720 Hz", &before);
                    if (before) *cycles = CYCLES_PER_STEP;
                    ImGui::EndMenu();
                }

                ImGui::MenuItem("Load/Store Quirk", NULL, load_store_quirk);
                ImGui::MenuItem("Shift Quirk", NULL, shift_quirk);
                ImGui::MenuItem("Vertical Wrapping", NULL, vwrap);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Settings")) {
                ImGui::MenuItem("Mute Audio", NULL, mute);
                ImGui::MenuItem("60 FPS Limit", NULL, &(display->limit_fps_flag));

                /* Toggle Vsync (disabled for now because it doesn't really 
                   make sense with the current design. It ends up slowing down 
                   emulation to whatever cpu speed puts out 60 frames a second 
                   which more often than not ends up being unbearably slow. A 
                   workaround I've found is to have both 60_fps_limit toggled on 
                   and vsync toggled on at the same time, but of course this 
                   will only work properly on 60hz monitors */

                /* before = display->vsync_flag;
                ImGui::MenuItem("Vsync", NULL, &(display->vsync_flag));
                if (before != display->vsync_flag) display->ToggleVsync(); */

                /* color chooser */
                if (ImGui::BeginMenu("Colors")) {
                    ImGui::ColorEdit3("Background", display->background_color);
                    ImGui::ColorEdit3("Foreground", display->foreground_color);
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help")) {
                ImGui::MenuItem("Usage", NULL, &show_usage);
                ImGui::MenuItem("Controls", NULL, &show_controls);
                ImGui::MenuItem("License", NULL, &show_license);
                ImGui::MenuItem("About", NULL, &show_about);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }
    HelpWindows();
}

void Gui::HelpWindows() {
    if (show_usage) {
        ImGui::SetNextWindowSize(ImVec2(270, 150), ImGuiSetCond_Appearing);
        ImGui::SetNextWindowPosCenter(ImGuiSetCond_Appearing);
        ImGui::Begin("Usage", &show_usage);
        ImGui::TextWrapped( "Alternatively, you may launch Kiwi8\n"
                            "from the command line.\n"
                            "\n"
                            "Usage: Kiwi8 [filename] [-FMLSV]\n"
                            "-F      Launch in fullscreen\n"
                            "-M      Launch with audio muted\n"
                            "-L      Disable load/store quirk\n"
                            "-S      Disable shift quirk\n"
                            "-V      Disable vertical wrapping");
        ImGui::End();
    }
    if (show_controls) {
        ImGui::SetNextWindowSize(ImVec2(345, 220), ImGuiSetCond_Appearing);
        ImGui::SetNextWindowPosCenter(ImGuiSetCond_Appearing);
        ImGui::Begin("Controls", &show_controls);
        ImGui::TextWrapped( "The Chip-8 uses a 16 digit hexadecimal keypad.\n"
                            "\n"
                            "controls:       --->        keybindings:\n"
                            "1 2 3 C                     1 2 3 4\n"
                            "4 5 6 D                     q w e r\n"
                            "7 8 9 E                     a s d f\n"
                            "A 0 B F                     z x c v\n"
                            "increase speed              page up\n"
                            "decrease speed              page down\n"
                            "quit                        esc\n"
                            "toggle fullscreen           enter\n"
                            "toggle menu                 left alt\n"
                            "show fps                    right alt\n"
                            "soft reset                  f5");
        ImGui::End();
    }
    if (show_license) {
        ImGui::SetNextWindowSize(ImVec2(500, 230), ImGuiSetCond_Appearing);
        ImGui::SetNextWindowPosCenter(ImGuiSetCond_Appearing);
        ImGui::Begin("License", &show_license);
        ImGui::TextWrapped( "Kiwi8 - \"A cross-platform Chip-8 interpreter\"\n"
                            "Copyright (C) 2016  Thomas Daley\n"
                            "\n"
                            "This program is free software: you can redistribute it and/or modify\n"
                            "it under the terms of the GNU General Public License as published by\n"
                            "the Free Software Foundation, either version 3 of the License, or\n"
                            "(at your option) any later version.\n"
                            "\n"
                            "This program is distributed in the hope that it will be useful,\n"
                            "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
                            "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
                            "GNU General Public License for more details.\n"
                            "\n"
                            "You should have received a copy of the GNU General Public License\n"
                            "along with this program.  If not, see <http://www.gnu.org/licenses/>.");
        ImGui::End();
    }
    if (show_about) {
        ImGui::SetNextWindowSize(ImVec2(330, 120), ImGuiSetCond_Appearing);
        ImGui::SetNextWindowPosCenter(ImGuiSetCond_Appearing);
        ImGui::Begin("About", &show_about);
        ImGui::TextWrapped( APPNAME_VERSION "\n"
                            "\n"
                            "A cross-platform Chip-8 interpreter written\n"
                            "in C++ using SDL2, ImGui, and OpenGL.\n"
                            "\n"
                            "<https://github.com/tomdaley92/Kiwi8>\n");
        ImGui::End();
    }
    if (show_fps_flag) {
        if (show_menu_flag) {
            ImGui::SetNextWindowPos(ImVec2(1, 21));
        } else {
            ImGui::SetNextWindowPos(ImVec2(1, 2));
        }
        if (!ImGui::Begin("FPS", &show_fps_flag, ImVec2(0, 0), 0.3f, ImGuiWindowFlags_NoTitleBar|
            ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings)) {
            ImGui::End();
            return;
        }
         ImGui::Text("%.1f FPS (%.3f ms/frame)", ImGui::GetIO().Framerate,
            1000.0f / ImGui::GetIO().Framerate );
        ImGui::End();
    }
}

void Gui::Render() {
    ImGui::Render();
}
