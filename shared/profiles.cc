#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#include "profiles.h"
#include "crc32.h"
#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <limits.h>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#elif defined(__linux__)
#include <unistd.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

/* Get the directory containing the executable */
static void get_executable_dir(char *buf, size_t size) {
    char exe_path[PATH_MAX];

#ifdef __APPLE__
    uint32_t bufsize = sizeof(exe_path);
    if (_NSGetExecutablePath(exe_path, &bufsize) == 0) {
        /* Find last slash to get directory */
        char *last_slash = strrchr(exe_path, '/');
        if (last_slash) {
            size_t dir_len = last_slash - exe_path;
            if (dir_len < size) {
                strncpy(buf, exe_path, dir_len);
                buf[dir_len] = '\0';
                return;
            }
        }
    }
#elif defined(__linux__)
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len != -1) {
        exe_path[len] = '\0';
        char *last_slash = strrchr(exe_path, '/');
        if (last_slash) {
            size_t dir_len = last_slash - exe_path;
            if (dir_len < size) {
                strncpy(buf, exe_path, dir_len);
                buf[dir_len] = '\0';
                return;
            }
        }
    }
#elif defined(_WIN32)
    if (GetModuleFileNameA(NULL, exe_path, sizeof(exe_path))) {
        char *last_slash = strrchr(exe_path, '\\');
        if (last_slash) {
            size_t dir_len = last_slash - exe_path;
            if (dir_len < size) {
                strncpy(buf, exe_path, dir_len);
                buf[dir_len] = '\0';
                return;
            }
        }
    }
#endif

    /* Fallback to current directory */
    buf[0] = '.';
    buf[1] = '\0';
}

/* ROM profile hashmap: CRC32 -> profile */
static struct { uint32_t key; struct profile value; } *profile_map = NULL;

/* Try to load profiles from file */
static void profiles_load_from_file(void) {
    FILE *file = NULL;
    char exe_dir[PATH_MAX];
    char search_paths[3][PATH_MAX];
    int i;

    get_executable_dir(exe_dir, sizeof(exe_dir));

    /* Build search paths relative to executable */
    snprintf(search_paths[0], PATH_MAX, "%s/profiles.ini", exe_dir);
    snprintf(search_paths[1], PATH_MAX, "%s/../Resources/profiles.ini", exe_dir);
    snprintf(search_paths[2], PATH_MAX, "%s/../../roms/profiles.ini", exe_dir);

    /* Try each search path */
    for (i = 0; i < 3; i++) {
        file = fopen(search_paths[i], "r");
        if (file) {
            printf("Loaded ROM profiles from: %s\n", search_paths[i]);
            break;
        }
    }

    if (!file) {
        printf("Warning: No profiles.ini found. Using hardcoded defaults.\n");
        return;
    }

    /* Parse INI file */
    char line[512];
    uint32_t current_crc = 0;
    struct profile current_profile;

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
                fprintf(stderr, "Error: Invalid profile section header: %s\n", line);
                current_crc = 0;
                continue;
            }

            /* Initialize profile with defaults */
            memset(&current_profile, 0, sizeof(current_profile));
            current_profile.crc32 = current_crc;
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

