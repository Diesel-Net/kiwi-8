#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#include "profiles.h"
#include "crc32.h"
#include "chip8.h"
#include "toast.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

/* ROM profile hashmap: CRC32 -> profile */
static struct { uint32_t key; struct profile value; } *profile_map = NULL;

/* Track which path we loaded profiles.ini from */
static char loaded_profiles_path[512] = "";

/* Try to load profiles from file, create if not found */
static void profiles_load_from_file(void) {
    FILE *file = NULL;
    char *base_path = NULL;
    char search_paths[2][512];
    int i;

    /* If a custom path was already set via profiles_init(), use it directly */
    if (loaded_profiles_path[0] != '\0') {
        file = fopen(loaded_profiles_path, "r");
        if (file) {
            char notif_msg[256];
            snprintf(notif_msg, sizeof(notif_msg), "ROM profiles: %s", loaded_profiles_path);
            toast_show(TOAST_SUCCESS, notif_msg);
            fclose(file);
        } else {
            /* Custom path specified but file doesn't exist — create it */
            file = fopen(loaded_profiles_path, "w");
            if (file) {
                fprintf(file, "# ROM Profiles Database\n");
                fprintf(file, "# Format: [0xCRC32] followed by quirk settings\n\n");
                fclose(file);

                char notif_msg[256];
                snprintf(notif_msg, sizeof(notif_msg), "ROM profiles created: %s", loaded_profiles_path);
                toast_show(TOAST_INFO, notif_msg);
            } else {
                char notif_msg[256];
                snprintf(notif_msg, sizeof(notif_msg), "Failed to create: %s", loaded_profiles_path);
                toast_show(TOAST_ERROR, notif_msg);
                printf("Error: Unable to create profiles.ini at: %s\n", loaded_profiles_path);
            }
            return;
        }
        /* Fall through to INI parsing below */
        goto parse_ini;
    }

    /* Get executable's base path using SDL */
    base_path = SDL_GetBasePath();
    if (!base_path) {
        printf("Warning: Could not determine executable path. Trying current directory.\n");
        snprintf(search_paths[0], sizeof(search_paths[0]), "./profiles.ini");
        snprintf(search_paths[1], sizeof(search_paths[1]), "../Resources/profiles.ini");
    } else {
        /* Build search paths relative to executable */
        snprintf(search_paths[0], sizeof(search_paths[0]), "%sprofiles.ini", base_path);
        snprintf(search_paths[1], sizeof(search_paths[1]), "%s../Resources/profiles.ini", base_path);
        SDL_free(base_path);
    }

    /* Try each search path */
    for (i = 0; i < 2; i++) {
        file = fopen(search_paths[i], "r");
        if (file) {
            /* Save the path we successfully loaded from */
            strncpy(loaded_profiles_path, search_paths[i], sizeof(loaded_profiles_path) - 1);
            loaded_profiles_path[sizeof(loaded_profiles_path) - 1] = '\0';

            char notif_msg[256];
            snprintf(notif_msg, sizeof(notif_msg), "ROM profiles: %s", search_paths[i]);
            toast_show(TOAST_SUCCESS, notif_msg);

            fclose(file);
            break;
        }
    }

    /* If file not found, create empty one at first search path */
    if (!file) {
        file = fopen(search_paths[0], "w");
        if (file) {
            fprintf(file, "# ROM Profiles Database\n");
            fprintf(file, "# Format: [0xCRC32] followed by quirk settings\n\n");
            fclose(file);
            strncpy(loaded_profiles_path, search_paths[0], sizeof(loaded_profiles_path) - 1);
            loaded_profiles_path[sizeof(loaded_profiles_path) - 1] = '\0';

            char notif_msg[256];
            snprintf(notif_msg, sizeof(notif_msg), "ROM profiles created: %s", search_paths[0]);
            toast_show(TOAST_INFO, notif_msg);
        } else {
            toast_show(TOAST_ERROR, "Failed to create profiles.ini");
            printf("Error: Unable to create profiles.ini at: %s\n", search_paths[0]);
        }
        return;
    }

parse_ini:
    /* Parse INI file */
    char line[512];
    uint32_t current_crc = 0;
    struct profile current_profile;

    file = fopen(loaded_profiles_path, "r");
    if (!file) return;

    while (fgets(line, sizeof(line), file)) {
        /* Remove newline */
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        /* Skip empty lines and comments */
        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }

        /* Parse section header [0xHEXVALUE] */
        if (line[0] == '[') {
            if (current_crc != 0) {
                hmput(profile_map, current_crc, current_profile);
            }

            if (sscanf(line, "[0x%x]", &current_crc) != 1) {
                printf("Error: Invalid profile section header: %s\n", line);
                current_crc = 0;
                continue;
            }

            /* Initialize profile with defaults */
            memset(&current_profile, 0, sizeof(current_profile));
            current_profile.crc32 = current_crc;
            /* Set default quirk values */
            current_profile.quirks = quirks_get_defaults();
            continue;
        }

        /* Parse key=value pairs */
        if (current_crc != 0) {
            char key[256];
            int value;
            char str_value[256];

            /* Try parsing as integer first */
            if (sscanf(line, "%255[^=]=%d", key, &value) == 2) {
                /* Map key names to quirk fields */
                if (strcmp(key, "load_store_quirk") == 0) {
                    current_profile.quirks.load_store_quirk = value;
                } else if (strcmp(key, "shift_quirk") == 0) {
                    current_profile.quirks.shift_quirk = value;
                } else if (strcmp(key, "jump_quirk") == 0) {
                    current_profile.quirks.jump_quirk = value;
                } else if (strcmp(key, "logic_vf_quirk") == 0) {
                    current_profile.quirks.logic_vf_quirk = value;
                } else if (strcmp(key, "i_overflow_quirk") == 0) {
                    current_profile.quirks.i_overflow_quirk = value;
                } else if (strcmp(key, "draw_flag_quirk") == 0) {
                    current_profile.quirks.draw_flag_quirk = value;
                } else if (strcmp(key, "vwrap") == 0) {
                    current_profile.quirks.vwrap = value;
                } else if (strcmp(key, "hwrap") == 0) {
                    current_profile.quirks.hwrap = value;
                }
            }
            /* Try parsing as string for rom_name */
            else if (sscanf(line, "%255[^=]=%255[^\n]", key, str_value) == 2) {
                if (strcmp(key, "name") == 0) {
                    strncpy(current_profile.rom_name, str_value, sizeof(current_profile.rom_name) - 1);
                    current_profile.rom_name[sizeof(current_profile.rom_name) - 1] = '\0';
                }
            }
        }
    }

    /* Don't forget the last entry */
    if (current_crc != 0) {
        hmput(profile_map, current_crc, current_profile);
    }

    fclose(file);
}

