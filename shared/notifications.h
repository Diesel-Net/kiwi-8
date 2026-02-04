#ifndef NOTIFICATIONS_H
#define NOTIFICATIONS_H

/* Notification struct definition */
struct notification {
    char message[256];
    int type;
    double time_remaining;
    int active;
};

/* Notification types */
#define NOTIFY_INFO 0
#define NOTIFY_SUCCESS 1
#define NOTIFY_ERROR 2

/* Initialize notification system */
void notify_init(void);

/* Show a notification */
void notify_show(int type, const char *message);

/* Update notification timers (call each frame with delta time) */
void notify_update(double delta_time);

/* Render notifications (called from gui.cc) */
void notify_render(void);

/* Helper functions for accessing notification data */
const struct notification* notify_get_notifications(int *count);
int notify_is_active(const struct notification *notif);
void notify_get_info(const struct notification *notif, const char **message, int *type, double *time);

#endif
