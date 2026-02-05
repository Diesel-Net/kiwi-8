#ifndef QUIRKS_H
#define QUIRKS_H

struct quirks {
    bool load_store_quirk;
    bool shift_quirk;
    bool jump_quirk;         /* BNNN: use VX instead of V0 for offset */
    bool logic_vf_quirk;     /* 8XY1/2/3: set VF=0 */
    bool i_overflow_quirk;   /* I+VX overflow sets VF (FX1E quirk) */
    bool draw_flag_quirk;    /* draw_flag reset behavior */
    bool vwrap;              /* vertical wrapping */
    bool hwrap;              /* horizontal wrapping */
};

/* Get default quirks */
static inline struct quirks quirks_get_defaults(void) {
    struct quirks defaults;
    defaults.load_store_quirk = 1;
    defaults.shift_quirk = 1;
    defaults.jump_quirk = 0;
    defaults.logic_vf_quirk = 0;
    defaults.i_overflow_quirk = 0;
    defaults.draw_flag_quirk = 0;
    defaults.vwrap = 1;
    defaults.hwrap = 0;
    return defaults;
}

#endif // QUIRKS_H