void profiles_init(const char *custom_path) {
    if (profile_map == NULL) {
        if (custom_path && custom_path[0] != '\0') {
            strncpy(loaded_profiles_path, custom_path, sizeof(loaded_profiles_path) - 1);
            loaded_profiles_path[sizeof(loaded_profiles_path) - 1] = '\0';
        }
        profiles_load_from_file();
    }
}

const struct profile* profile_lookup(uint32_t crc32) {
    ptrdiff_t idx;
    idx = hmgeti(profile_map, crc32);
    if (idx < 0) {
        return NULL;
    }
    return &profile_map[idx].value;
}

/* Write entire profile hashmap to INI file */
static void profiles_write_to_file(void) {
    FILE *file = NULL;
    int i;

    if (loaded_profiles_path[0] == '\0') {
        printf("Error: profiles.ini path not initialized\n");
        return;
    }

    file = fopen(loaded_profiles_path, "w");
    if (!file) {
        printf("Error: Unable to write to %s\n", loaded_profiles_path);
        return;
    }

    /* Write header */
    fprintf(file, "# ROM Profiles Database\n");
    fprintf(file, "# Format: [0xCRC32] followed by quirk settings\n\n");

    /* Write all profiles from hashmap */
    for (i = 0; i < hmlen(profile_map); i++) {
        struct profile *p = &profile_map[i].value;

        fprintf(file, "[0x%X]\n", p->crc32);
        if (p->rom_name[0] != '\0') {
            fprintf(file, "name=%s\n", p->rom_name);
        }
        fprintf(file, "load_store_quirk=%d\n", p->quirks.load_store_quirk);
        fprintf(file, "shift_quirk=%d\n", p->quirks.shift_quirk);
        fprintf(file, "jump_quirk=%d\n", p->quirks.jump_quirk);
        fprintf(file, "logic_vf_quirk=%d\n", p->quirks.logic_vf_quirk);
        fprintf(file, "i_overflow_quirk=%d\n", p->quirks.i_overflow_quirk);
        fprintf(file, "draw_flag_quirk=%d\n", p->quirks.draw_flag_quirk);
        fprintf(file, "vwrap=%d\n", p->quirks.vwrap);
        fprintf(file, "hwrap=%d\n\n", p->quirks.hwrap);
    }

    fclose(file);
}

void profiles_save_current(void) {
    uint32_t crc32;
    struct profile p;

    /* Check if ROM is actually loaded */
    if (!chip8.rom_loaded || !chip8.rom || chip8.rom_size == 0) {
        toast_show(TOAST_ERROR, "No ROM loaded. Cannot save profile.");
        return;
    }

    /* Check if we have a valid profiles path */
    if (loaded_profiles_path[0] == '\0') {
        toast_show(TOAST_ERROR, "Could not locate profiles.ini");
        return;
    }

    /* Compute CRC32 */
    crc32 = crc32_compute(chip8.rom, chip8.rom_size);

    /* Initialize profile struct */
    memset(&p, 0, sizeof(p));
    p.crc32 = crc32;
    strncpy(p.rom_name, chip8.rom_filename, sizeof(p.rom_name) - 1);
    p.rom_name[sizeof(p.rom_name) - 1] = '\0';

    /* Copy current quirks */
    p.quirks = chip8.quirks;

    /* Update runtime hashmap */
    hmput(profile_map, crc32, p);

    /* Write entire hashmap to INI file */
    profiles_write_to_file();

    printf("Saved ROM profile for: %s (CRC32: 0x%X)\n",
           chip8.rom_filename, crc32);

    /* Show success notification with path */
    char notif_msg[256];
    snprintf(notif_msg, sizeof(notif_msg), "Profile saved: %s", chip8.rom_filename);
    toast_show(TOAST_SUCCESS, notif_msg);
}
