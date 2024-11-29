#ifndef LOGGER_H
#define LOGGER_H

#include <stdbool.h>

#include "platform.h"

// Log levels for messages
typedef enum {
    LOG_LEVEL_DEBUG,  // Debug level
    LOG_LEVEL_INFO,   // Information level
    LOG_LEVEL_ERROR   // Error level
} LogLevel;

/**
 * @brief Enable or disable logging to a file.
 *
 * @param enabled If true, file logging is enabled. Otherwise, it is disabled.
 */
FFI_PLUGIN_EXPORT
void set_log_to_file_enabled(bool enabled);

/**
 * @brief Check if logging to a file is enabled.
 *
 * @return true if file logging is enabled, false otherwise.
 */
FFI_PLUGIN_EXPORT
bool is_log_to_file_enabled(void);

/**
 * @brief Initialize logging to a file.
 *
 * @param filename Path to the log file.
 */
FFI_PLUGIN_EXPORT
void init_log(const char *filename);

/**
 * @brief Close the log file.
 */
FFI_PLUGIN_EXPORT
void close_log(void);

/**
 * @brief Enable or disable logging to the console.
 *
 * @param enabled If true, console logging is enabled. Otherwise, it is disabled.
 */
FFI_PLUGIN_EXPORT
void set_log_to_console_enabled(bool enabled);

/**
 * @brief Log a message with a specified level.
 *
 * @param level The severity level of the log message (DEBUG, INFO, ERROR).
 * @param format The format string, similar to printf.
 * @param ... Additional arguments for the format string.
 */
FFI_PLUGIN_EXPORT
void log_message(LogLevel level, const char *format, ...);

#define LOG_DEBUG(fmt, ...) log_message(LOG_LEVEL_DEBUG, fmt __VA_OPT__(, ) __VA_ARGS__)
#define LOG_INFO(fmt, ...) log_message(LOG_LEVEL_INFO, fmt __VA_OPT__(, ) __VA_ARGS__)
#define LOG_ERROR(fmt, ...) log_message(LOG_LEVEL_ERROR, fmt __VA_OPT__(, ) __VA_ARGS__)

#endif  // LOGGER_H
