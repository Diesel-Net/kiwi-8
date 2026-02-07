#ifndef TOAST_H
#define TOAST_H

/* Toast struct definition */
struct toast {
    char message[256];
    int type;
    double time_remaining;
    int active;
};

/* Toast types */
#define TOAST_INFO 0
#define TOAST_SUCCESS 1
#define TOAST_ERROR 2

/* Initialize toast system */
void toast_init(void);

/* Show a toast */
void toast_show(int type, const char *message);

/* Update toast timers (call each frame with delta time) */
void toast_update(double delta_time);

/* Render toasts (called from gui.cc) */
void toast_render(void);

/* Helper functions for accessing toast data */
const struct toast* toast_get_toasts(int *count);
int toast_is_active(const struct toast *t);
void toast_get_info(const struct toast *t, const char **message, int *type, double *time);

#endif
