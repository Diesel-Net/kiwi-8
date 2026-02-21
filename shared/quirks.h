#ifndef QUIRKS_H
#define QUIRKS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/*
 * X-macro list of all quirk fields.
 * Format: X(field_name, default_value)
 * This is the single source of truth — the struct, defaults,
 * and INI field table are all generated from this list.
 */
#define QUIRK_FIELDS(X)                 \
    X(load_store_quirk, 1)              \
    X(shift_quirk,      1)              \
    X(jump_quirk,       0)              \
    X(logic_vf_quirk,   0)              \
    X(i_overflow_quirk, 0)              \
    X(draw_flag_quirk,  0)              \
    X(vwrap,            1)              \
    X(hwrap,            0)


struct quirks {
#define QUIRK_X_FIELD(name, default_val) bool name;
    QUIRK_FIELDS(QUIRK_X_FIELD)
#undef QUIRK_X_FIELD
};

/* Get default quirks */
static inline struct quirks quirks_get_defaults(void) {
    struct quirks defaults;
#define QUIRK_X_DEFAULT(name, default_val) defaults.name = default_val;
    QUIRK_FIELDS(QUIRK_X_DEFAULT)
#undef QUIRK_X_DEFAULT
    return defaults;
}

#ifdef __cplusplus
}
#endif

#endif // QUIRKS_H
