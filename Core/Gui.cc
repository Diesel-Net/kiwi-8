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

#ifdef __Linux__
    // Load Fonts. See extra_fonts/README.txt for more details)
    ImGuiIO& io = ImGui::GetIO();
    ImFontConfig config;
    config.MergeMode = true;

    // IMPROVE ME : replace the default value, and use the one in the preferences, just in case
    // we need to adapt (hdpi, or something around)
    io.Fonts->AddFontFromFileTTF("../fonts/DroidSans.ttf", 17.0f);

    // Application::SetImGuiStyle();
    // current style
    ImGuiStyle & style = ImGui::GetStyle();
    style.WindowRounding          = 3.0f;             // Radius of window corners rounding. Set to 0.0f to have rectangular windows
    style.ScrollbarRounding       = 3.0f;             // Radius of grab corners rounding for scrollbar
    style.GrabRounding            = 2.0f;             // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.

#ifndef BLUE_STYLE
    style.Colors[ImGuiCol_Text]                  = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    style.Colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg]                = ImVec4(0.93f, 0.93f, 0.93f, 0.98f);
    style.Colors[ImGuiCol_Border]                = ImVec4(0.71f, 0.71f, 0.71f, 0.08f);
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.04f);
    style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.71f, 0.71f, 0.71f, 0.55f);
    style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.94f, 0.94f, 0.94f, 0.55f);
    style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.71f, 0.78f, 0.69f, 0.98f);
    style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.82f, 0.78f, 0.78f, 0.51f);
    style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.20f, 0.25f, 0.30f, 0.61f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.90f, 0.90f, 0.90f, 0.30f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.92f, 0.92f, 0.92f, 0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_ComboBg]               = ImVec4(0.86f, 0.86f, 0.86f, 0.99f);
    style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.89f, 0.90f, 0.04f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Button]                = ImVec4(0.71f, 0.78f, 0.69f, 0.40f);
    style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.71f, 0.78f, 0.69f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.71f, 0.78f, 0.69f, 1.00f);
    style.Colors[ImGuiCol_Header]                = ImVec4(0.71f, 0.78f, 0.69f, 0.31f);
    style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.71f, 0.78f, 0.69f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.71f, 0.78f, 0.69f, 1.00f);
    style.Colors[ImGuiCol_Column]                = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    style.Colors[ImGuiCol_ColumnHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
    style.Colors[ImGuiCol_ColumnActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.45f);
    style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
    style.Colors[ImGuiCol_CloseButton]           = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
    style.Colors[ImGuiCol_CloseButtonActive]    = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_CloseButtonHovered]     = ImVec4(1.00f, 0.43f, 0.35f, 0.90f);
    style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    style.Colors[ImGuiCol_ModalWindowDarkening]  = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

    style.AntiAliasedLines = true;
    style.AntiAliasedShapes = true;
    style.WindowRounding = 2;
    style.ChildWindowRounding = 2;
    style.ScrollbarSize = 16;
    style.ScrollbarRounding = 3;
    style.GrabRounding = 2;
    style.ItemSpacing.x = 10;
    style.ItemSpacing.y = 4;
    style.IndentSpacing = 22;
    style.FramePadding.x = 6;
    style.FramePadding.y = 4;
    style.Alpha = 1.0f;
    style.FrameRounding = 3.0f;

#else
    style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f/*0.94f*/);
    style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f/*0.94*/);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    style.Colors[ImGuiCol_ComboBg] = ImVec4(0.86f, 0.86f, 0.86f, 0.99f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
    style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_CloseButton] = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
    style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);

#ifdef BLUE_DARK_STYLE
    for (int i = 0; i <= ImGuiCol_COUNT; i++) {
        ImVec4& col = style.Colors[i];
        float H, S, V;
        ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, H, S, V);

        if (S < 0.1f) {
        V = 1.0f - V;
        }

        ImGui::ColorConvertHSVtoRGB(H, S, V, col.x, col.y, col.z);

        if (col.w < 1.00f) {
        col.w *= 0.98f;
        }
    }

    style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.0f, 0.0f, 0.0f, 0.9f);
#endif /* BLUE_STYLE */
#endif /* BLUE_DARK_STYLE */
#endif /* Linux */
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
                ImGui::MenuItem("Show Menu", "Left-Alt", &show_menu_flag);
                ImGui::MenuItem("Show FPS", "Right-Alt", &show_fps_flag);

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
                ImGui::MenuItem("Mute Audio", "M", mute);
                ImGui::MenuItem("60 FPS Limit", NULL, &(display->limit_fps_flag));

                /* toggle Vsync is disabled for now because it doesn't really 
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
                    before = ( display->background_color[0] == ((float) DEFAULT_BACKGROUND_R / (float) 0xFF) &&
                               display->background_color[1] == ((float) DEFAULT_BACKGROUND_G / (float) 0xFF) &&
                               display->background_color[2] == ((float) DEFAULT_BACKGROUND_B / (float) 0xFF) &&
                               display->foreground_color[0] == ((float) DEFAULT_FOREGROUND_R / (float) 0xFF) &&
                               display->foreground_color[1] == ((float) DEFAULT_FOREGROUND_G / (float) 0xFF) &&
                               display->foreground_color[2] == ((float) DEFAULT_FOREGROUND_B / (float) 0xFF) );
                    ImGui::MenuItem("Default", NULL, &before);
                    if (before) {
                        display->background_color[0] = (float) DEFAULT_BACKGROUND_R / (float) 0xFF;
                        display->background_color[1] = (float) DEFAULT_BACKGROUND_G / (float) 0xFF;
                        display->background_color[2] = (float) DEFAULT_BACKGROUND_B / (float) 0xFF;
                        display->foreground_color[0] = (float) DEFAULT_FOREGROUND_R / (float) 0xFF;
                        display->foreground_color[1] = (float) DEFAULT_FOREGROUND_G / (float) 0xFF;
                        display->foreground_color[2] = (float) DEFAULT_FOREGROUND_B / (float) 0xFF;
                    }
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
        ImGui::SetNextWindowSize(ImVec2(345, 245), ImGuiSetCond_Appearing);
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
                            "soft reset                  f5\n"
                            "pause                       p\n"
                            "mute                        m");
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
