#include "chip8.h"
#include "display.h"
#include "gui.h"
#include "license.h" // Generated at build time from LICENSE
#include <stdio.h>

/* Forward declarations for static helpers */
static void gui_main_menu(void);
static void gui_help_windows(void);

/* Global gui instance */
struct gui gui;

void gui_create(void) {
    gui.soft_reset_flag = 0;
    gui.load_rom_flag = 0;
    gui.quit_flag = 0;
    gui.show_menu_flag = 1;
    gui.show_fps_flag = 0;

    gui.show_controls = 0;
    gui.show_license = 0;
    gui.show_about = 0;
    gui.show_usage = 0;
}

void gui_cleanup(void) {
    ImGui_ImplSdl_Shutdown();
}

void gui_initialize(
    struct display *display,
    int *cycles,
    bool *paused,
    bool *load_store_quirk,
    bool *shift_quirk,
    bool *vwrap,
    bool *mute
) {

    gui.display = display;

    /* connect pointers to chip8 toggles */
    gui.cycles = cycles;
    gui.paused = paused;
    gui.load_store_quirk = load_store_quirk;
    gui.shift_quirk = shift_quirk;
    gui.vwrap = vwrap;
    gui.mute = mute;

    ImGui_ImplSdl_Init(display->window);

    /* disable imgui.ini file saving */
    ImGui::GetIO().IniFilename = NULL;
}

void gui_process_events(SDL_Event *event) {
    ImGui_ImplSdl_ProcessEvent(event);
}

void gui_new_frame(void) {
    ImGui_ImplSdl_NewFrame(display.window);
    gui_main_menu();
}

