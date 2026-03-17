#include "profiles.h"
#include "compat.h"
#include "stb_ds.h"
#include "sha256.h"
#include "toast.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>

/* ROM profile hashmap: SHA256 -> profile */
static struct { sha256_hash_t key; struct profile value; } *profile_map = NULL;

/* Track which path we loaded profiles.ini from */
static char loaded_profiles_path[PATH_MAX] = "";

static void set_path(char *dst, size_t dst_size, const char *src) {
    strncpy(dst, src, dst_size - 1);
    dst[dst_size - 1] = '\0';
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

/* Decode a 64-character hex string into a 32-byte SHA-256 digest.
 * Returns 0 on success, non-zero on failure. */
static int sha256_hex_to_bytes(const char *hex_str, sha256_hash_t *out) {
    for (int i = 0; i < 32; i++) {
        unsigned int byte;
        if (sscanf(hex_str + i*2, "%2x", &byte) != 1) {
            printf("Warning: Invalid hex byte at offset %d in SHA256: %s\n", i*2, hex_str);
            return 1;
        }
        out->bytes[i] = (uint8_t)byte;
    }
    return 0;
}

/* Trim leading and trailing whitespace from str in-place. */
static void trim_whitespace(char *str) {
    char *start = str;
    while (*start && isspace((unsigned char)*start)) start++;
    if (start != str) memmove(str, start, strlen(start) + 1);
    size_t len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) str[--len] = '\0';
}

/* Create an empty profiles.ini with a comment header.
 * Returns 0 on success, non-zero on failure. */
static int create_profiles_ini(const char *path) {
    FILE *file = fopen(path, "w");
    if (!file) return 1;
    fprintf(file, "# ROM Profiles Database\n");
    fprintf(file, "# Format: [SHA256] followed by quirk settings\n\n");
    fclose(file);
    return 0;
}

/* Flush a completed profile entry into the hashmap. No-op if has_current is 0. */
static void commit_pending_profile(int has_current, const sha256_hash_t *sha256,
                                   const struct profile *p) {
    if (!has_current) return;
    hmput(profile_map, *sha256, *p);
    printf("Profile loaded: %s\n", p->rom_name);
    quirks_print(&p->quirks, "Quirks:");
}

/* Parse a section-header line of the form [SHA256] or [0xSHA256] (case-insensitive).
 * Writes the decoded 32-byte hash into *sha256_out.
 * Returns 0 on success, non-zero on any parse or validation error. */
static int parse_section_header(const char *line, sha256_hash_t *sha256_out) {
    char inner[128];
    if (sscanf(line, "[%127[^]]]", inner) != 1) {
        printf("Warning: Malformed section header: %s\n", line);
        return 1;
    }

    /* Skip optional "0x" / "0X" prefix */
    const char *hex_start = inner;
    if (inner[0] == '0' && (inner[1] == 'x' || inner[1] == 'X'))
        hex_start = inner + 2;

    if (strlen(hex_start) != 64) {
        printf("Warning: SHA256 must be 64 hex characters (got %zu): %s\n",
               strlen(hex_start), hex_start);
        return 1;
    }

    return sha256_hex_to_bytes(hex_start, sha256_out);
}

/* Parse a key=value line and apply it to the in-progress profile.
 * Trims whitespace from both key and value.
 * Warns about unrecognized keys. */
static void parse_profile_field(const char *line, struct profile *p) {
    char key[256];
    char str_value[256];

    if (sscanf(line, "%255[^=]=%255[^\n]", key, str_value) != 2) return;
    trim_whitespace(key);
    trim_whitespace(str_value);

    if (strcmp(key, "name") == 0) {
        set_path(p->rom_name, sizeof(p->rom_name), str_value);
        return;
    }

    int int_value;
    if (sscanf(str_value, "%d", &int_value) == 1) {
        if (quirks_set_field_by_name(&p->quirks, key, int_value)) return;
    }

    printf("Warning: Unknown profile field '%s' — ignored\n", key);
}

/* Parse profiles.ini at loaded_profiles_path into profile_map. */
static void parse_profiles_ini(void) {
    FILE *file = fopen(loaded_profiles_path, "r");
    if (!file) return;

    char line[LINE_MAX];
    sha256_hash_t current_sha256 = {0};
    struct profile current_profile;
    int has_current = 0;

    while (fgets(line, sizeof(line), file)) {
        /* Strip trailing newline */
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';

        /* Skip blank lines and full-line comments */
        if (line[0] == '\0' || line[0] == '#') continue;

        if (line[0] == '[') {
            /* Commit the previous profile before starting a new one */
            commit_pending_profile(has_current, &current_sha256, &current_profile);

            sha256_hash_t parsed_sha256;
            if (parse_section_header(line, &parsed_sha256) != 0) {
                has_current = 0;
                continue;
            }

            current_sha256 = parsed_sha256;
            memset(&current_profile, 0, sizeof(current_profile));
            current_profile.sha256 = current_sha256;
            current_profile.quirks = quirks_get_defaults();
            has_current = 1;
            continue;
        }

        /* Key=value line: only process when inside a valid section */
        if (has_current)
            parse_profile_field(line, &current_profile);
    }

    /* Commit the last profile in the file */
    commit_pending_profile(has_current, &current_sha256, &current_profile);
    fclose(file);
}

/* Populate paths with the default locations to look for profiles.ini,
 * ordered by preference (alongside the executable, then macOS bundle Resources). */
