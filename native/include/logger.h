#ifndef LOGGER_H
#define LOGGER_H

#include <stdbool.h>

#include "platform.h"

/**
 * @enum LogLevel
 * @brief Defines the severity levels for log messages.
 */
typedef enum {
    LOG_LEVEL_DEBUG,   /**< Debug level: Detailed information for debugging purposes. */
    LOG_LEVEL_INFO,    /**< Info level: General informational messages. */
    LOG_LEVEL_WARNING, /**< Warning level: Potential issues that are non-critical. */
    LOG_LEVEL_ERROR    /**< Error level: Critical issues requiring immediate attention. */
} LogLevel;

/**
 * @brief Sets the current log level.
 *
 * Messages with a severity level lower than the set level will be ignored.
 *
 * @param level The log level to set (DEBUG, INFO, WARNING, ERROR).
 */
FFI_PLUGIN_EXPORT
void set_log_level(LogLevel level);

/**
 * @brief Enables or disables logging to a file.
 *
 * When enabled, all log messages will be written to the specified log file.
 *
 * @param enabled `true` to enable file logging, `false` to disable it.
 */
FFI_PLUGIN_EXPORT
void set_log_to_file_enabled(bool enabled);

/**
 * @brief Checks if file logging is currently enabled.
 *
 * @return `true` if file logging is enabled, `false` otherwise.
 */
FFI_PLUGIN_EXPORT
bool is_log_to_file_enabled(void);

/**
 * @brief Initializes logging to a file.
 *
 * Opens the specified file for writing log messages. If the file already exists,
 * new log messages will be appended.
 *
 * @param filename The path to the log file.
 */
FFI_PLUGIN_EXPORT
void init_file_log(const char *filename);

/**
 * @brief Closes the currently opened log file.
 *
 * Call this function to properly close the log file and release associated resources.
 */
FFI_PLUGIN_EXPORT
void close_file_log(void);

/**
 * @brief Enables or disables logging to the console.
 *
 * When enabled, all log messages will be written to the console (stdout/stderr).
 *
 * @param enabled `true` to enable console logging, `false` to disable it.
 */
FFI_PLUGIN_EXPORT
void set_log_to_console_enabled(bool enabled);

/**
 * @brief Logs a message with the specified severity level.
 *
 * This is a general-purpose function for logging messages. Use the predefined macros
 * `LOG_DEBUG`, `LOG_INFO`, `LOG_WARN`, and `LOG_ERROR` for convenience.
 *
 * @param level The severity level of the log message.
 * @param funcName The name of the function where the log message is generated.
 * @param format The format string (similar to printf).
 * @param ... Additional arguments for the format string.
 */
FFI_PLUGIN_EXPORT
void log_message(LogLevel level, const char *funcName, const char *format, ...);

/**
 * @brief Logs a debug-level message.
 *
 * Logs a message at the DEBUG level, including the function name.
 *
 * @param fmt The format string (similar to printf).
 * @param ... Additional arguments for the format string.
 */
#define LOG_DEBUG(fmt, ...) log_message(LOG_LEVEL_DEBUG, __func__, fmt __VA_OPT__(, ) __VA_ARGS__)

/**
 * @brief Logs an info-level message.
 *
 * Logs a message at the INFO level, including the function name.
 *
 * @param fmt The format string (similar to printf).
 * @param ... Additional arguments for the format string.
 */
#define LOG_INFO(fmt, ...) log_message(LOG_LEVEL_INFO, __func__, fmt __VA_OPT__(, ) __VA_ARGS__)

/**
 * @brief Logs a warning-level message.
 *
 * Logs a message at the WARNING level, including the function name.
 *
 * @param fmt The format string (similar to printf).
 * @param ... Additional arguments for the format string.
 */
#define LOG_WARN(fmt, ...) log_message(LOG_LEVEL_WARNING, __func__, fmt __VA_OPT__(, ) __VA_ARGS__)

/**
 * @brief Logs an error-level message.
 *
 * Logs a message at the ERROR level, including the function name.
 *
 * @param fmt The format string (similar to printf).
 * @param ... Additional arguments for the format string.
 */
#define LOG_ERROR(fmt, ...) log_message(LOG_LEVEL_ERROR, __func__, fmt __VA_OPT__(, ) __VA_ARGS__)

#endif  // LOGGER_H
