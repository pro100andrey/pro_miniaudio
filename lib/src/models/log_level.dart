part of '../library.dart';

/// Enum representing log levels for file-based logging.
///
/// Log levels are used to categorize log messages by their severity or
/// importance. This enum provides a structured way to filter and manage log
/// output.
///
/// ### Log Levels
/// - [debug]: Detailed debugging information, useful during development.
/// - [info]: General information about application execution.
/// - [warning]: Non-critical issues that might require attention.
/// - [error]: Critical issues affecting the application's reliability.
///
/// Example usage:
/// ```dart
/// final logLevel = FileLogLevel.info;
/// print('Current log level: ${logLevel.value}'); // Outputs: 1
/// ```
enum FileLogLevel {
  /// Debug level.
  ///
  /// Used for detailed debugging information. These messages are typically
  /// verbose and helpful during development or troubleshooting.
  debug(0),

  /// Info level.
  ///
  /// Used for general informational messages that highlight the progress
  /// of the application at a high level.
  info(1),

  /// Warning level.
  ///
  /// Used for logging non-critical issues that might indicate potential
  /// problems or require attention.
  warning(2),

  /// Error level.
  ///
  /// Used for logging critical issues that have a significant impact
  /// on the application's execution or reliability.
  error(3);

  /// Creates a [FileLogLevel] instance with the associated integer value.
  ///
  /// Each log level corresponds to a unique integer value for comparison
  /// or storage in native APIs.
  const FileLogLevel(this.value);

  /// The integer value associated with the log level.
  ///
  /// - `debug`: 0
  /// - `info`: 1
  /// - `warning`: 2
  /// - `error`: 3
  final int value;

  /// Returns the [FileLogLevel] corresponding to the given integer [value].
  ///
  /// If the value is not within the valid range of log levels, a
  /// [RangeError] is thrown.
  ///
  /// Example:
  /// ```dart
  /// final level = FileLogLevel.fromValue(2);
  /// print(level); // Outputs: FileLogLevel.warning
  /// ```
  ///
  /// Throws:
  /// - [RangeError]: If the value is not a valid log level.
  static FileLogLevel fromValue(int value) {
    if (value < 0 || value >= FileLogLevel.values.length) {
      throw RangeError('Invalid FileLogLevel value: $value');
    }

    return FileLogLevel.values[value];
  }
}
