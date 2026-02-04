#include "chip8.h"
#include "display.h"
#include "gui.h"
#include "license.h" // Generated at build time from LICENSE
#include "notifications.h"
#include "usage.h"
#include <stdio.h>

static void gui_help_windows(void) {
    if (gui.show_usage) {
        ImGui::SetNextWindowSize(ImVec2(350, 210), ImGuiSetCond_Appearing);
        ImGui::SetNextWindowPosCenter(ImGuiSetCond_Appearing);
        ImGui::Begin("Usage", &gui.show_usage);
        ImGui::TextWrapped("%s", USAGE_TEXT);
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
        ImGui::SetNextWindowSize(ImVec2(550, 320), ImGuiSetCond_Appearing);
        ImGui::SetNextWindowPosCenter(ImGuiSetCond_Appearing);
        ImGui::Begin("License", &gui.show_license);

        ImGui::TextWrapped("%s", LICENSE_TEXT);

        ImGui::End();
    }
    if (gui.show_about) {
        ImGui::SetNextWindowSize(ImVec2(350, 140), ImGuiSetCond_Appearing);
        ImGui::SetNextWindowPosCenter(ImGuiSetCond_Appearing);
        ImGui::Begin("About", &gui.show_about);

        ImGui::TextWrapped(
            APP_NAME " " VERSION " (" SUB_VERSION ")\n"
            BUILD_OS " " BUILD_ARCH "\n"
            "\n"
            "A cross-platform Chip-8 interpreter written\n"
            "in C-Style C++ using SDL2, ImGui, and OpenGL.\n"
            "\n"
            "<https://github.com/Diesel-Net/kiwi-8>\n"
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

static void gui_notifications(void) {
    int count;
    const struct notification *notifications = notify_get_notifications(&count);

    if (count == 0) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    float spacing = 10.0f;
    float y_offset = io.DisplaySize.y - spacing;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));

    for (int i = 0; i < 8; i++) {
        if (!notify_is_active(&notifications[i])) {
            continue;
        }

        const char *message;
        int type;
        double time_remaining;
        notify_get_info(&notifications[i], &message, &type, &time_remaining);

        /* Calculate alpha for fade out effect */
        float alpha = 1.0f;
        if (time_remaining < 0.5) {
            alpha = (float)(time_remaining / 0.5);
        }

        /* Set colors based on notification type */
        ImVec4 bg_color;
        if (type == NOTIFY_SUCCESS) {
            bg_color = ImVec4(0.1f, 0.4f, 0.1f, 0.85f * alpha);
        } else if (type == NOTIFY_ERROR) {
            bg_color = ImVec4(0.5f, 0.1f, 0.1f, 0.85f * alpha);
        } else {
            bg_color = ImVec4(0.25f, 0.25f, 0.25f, 0.85f * alpha);
        }

        ImGui::PushStyleColor(ImGuiCol_WindowBg, bg_color);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, alpha));

        /* Begin window to calculate size */
        char window_name[32];
        snprintf(window_name, sizeof(window_name), "##notification%d", i);

        /* Set max width for notifications */
        float max_width = 300.0f;
        ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(max_width, FLT_MAX));

        ImGui::Begin(window_name, NULL,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_AlwaysAutoResize);

        /* Wrap text to max width */
        ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + max_width - 20);
        ImGui::TextWrapped("%s", message);
        ImGui::PopTextWrapPos();

        /* Get actual window size and reposition from bottom-right */
        ImVec2 window_size = ImGui::GetWindowSize();
        ImVec2 window_pos = ImVec2(io.DisplaySize.x - window_size.x - spacing, y_offset - window_size.y);
        ImGui::SetWindowPos(window_pos);

        y_offset -= window_size.y + spacing;

        ImGui::End();

        ImGui::PopStyleColor(2);
    }

    ImGui::PopStyleVar(2);
}

static void gui_main_menu(void) {
    bool before;

    if (gui.show_menu_flag) {
        if (ImGui::BeginMainMenuBar()) {

            if (ImGui::BeginMenu("File")) {
                ImGui::MenuItem("Load ROM...", NULL, &gui.load_rom_flag);
                if (ImGui::MenuItem("Save ROM Profile", NULL, false, chip8.rom_loaded)) {
                    gui.save_profile_flag = 1;
                }
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
                ImGui::MenuItem("Pause", "P", &chip8.paused);

                /* CPU frequency */
                if (ImGui::BeginMenu("CPU Frequency")){
                    ImGui::MenuItem("", "PageDown/PageUp", !!0);
                    int cpu_frequency = chip8.cycles * TICKS;
                    ImGui::SliderInt("Hz", &cpu_frequency, TICKS, TICKS * MAX_CYCLES_PER_STEP, "%.f");
                    chip8.cycles = cpu_frequency / TICKS;
                    before = (chip8.cycles == CYCLES_PER_STEP);
                    ImGui::MenuItem("Default", "720 Hz", &before);
                    if (before) chip8.cycles = CYCLES_PER_STEP;
                    ImGui::EndMenu();
                }

                ImGui::MenuItem("Load/Store Quirk", NULL, &chip8.quirks.load_store_quirk);
                ImGui::MenuItem("Shift Quirk", NULL, &chip8.quirks.shift_quirk);
                ImGui::MenuItem("Vertical Wrapping", NULL, &chip8.quirks.vwrap);
                ImGui::MenuItem("Horizontal Wrapping", NULL, &chip8.quirks.hwrap);
                ImGui::MenuItem("Jump with VX Offset (BNNN)", NULL, &chip8.quirks.jump_quirk);
                ImGui::MenuItem("Logic Ops VF=0 (8XY1/2/3)", NULL, &chip8.quirks.logic_vf_quirk);
                ImGui::MenuItem("I+VX Overflow Quirk (FX1E)", NULL, &chip8.quirks.i_overflow_quirk);
                ImGui::MenuItem("Draw Flag Reset Quirk", NULL, &chip8.quirks.draw_flag_quirk);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Settings")) {
                ImGui::MenuItem("Mute Audio", "M", &chip8.muted);
                ImGui::MenuItem("60 FPS Limit", NULL, &display.limit_fps_flag);

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

/* Global gui instance */
struct gui gui;

void gui_cleanup(void) {
    ImGui_ImplSdl_Shutdown();
}

void gui_init(void) {
    gui.soft_reset_flag = 0;
    gui.load_rom_flag = 0;
    gui.save_profile_flag = 0;
    gui.quit_flag = 0;
    gui.show_menu_flag = 1;
    gui.show_fps_flag = 0;

    gui.show_controls = 0;
    gui.show_license = 0;
    gui.show_about = 0;
    gui.show_usage = 0;

    ImGui_ImplSdl_Init(display.window);

    /* disable imgui.ini file saving */
    ImGui::GetIO().IniFilename = NULL;
}

void gui_process_events(SDL_Event *event) {
    ImGui_ImplSdl_ProcessEvent(event);
}

void gui_new_frame(void) {
    ImGui_ImplSdl_NewFrame(display.window);
    gui_main_menu();
    gui_notifications();
}

void gui_render(void) {
    ImGui::Render();
}
