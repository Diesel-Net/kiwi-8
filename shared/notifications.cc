#include "notifications.h"
#include <string.h>
#include <stdio.h>

#define MAX_NOTIFICATIONS 8
#define NOTIFICATION_DURATION 3.0

static struct notification notifications[MAX_NOTIFICATIONS];

void notify_init(void) {
    memset(notifications, 0, sizeof(notifications));
}

void notify_show(int type, const char *message) {
    int slot = -1;

    /* Find first inactive slot */
    for (int i = 0; i < MAX_NOTIFICATIONS; i++) {
        if (!notifications[i].active) {
            slot = i;
            break;
        }
    }

    /* If all slots full, overwrite the one with least time remaining */
    if (slot == -1) {
        slot = 0;
        double min_time = notifications[0].time_remaining;
        for (int i = 1; i < MAX_NOTIFICATIONS; i++) {
            if (notifications[i].time_remaining < min_time) {
                min_time = notifications[i].time_remaining;
                slot = i;
            }
        }
    }

    /* Populate the notification */
    notifications[slot].type = type;
    notifications[slot].time_remaining = NOTIFICATION_DURATION;
    notifications[slot].active = 1;
    strncpy(notifications[slot].message, message, sizeof(notifications[slot].message) - 1);
    notifications[slot].message[sizeof(notifications[slot].message) - 1] = '\0';
}

void notify_update(double delta_time) {
    for (int i = 0; i < MAX_NOTIFICATIONS; i++) {
        if (notifications[i].active) {
            notifications[i].time_remaining -= delta_time;
            if (notifications[i].time_remaining <= 0.0) {
                notifications[i].active = 0;
            }
        }
    }
}

void notify_render(void) {
    /* Rendering is done in gui.cc via notify_get_active() */
}

/* Helper function for gui.cc to access active notifications */
const struct notification* notify_get_notifications(int *count) {
    static int active_count = 0;
    active_count = 0;

    for (int i = 0; i < MAX_NOTIFICATIONS; i++) {
        if (notifications[i].active) {
            active_count++;
        }
    }

    *count = active_count;
    return notifications;
}

/* Helper to get notification info */
void notify_get_info(const struct notification *notif, const char **message, int *type, double *time) {
    *message = notif->message;
    *type = notif->type;
    *time = notif->time_remaining;
}

int notify_is_active(const struct notification *notif) {
    return notif->active;
}
