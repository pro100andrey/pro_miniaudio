#ifndef PLATFORM_H
#define PLATFORM_H

/**
 * This header provides platform-specific includes and macros to ensure
 * portability across different operating systems.
 */

/* Windows platform-specific settings */
#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>                            /**< Include Windows-specific headers. */
    #define FFI_PLUGIN_EXPORT __declspec(dllexport) /**< Export symbol for Windows. */
/* Non-Windows (POSIX) platform-specific settings */
#else
    #include <pthread.h> /**< Include pthread for thread handling. */
    #include <unistd.h>  /**< Include POSIX standard API. */
    #if defined(__GNUC__) || defined(__clang__)
        #define FFI_PLUGIN_EXPORT __attribute__((visibility("default"))) /**< Export symbol for GCC/Clang. */
    #else
    #define FFI_PLUGIN_EXPORT /**< Fallback for other compilers. */
    #endif
#endif

#endif  // PLATFORM_H