void profiles_init(void) {
    if (profile_map == NULL) {
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

/* Write a profile to the INI file
 * Reads existing INI, updates or appends entry, writes back */
static void profiles_write_to_file(uint32_t crc32, const struct profile *p) {
    FILE *file = NULL;
    const char *filepath = "./profiles.ini";

    /* Try to open for reading first */
    file = fopen(filepath, "r");

    if (file) {
        /* File exists: read, update, rewrite */
        FILE *temp_file = NULL;
        char line[512];
        char temp_filepath[512];
        int found = 0;
        uint32_t current_crc = 0;

        snprintf(temp_filepath, sizeof(temp_filepath), "%s.tmp", filepath);
        temp_file = fopen(temp_filepath, "w");

        if (!temp_file) {
            fprintf(stderr, "Error: Unable to write profile to %s\n", filepath);
            fclose(file);
            return;
        }

        /* Copy existing file, updating matching entry */
        while (fgets(line, sizeof(line), file)) {
            if (line[0] == '[' && sscanf(line, "[0x%x]", &current_crc) == 1) {
                if (current_crc == crc32) {
                    /* This is our entry: write updated version */
                    found = 1;
                    fprintf(temp_file, "[0x%X]\n", crc32);
                    if (p->rom_name[0] != '\0') {
                        fprintf(temp_file, "name=%s\n", p->rom_name);
                    }
                    fprintf(temp_file, "load_store_quirk=%d\n", p->quirks.load_store_quirk);
                    fprintf(temp_file, "shift_quirk=%d\n", p->quirks.shift_quirk);
                    fprintf(temp_file, "jump_quirk=%d\n", p->quirks.jump_quirk);
                    fprintf(temp_file, "logic_vf_quirk=%d\n", p->quirks.logic_vf_quirk);
                    fprintf(temp_file, "i_overflow_quirk=%d\n", p->quirks.i_overflow_quirk);
                    fprintf(temp_file, "draw_flag_quirk=%d\n", p->quirks.draw_flag_quirk);
                    fprintf(temp_file, "vwrap=%d\n", p->quirks.vwrap);
                    fprintf(temp_file, "hwrap=%d\n\n", p->quirks.hwrap);

                    /* Skip old entries for this CRC */
                    while (fgets(line, sizeof(line), file)) {
                        if (line[0] == '[') {
                            fputs(line, temp_file);
                            break;
                        }
                    }
                    continue;
                }
            }

            fputs(line, temp_file);
        }

        /* If entry wasn't found, append it */
        if (!found) {
            fprintf(temp_file, "[0x%X]\n", crc32);
            if (p->rom_name[0] != '\0') {
                fprintf(temp_file, "name=%s\n", p->rom_name);
            }
            fprintf(temp_file, "load_store_quirk=%d\n", p->quirks.load_store_quirk);
            fprintf(temp_file, "shift_quirk=%d\n", p->quirks.shift_quirk);
            fprintf(temp_file, "jump_quirk=%d\n", p->quirks.jump_quirk);
            fprintf(temp_file, "logic_vf_quirk=%d\n", p->quirks.logic_vf_quirk);
            fprintf(temp_file, "i_overflow_quirk=%d\n", p->quirks.i_overflow_quirk);
            fprintf(temp_file, "draw_flag_quirk=%d\n", p->quirks.draw_flag_quirk);
            fprintf(temp_file, "vwrap=%d\n", p->quirks.vwrap);
            fprintf(temp_file, "hwrap=%d\n\n", p->quirks.hwrap);
        }

        fclose(file);
        fclose(temp_file);

        /* Replace original with temp */
        remove(filepath);
        rename(temp_filepath, filepath);

    } else {
        /* File doesn't exist: create new */
        file = fopen(filepath, "w");
        if (!file) {
            fprintf(stderr, "Error: Unable to create %s\n", filepath);
            return;
        }

        fprintf(file, "# ROM Profiles Database\n");
        fprintf(file, "# Format: [0xCRC32] followed by quirk settings\n\n");
        fprintf(file, "[0x%X]\n", crc32);
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
        fprintf(file, "hwrap=%d\n", p->quirks.hwrap);

        fclose(file);
    }
}

void profiles_save_current(void) {
    uint32_t crc32;
    struct profile p;

    /* Check if ROM is actually loaded */
    if (!chip8.rom_loaded || !chip8.rom || chip8.rom_size == 0) {
        fprintf(stderr, "Error: No ROM loaded. Cannot save profile.\n");
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

    /* Add to runtime hashmap */
    hmput(profile_map, crc32, p);

    /* Write to INI file */
    profiles_write_to_file(crc32, &p);

    printf("Saved ROM profile for: %s (CRC32: 0x%X)\n",
           chip8.rom_filename, crc32);
}
