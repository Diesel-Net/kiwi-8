#include "profiles.h"
#include "compat.h"
#include "stb_ds.h"
#include "sha256.h"
#include "chip8.h"
#include "toast.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

/* ROM profile hashmap: SHA256 -> profile */
static struct { sha256_hash_t key; struct profile value; } *profile_map = NULL;

/* Track which path we loaded profiles.ini from */
static char loaded_profiles_path[PATH_MAX] = "";

static void set_path(char *dst, size_t dst_size, const char *src) {
    strncpy(dst, src, dst_size - 1);
    dst[dst_size - 1] = '\0';
}

static int quirks_set_field_by_name(struct quirks *quirks, const char *field_name, int value) {
    bool enabled = value != 0;
#define QUIRK_X_SET(name, default_val) \
    if (strcmp(field_name, #name) == 0) { \
        quirks->name = enabled; \
        return 1; \
    }
    QUIRK_FIELDS(QUIRK_X_SET)
#undef QUIRK_X_SET
    return 0;
}

/* Convert 32-byte digest to lowercase hex (null-terminated) */
static void bytes_to_hex(const uint8_t *bytes, char *hex_out, size_t dst_size) {
    if (dst_size < 65) return;
    static const char *lut = "0123456789abcdef";
    for (int i = 0; i < 32; ++i) {
        uint8_t c = bytes[i];
        hex_out[i*2] = lut[c >> 4];
        hex_out[i*2 + 1] = lut[c & 0x0f];
    }
    hex_out[64] = '\0';
}

/* Write the empty profiles.ini header to a new file. Returns 1 on success. */
static int create_profiles_ini(const char *path) {
    FILE *file = fopen(path, "w");
    if (!file) return 0;
    fprintf(file, "# ROM Profiles Database\n");
    fprintf(file, "# Format: [0xSHA256] followed by quirk settings\n\n");
    fclose(file);
    return 1;
}

/* Parse profiles.ini at loaded_profiles_path into profile_map */
static void parse_profiles_ini(void) {
    FILE *file = fopen(loaded_profiles_path, "r");
    if (!file) return;

    char line[LINE_MAX];
    sha256_hash_t current_sha256 = {0};
    int has_current = 0;
    struct profile current_profile;

    while (fgets(line, sizeof(line), file)) {
        /* Strip newline */
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';

        /* Skip empty lines and comments */
        if (line[0] == '\0' || line[0] == '#') continue;

        /* Section header [0xHEXVALUE] - 64 hex chars for SHA256 */
        if (line[0] == '[') {
            if (has_current){
                hmput(profile_map, current_sha256, current_profile);
                printf("Profile loaded: %s\n", current_profile.rom_name);
                quirks_print(&current_profile.quirks, "Quirks:");
            }

            char hex_str[65];
            if (sscanf(line, "[%64[0-9a-fA-F]]", hex_str) != 1) {
                printf("Error: Invalid profile section header: %s\n", line);
                has_current = 0;
                continue;
            }

            /* Parse 64 hex characters into 32 bytes */
            if (strlen(hex_str) != 64) {
                printf("Error: SHA256 must be 64 hex characters: %s\n", hex_str);
                has_current = 0;
                continue;
            }

            for (int i = 0; i < 32; i++) {
                unsigned int byte;
                if (sscanf(hex_str + i*2, "%2x", &byte) != 1) {
                    printf("Error: Invalid hex in SHA256: %s\n", hex_str);
                    has_current = 0;
                    goto next_line;
                }
                current_sha256.bytes[i] = (uint8_t)byte;
            }

            memset(&current_profile, 0, sizeof(current_profile));
            current_profile.sha256 = current_sha256;
            current_profile.quirks = quirks_get_defaults();
            has_current = 1;
            continue;

        next_line:
            continue;
        }

        /* Key=value pairs */
        if (!has_current) continue;

        char key[256];
        int value;
        char str_value[256];

        if (sscanf(line, "%255[^=]=%d", key, &value) == 2) {
            quirks_set_field_by_name(&current_profile.quirks, key, value);
        } else if (sscanf(line, "%255[^=]=%255[^\n]", key, str_value) == 2) {
            if (strcmp(key, "name") == 0) {
                set_path(current_profile.rom_name, sizeof(current_profile.rom_name), str_value);
            }
        }
    }

    /* Commit last entry */
    if (has_current) {
        hmput(profile_map, current_sha256, current_profile);
        printf("Profile loaded: %s\n", current_profile.rom_name);
        quirks_print(&current_profile.quirks, "Quirks:");
    }
    fclose(file);
}

/* Resolve profiles.ini path: use custom path if set, otherwise search near executable.
 * Creates an empty file if none found. Returns 1 if path is valid. */
static int resolve_profiles_path(void) {
    /* Custom path already set via profiles_init() */
    if (loaded_profiles_path[0] != '\0') {
        FILE *file = fopen(loaded_profiles_path, "r");
        if (file) {
            fclose(file);
            char msg[TOAST_MSG_MAX];
            snprintf(msg, sizeof(msg), "ROM profiles: %s", loaded_profiles_path);
            toast_show(TOAST_INFO, msg);
            printf("%s\n", msg);
            return 1;
        }
        /* Doesn't exist yet — create it */
        if (create_profiles_ini(loaded_profiles_path)) {
            char msg[TOAST_MSG_MAX];
            snprintf(msg, sizeof(msg), "ROM profiles created: %s", loaded_profiles_path);
            toast_show(TOAST_SUCCESS, msg);
            printf("%s\n", msg);
        } else {
            char msg[TOAST_MSG_MAX];
            snprintf(msg, sizeof(msg), "Failed to create: %s", loaded_profiles_path);
            toast_show(TOAST_ERROR, msg);
            printf("%s\n", msg);
        }
        return 0;
    }

    /* Build search paths relative to executable */
    char search_paths[2][LINE_MAX];
    char *base_path = SDL_GetBasePath();
    if (!base_path) {
        printf("Warning: Could not determine executable path. Trying current directory.\n");
        snprintf(search_paths[0], sizeof(search_paths[0]), "./profiles.ini");
        snprintf(search_paths[1], sizeof(search_paths[1]), "../Resources/profiles.ini");
    } else {
        snprintf(search_paths[0], sizeof(search_paths[0]), "%sprofiles.ini", base_path);
        snprintf(search_paths[1], sizeof(search_paths[1]), "%s../Resources/profiles.ini", base_path);
        SDL_free(base_path);
    }

    /* Try each search path */
    for (int i = 0; i < 2; i++) {
        FILE *file = fopen(search_paths[i], "r");
        if (file) {
            fclose(file);
            set_path(loaded_profiles_path, sizeof(loaded_profiles_path), search_paths[i]);
            char msg[TOAST_MSG_MAX];
            snprintf(msg, sizeof(msg), "ROM profiles: %s", search_paths[i]);
            toast_show(TOAST_INFO, msg);
            printf("%s\n", msg);
            return 1;
        }
    }

    /* Not found — create at first search path */
    if (create_profiles_ini(search_paths[0])) {
        set_path(loaded_profiles_path, sizeof(loaded_profiles_path), search_paths[0]);
        char msg[TOAST_MSG_MAX];
        snprintf(msg, sizeof(msg), "ROM profiles created: %s", search_paths[0]);
        toast_show(TOAST_INFO, msg);
        printf("%s\n", msg);
    } else {
        char msg[TOAST_MSG_MAX];
        snprintf(msg, sizeof(msg), "Failed to create: %s", search_paths[0]);
        toast_show(TOAST_ERROR, msg);
        printf("%s\n", msg);
    }
    return 0;
}

void profiles_init(const char *custom_path) {
    if (profile_map != NULL) return;

    if (custom_path && custom_path[0] != '\0') {
        set_path(loaded_profiles_path, sizeof(loaded_profiles_path), custom_path);
    }
    if (resolve_profiles_path()) {
        parse_profiles_ini();
    }
}

const struct profile* profile_lookup(const sha256_hash_t *sha256) {
    char hash_hex[65];
    bytes_to_hex(sha256->bytes, hash_hex, sizeof(hash_hex));
    printf("Lookup SHA256: %s\n", hash_hex);

    ptrdiff_t idx = hmgeti(profile_map, *sha256);
    if (idx < 0) {
        printf("Lookup result: not found\n");
        return NULL;
    }

    char matched_hash_hex[65];
    bytes_to_hex(profile_map[idx].key.bytes, matched_hash_hex, sizeof(matched_hash_hex));
    printf("Lookup result: FOUND! name=%s\n", profile_map[idx].value.rom_name);
    quirks_print(&profile_map[idx].value.quirks, "Lookup quirks:");
    return &profile_map[idx].value;
}

/* Write entire profile hashmap to INI file */
static void profiles_write_to_file(void) {
    if (loaded_profiles_path[0] == '\0') {
        printf("Error: profiles.ini path not initialized\n");
        return;
    }

    FILE *file = fopen(loaded_profiles_path, "w");
    if (!file) {
        printf("Error: Unable to write to %s\n", loaded_profiles_path);
        return;
    }

    fprintf(file, "# ROM Profiles Database\n");
    fprintf(file, "# Format: [0xSHA256] followed by quirk settings\n\n");

    /* use file-scope bytes_to_hex */
    for (int i = 0; i < hmlen(profile_map); i++) {
        struct profile *p = &profile_map[i].value;
        char hash_hex[65];
        bytes_to_hex(p->sha256.bytes, hash_hex, sizeof(hash_hex));
        fprintf(file, "[%s]\n", hash_hex);
        if (p->rom_name[0] != '\0') fprintf(file, "name=%s\n", p->rom_name);
        #define QUIRK_X_WRITE(name, default_val) \
            fprintf(file, "%s=%d\n", #name, p->quirks.name ? 1 : 0);
        QUIRK_FIELDS(QUIRK_X_WRITE)
        #undef QUIRK_X_WRITE
        fprintf(file, "\n");
    }

    fclose(file);
}

void profiles_save_current(void) {
    if (!chip8.rom_loaded || !chip8.rom || chip8.rom_size == 0) {
        toast_show(TOAST_ERROR, "No ROM loaded. Cannot save profile.");
        return;
    }
    if (loaded_profiles_path[0] == '\0') {
        toast_show(TOAST_ERROR, "Could not locate profiles.ini");
        return;
    }

    sha256_hash_t sha256;
    sha256_easy_hash(chip8.rom, chip8.rom_size, sha256.bytes);

    struct profile p;
    memset(&p, 0, sizeof(p));
    p.sha256 = sha256;
    set_path(p.rom_name, sizeof(p.rom_name), chip8.rom_filename);
    p.quirks = chip8.quirks;

    hmput(profile_map, sha256, p);
    profiles_write_to_file();

    char hash_hex[65];
    bytes_to_hex(sha256.bytes, hash_hex, sizeof(hash_hex));
    printf("Saved ROM profile for: %s (SHA256: %s)\n", chip8.rom_filename, hash_hex);
    char msg[TOAST_MSG_MAX];
    snprintf(msg, sizeof(msg), "Profile saved: %s", chip8.rom_filename);
    toast_show(TOAST_SUCCESS, msg);
}
