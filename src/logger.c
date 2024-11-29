#include "logger.h"

#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

// Global variables
static pthread_mutex_t g_logMutex = PTHREAD_MUTEX_INITIALIZER;
static bool g_logToFileEnabled = false;
static bool g_logToConsoleEnabled = false;
static FILE *g_logFile = NULL;

FFI_PLUGIN_EXPORT
void set_log_to_file_enabled(bool enabled) {
    g_logToFileEnabled = enabled;
}

FFI_PLUGIN_EXPORT
bool is_log_to_file_enabled(void) {
    return g_logToFileEnabled;
}

FFI_PLUGIN_EXPORT
void init_log(const char *filename) {
    pthread_mutex_lock(&g_logMutex);
    if (g_logFile == NULL) {
        g_logFile = fopen(filename, "a");
        if (g_logFile == NULL) {
            perror("Failed to open log file");
        }
    }
    pthread_mutex_unlock(&g_logMutex);
}

FFI_PLUGIN_EXPORT
void close_log(void) {
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
void log_message(LogLevel level, const char *format, ...) {
    pthread_mutex_lock(&g_logMutex);

    // Get the current time
    time_t now = time(NULL);
    struct tm *localTime = localtime(&now);
    char timeStr[64];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localTime);

    // Determine log level
    const char *levelStr = NULL;
    switch (level) {
        case LOG_LEVEL_DEBUG:
            levelStr = "[DEBUG]";
            break;
        case LOG_LEVEL_INFO:
            levelStr = "[INFO]";
            break;
        case LOG_LEVEL_ERROR:
            levelStr = "[ERROR]";
            break;
        default:
            levelStr = "[UNKNOWN]";
            break;
    }

    // Write to file if enabled
    if (g_logToFileEnabled && g_logFile != NULL) {
        fprintf(g_logFile, "%s %s ", timeStr, levelStr);
        va_list args;
        va_start(args, format);
        vfprintf(g_logFile, format, args);
        va_end(args);
        fprintf(g_logFile, "\n");
        fflush(g_logFile);
    }

    // Write to console if enabled
    if (g_logToConsoleEnabled) {
        printf("%s %s ", timeStr, levelStr);
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
        printf("\n");
    }

    pthread_mutex_unlock(&g_logMutex);
}
