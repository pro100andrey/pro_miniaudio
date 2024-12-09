#include "../include/logger.h"

#include <limits.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

// Global variables
static pthread_mutex_t g_logMutex = PTHREAD_MUTEX_INITIALIZER;
static bool g_logToFileEnabled = false;
static bool g_logToConsoleEnabled = false;
static FILE *g_logFile = NULL;
static LogLevel g_logLevel = LOG_LEVEL_ERROR;

FFI_PLUGIN_EXPORT
void set_log_level(LogLevel level) {
    g_logLevel = level;
}

FFI_PLUGIN_EXPORT
void set_log_to_file_enabled(bool enabled) {
    g_logToFileEnabled = enabled;
}

FFI_PLUGIN_EXPORT
bool is_log_to_file_enabled(void) {
    return g_logToFileEnabled;
}

FFI_PLUGIN_EXPORT
void init_file_log(const char *filename) {
    pthread_mutex_lock(&g_logMutex);

    if (g_logFile != NULL) {
        fclose(g_logFile);
        g_logFile = NULL;
    }

    if (g_logFile == NULL) {
        g_logFile = fopen(filename, "a");
        if (g_logFile == NULL) {
            perror("Failed to open log file");
        }
    }

    pthread_mutex_unlock(&g_logMutex);
}

FFI_PLUGIN_EXPORT
void close_file_log(void) {
    pthread_mutex_lock(&g_logMutex);
    if (g_logFile != NULL) {
        fclose(g_logFile);
        g_logFile = NULL;
    }
    pthread_mutex_unlock(&g_logMutex);
}

FFI_PLUGIN_EXPORT
void set_log_to_console_enabled(bool enabled) {
    g_logToConsoleEnabled = enabled;
}

FFI_PLUGIN_EXPORT
void log_message(LogLevel level, const char *funcName, const char *format, ...) {
    if (level < g_logLevel) {
        return;
    }

    if (!g_logToFileEnabled && !g_logToConsoleEnabled) {
        return;
    }

    pthread_mutex_lock(&g_logMutex);

    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm *localTime = localtime(&tv.tv_sec);

    char timeStr[16];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", localTime);
    char finalTimeStr[32];
    snprintf(finalTimeStr, sizeof(finalTimeStr), "%s.%03d", timeStr, tv.tv_usec / 1000);

    // Determine log level
    const char *levelStr = NULL;
    switch (level) {
        case LOG_LEVEL_DEBUG:
            levelStr = "[DEBUG]";
            break;
        case LOG_LEVEL_STATS:
            levelStr = "[STATS]";
            break;
        case LOG_LEVEL_INFO:
            levelStr = "[INFO]";
            break;
        case LOG_LEVEL_ERROR:
            levelStr = "[ERROR]";
            break;
        case LOG_LEVEL_WARNING:
            levelStr = "[WARNING]";
            break;
        default:
            levelStr = "[UNKNOWN]";
            break;
    }

    // Write to file if enabled
    if (g_logToFileEnabled && g_logFile != NULL) {
        fprintf(g_logFile, "%s %s [%s] - ", finalTimeStr, levelStr, funcName);
        va_list args;
        va_start(args, format);
        vfprintf(g_logFile, format, args);
        va_end(args);
        fflush(g_logFile);
    }

    // Write to console if enabled
    if (g_logToConsoleEnabled) {
        printf("%s %s [%s] - ", finalTimeStr, levelStr, funcName);
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }

    pthread_mutex_unlock(&g_logMutex);
}
