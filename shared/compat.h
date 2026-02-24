#ifndef COMPAT_H
#define COMPAT_H

#include <limits.h>

// Handle missing PATH_MAX on some systems
#if defined(__APPLE__)
    #include <sys/syslimits.h>
#elif defined(__linux__)
    #include <linux/limits.h>
#elif defined(_WIN32)
    #include <windows.h>
    #ifndef PATH_MAX
        #define PATH_MAX MAX_PATH
    #endif
#endif

#ifndef PATH_MAX
    #define PATH_MAX 4096
#endif
#ifndef FILENAME_MAX
    #define FILENAME_MAX 256
#endif
#ifndef LINE_MAX
    #define LINE_MAX 2048
#endif

#endif // COMPAT_H
