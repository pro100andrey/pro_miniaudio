#ifndef PLATFORM_H
#define PLATFORM_H

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #define FFI_PLUGIN_EXPORT __declspec(dllexport)
#else
    #include <pthread.h>
    #include <unistd.h>
    #if defined(__GNUC__) || defined(__clang__)
        #define FFI_PLUGIN_EXPORT __attribute__((visibility("default")))
    #else
        #define FFI_PLUGIN_EXPORT
    #endif
#endif

#endif // PLATFORM_H
