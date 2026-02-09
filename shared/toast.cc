#include "toast.h"
#include <string.h>

#define MAX_TOASTS 8
#define TOAST_DURATION 3.0

static struct toast toasts[MAX_TOASTS];

void toast_init(void) {
    memset(toasts, 0, sizeof(toasts));
}

void toast_show(int type, const char *message) {
    int slot = -1;

    /* Find first inactive slot */
    for (int i = 0; i < MAX_TOASTS; i++) {
        if (!toasts[i].active) {
            slot = i;
            break;
        }
    }

    /* If all slots full, overwrite the one with least time remaining */
    if (slot == -1) {
        slot = 0;
        double min_time = toasts[0].time_remaining;
        for (int i = 1; i < MAX_TOASTS; i++) {
            if (toasts[i].time_remaining < min_time) {
                min_time = toasts[i].time_remaining;
                slot = i;
            }
        }
    }

    /* Populate the toast */
    toasts[slot].type = type;
    toasts[slot].time_remaining = TOAST_DURATION;
    toasts[slot].active = 1;
    strncpy(toasts[slot].message, message, sizeof(toasts[slot].message) - 1);
    toasts[slot].message[sizeof(toasts[slot].message) - 1] = '\0';
}

void toast_update(double delta_time) {
    for (int i = 0; i < MAX_TOASTS; i++) {
        if (toasts[i].active) {
            toasts[i].time_remaining -= delta_time;
            if (toasts[i].time_remaining <= 0.0) {
                toasts[i].active = 0;
            }
        }
    }
}

void toast_render(void) {
    /* Rendering is done in gui.cc via toast_get_toasts() */
}

/* Helper function for gui.cc to access active toasts */
const struct toast* toast_get_toasts(int *count) {
    static int active_count = 0;
    active_count = 0;

    for (int i = 0; i < MAX_TOASTS; i++) {
        if (toasts[i].active) {
            active_count++;
        }
    }

    *count = active_count;
    return toasts;
}

/* Helper to get toast info */
void toast_get_info(const struct toast *t, const char **message, int *type, double *time) {
    *message = t->message;
    *type = t->type;
    *time = t->time_remaining;
}

int toast_is_active(const struct toast *t) {
    return t->active;
}
