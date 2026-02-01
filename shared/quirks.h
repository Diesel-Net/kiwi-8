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

#endif // QUIRKS_H