static void gui_main_menu(void) {
    bool before;

    if (gui.show_menu_flag) {
        if (ImGui::BeginMainMenuBar()) {

            if (ImGui::BeginMenu("File")) {
                ImGui::MenuItem("Load ROM...", NULL, &gui.load_rom_flag);
                ImGui::MenuItem("Exit", "Esc", &gui.quit_flag);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View")) {
                ImGui::MenuItem("Show Menu", "Left-Alt", &gui.show_menu_flag);
                ImGui::MenuItem("Show FPS", "Right-Alt", &gui.show_fps_flag);

                /* fullscreen toggle */
                before = display.fullscreen_flag;
                ImGui::MenuItem("Fullscreen", "Enter", &(display.fullscreen_flag));
                if (before != display.fullscreen_flag) display_toggle_fullscreen();

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Emulation")) {
                ImGui::MenuItem("Reset", "F5", &gui.soft_reset_flag);
                ImGui::MenuItem("Pause", "P", gui.paused);

                /* CPU frequency */
                if (ImGui::BeginMenu("CPU Frequency")){
                    ImGui::MenuItem("", "PageDown/PageUp", !!0);
                    int cpu_frequency = *(gui.cycles) * TICKS;
                    ImGui::SliderInt("Hz", &cpu_frequency, TICKS, TICKS * MAX_CYCLES_PER_STEP, "%.f");
                    *(gui.cycles) = cpu_frequency / TICKS;
                    before = (*(gui.cycles) == CYCLES_PER_STEP);
                    ImGui::MenuItem("Default", "720 Hz", &before);
                    if (before) *(gui.cycles) = CYCLES_PER_STEP;
                    ImGui::EndMenu();
                }

                ImGui::MenuItem("Load/Store Quirk", NULL, gui.load_store_quirk);
                ImGui::MenuItem("Shift Quirk", NULL, gui.shift_quirk);
                ImGui::MenuItem("Vertical Wrapping", NULL, gui.vwrap);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Settings")) {
                ImGui::MenuItem("Mute Audio", "M", gui.mute);
                ImGui::MenuItem("60 FPS Limit", NULL, &(display.limit_fps_flag));

                /*
                toggle Vsync is disabled for now because it doesn't really
                make sense with the current design. It ends up slowing down
                emulation to whatever cpu speed puts out 60 frames a second
                which more often than not ends up being unbearably slow. A
                workaround I've found is to have both 60_fps_limit toggled on
                and vsync toggled on at the same time, but of course this
                will only work properly on 60hz monitors
                */

                /*
                before = display->vsync_flag;
                ImGui::MenuItem("Vsync", NULL, &(display->vsync_flag));
                if (before != display->vsync_flag) display->ToggleVsync();
                */

                /* color chooser */
                if (ImGui::BeginMenu("Colors")) {
                    ImGui::ColorEdit3("Background", display.background_color);
                    ImGui::ColorEdit3("Foreground", display.foreground_color);
                    before = (
                        display.background_color[0] == ((float) DEFAULT_BACKGROUND_R / (float) 0xFF) &&
                        display.background_color[1] == ((float) DEFAULT_BACKGROUND_G / (float) 0xFF) &&
                        display.background_color[2] == ((float) DEFAULT_BACKGROUND_B / (float) 0xFF) &&
                        display.foreground_color[0] == ((float) DEFAULT_FOREGROUND_R / (float) 0xFF) &&
                        display.foreground_color[1] == ((float) DEFAULT_FOREGROUND_G / (float) 0xFF) &&
                        display.foreground_color[2] == ((float) DEFAULT_FOREGROUND_B / (float) 0xFF)
                    );
                    ImGui::MenuItem("Default", NULL, &before);
                    if (before) {
                        display.background_color[0] = (float) DEFAULT_BACKGROUND_R / (float) 0xFF;
                        display.background_color[1] = (float) DEFAULT_BACKGROUND_G / (float) 0xFF;
                        display.background_color[2] = (float) DEFAULT_BACKGROUND_B / (float) 0xFF;
                        display.foreground_color[0] = (float) DEFAULT_FOREGROUND_R / (float) 0xFF;
                        display.foreground_color[1] = (float) DEFAULT_FOREGROUND_G / (float) 0xFF;
                        display.foreground_color[2] = (float) DEFAULT_FOREGROUND_B / (float) 0xFF;
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help")) {
                ImGui::MenuItem("Usage", NULL, &gui.show_usage);
                ImGui::MenuItem("Controls", NULL, &gui.show_controls);
                ImGui::MenuItem("License", NULL, &gui.show_license);
                ImGui::MenuItem("About", NULL, &gui.show_about);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }
    gui_help_windows();
}

static void gui_help_windows(void) {
    if (gui.show_usage) {
        ImGui::SetNextWindowSize(ImVec2(270, 150), ImGuiSetCond_Appearing);
        ImGui::SetNextWindowPosCenter(ImGuiSetCond_Appearing);
        ImGui::Begin("Usage", &gui.show_usage);

        ImGui::TextWrapped(
            "Alternatively, you may launch Kiwi8\n"
            "from the command line.\n"
            "\n"
            "Usage: Kiwi8 [filename] [-FMLSV]\n"
            "-F      Launch in fullscreen\n"
            "-M      Launch with audio muted\n"
            "-L      Disable load/store quirk\n"
            "-S      Disable shift quirk\n"
            "-V      Disable vertical wrapping"
        );

        ImGui::End();
    }
    if (gui.show_controls) {
        ImGui::SetNextWindowSize(ImVec2(345, 245), ImGuiSetCond_Appearing);
        ImGui::SetNextWindowPosCenter(ImGuiSetCond_Appearing);
        ImGui::Begin("Controls", &gui.show_controls);

        ImGui::TextWrapped(
            "The Chip-8 uses a 16 digit hexadecimal keypad.\n"
            "\n"
            "controls:       <-->        keybindings:\n"
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
            "mute                        m"
        );

        ImGui::End();
    }
    if (gui.show_license) {
        ImGui::SetNextWindowSize(ImVec2(550, 245), ImGuiSetCond_Appearing);
        ImGui::SetNextWindowPosCenter(ImGuiSetCond_Appearing);
        ImGui::Begin("License", &gui.show_license);

        ImGui::TextWrapped("%s", LICENSE_TEXT);

        ImGui::End();
    }
    if (gui.show_about) {
        ImGui::SetNextWindowSize(ImVec2(350, 140), ImGuiSetCond_Appearing);
        ImGui::SetNextWindowPosCenter(ImGuiSetCond_Appearing);
        ImGui::Begin("About", &gui.show_about);

        // Truncate COMMIT_HASH to first 7 characters for display
        char short_hash[8];
        snprintf(short_hash, sizeof(short_hash), "%.7s", COMMIT_HASH);

        ImGui::TextWrapped(
            APP_NAME " " VERSION " (%s)\n"
            BUILD_OS " " BUILD_ARCH "\n"
            "\n"
            "A cross-platform Chip-8 interpreter written\n"
            "in C-Style C++ using SDL2, ImGui, and OpenGL.\n"
            "\n"
            "<https://github.com/Diesel-Net/kiwi-8>\n",
            short_hash
        );

        ImGui::End();
    }
    if (gui.show_fps_flag) {
        if (gui.show_menu_flag) {
            ImGui::SetNextWindowPos(ImVec2(1, 21));
        } else {
            ImGui::SetNextWindowPos(ImVec2(1, 2));
        }
        if (!ImGui::Begin(
                "FPS",
                &gui.show_fps_flag,
                ImVec2(0, 0),
                0.3f,
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoSavedSettings
            )
        ) {
            ImGui::End();
            return;
        }

        ImGui::Text(
            "%.1f FPS (%.3f ms/frame)",
            ImGui::GetIO().Framerate,
            1000.0f / ImGui::GetIO().Framerate
        );

        ImGui::End();
    }
}

void gui_render(void) {
    ImGui::Render();
}