static void build_default_search_paths(char paths[2][LINE_MAX]) {
    char *base_path = SDL_GetBasePath();
    if (!base_path) {
        printf("Warning: Could not determine executable path. Trying current directory.\n");
        snprintf(paths[0], LINE_MAX, "./profiles.ini");
        snprintf(paths[1], LINE_MAX, "../Resources/profiles.ini");
    } else {
        snprintf(paths[0], LINE_MAX, "%sprofiles.ini", base_path);
        snprintf(paths[1], LINE_MAX, "%s../Resources/profiles.ini", base_path);
        SDL_free(base_path);
    }
}

/* Search candidate paths for an existing readable profiles.ini.
 * On success, copies the found path into loaded_profiles_path and returns 0.
 * Returns non-zero if no candidate exists. */
static int find_existing_profiles_file(char candidates[2][LINE_MAX]) {
    for (int i = 0; i < 2; i++) {
        FILE *file = fopen(candidates[i], "r");
        if (file) {
            fclose(file);
            set_path(loaded_profiles_path, sizeof(loaded_profiles_path), candidates[i]);
            return 0;
        }
    }
    return 1;
}

/* Locate or create profiles.ini, populating loaded_profiles_path.
 * If a custom path was set by profiles_init(), honour it exclusively.
 * Otherwise, search next to the executable and in macOS bundle Resources,
 * creating a new file in the first candidate location if none is found.
 * Returns 0 on success, non-zero on failure. */
static int resolve_profiles_path(void) {
    char msg[TOAST_MSG_MAX];

    /* --- Custom path (set via --profiles flag) --- */
    if (loaded_profiles_path[0] != '\0') {
        FILE *file = fopen(loaded_profiles_path, "r");
        if (file) {
            fclose(file);
            snprintf(msg, sizeof(msg), "ROM profiles: %s", loaded_profiles_path);
            toast_show(TOAST_INFO, msg);
            printf("%s\n", msg);
            return 0;
        }
        /* File does not exist yet — create it */
        if (create_profiles_ini(loaded_profiles_path) == 0) {
            snprintf(msg, sizeof(msg), "ROM profiles created: %s", loaded_profiles_path);
            toast_show(TOAST_SUCCESS, msg);
            printf("%s\n", msg);
            return 0;
        } else {
            snprintf(msg, sizeof(msg), "Failed to create: %s", loaded_profiles_path);
            toast_show(TOAST_ERROR, msg);
            printf("%s\n", msg);
            return 1;
        }
    }

    /* --- Default search paths --- */
    char search_paths[2][LINE_MAX];
    build_default_search_paths(search_paths);

    if (find_existing_profiles_file(search_paths) == 0) {
        snprintf(msg, sizeof(msg), "ROM profiles: %s", loaded_profiles_path);
        toast_show(TOAST_INFO, msg);
        printf("%s\n", msg);
        return 0;
    }

    /* --- Not found: create at the first candidate path --- */
    if (create_profiles_ini(search_paths[0]) == 0) {
        set_path(loaded_profiles_path, sizeof(loaded_profiles_path), search_paths[0]);
        snprintf(msg, sizeof(msg), "ROM profiles created: %s", search_paths[0]);
        toast_show(TOAST_INFO, msg);
        printf("%s\n", msg);
        return 0;
    } else {
        snprintf(msg, sizeof(msg), "Failed to create: %s", search_paths[0]);
        toast_show(TOAST_ERROR, msg);
        printf("%s\n", msg);
        return 1;
    }
}

void profiles_init(const char *custom_path) {
    if (profile_map != NULL) return;

    if (custom_path && custom_path[0] != '\0') {
        set_path(loaded_profiles_path, sizeof(loaded_profiles_path), custom_path);
    }
    if (resolve_profiles_path() == 0) {
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

    printf("Lookup result: found — name=%s\n", profile_map[idx].value.rom_name);
    quirks_print(&profile_map[idx].value.quirks, "Lookup quirks:");
    return &profile_map[idx].value;
}

/* Rewrite the entire profiles.ini from the in-memory hashmap.
 * The file is fully replaced on each save rather than appended,
 * which keeps the format clean and avoids duplicate entries. */
static int profiles_write_to_file(void) {
    if (loaded_profiles_path[0] == '\0') {
        printf("Error: profiles.ini path not initialized\n");
        return 1;
    }

    FILE *file = fopen(loaded_profiles_path, "w");
    if (!file) {
        printf("Error: Unable to write to %s\n", loaded_profiles_path);
        return 1;
    }

    fprintf(file, "# ROM Profiles Database\n");
    fprintf(file, "# Format: [SHA256] followed by quirk settings\n\n");

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
    return 0;
}

int profiles_save(const uint8_t *rom, size_t rom_size, const char *rom_name,
                  const struct quirks *quirks) {
    if (!rom || rom_size == 0 || !rom_name || !quirks) {
        toast_show(TOAST_ERROR, "No ROM loaded. Cannot save profile.");
        return 1;
    }
    if (loaded_profiles_path[0] == '\0') {
        toast_show(TOAST_ERROR, "Could not locate profiles.ini");
        return 1;
    }

    sha256_hash_t sha256;
    sha256_easy_hash(rom, rom_size, sha256.bytes);

    struct profile p = {0};
    p.sha256 = sha256;
    set_path(p.rom_name, sizeof(p.rom_name), rom_name);
    p.quirks = *quirks;

    hmput(profile_map, sha256, p);
    if (profiles_write_to_file() != 0) {
        toast_show(TOAST_ERROR, "Failed to write profiles.ini");
        return 1;
    }

    char hash_hex[65];
    bytes_to_hex(sha256.bytes, hash_hex, sizeof(hash_hex));
    printf("Saved ROM profile for: %s (SHA256: %s)\n", rom_name, hash_hex);
    char msg[TOAST_MSG_MAX];
    snprintf(msg, sizeof(msg), "Profile saved: %s", rom_name);
    toast_show(TOAST_SUCCESS, msg);
    return 0;
}
