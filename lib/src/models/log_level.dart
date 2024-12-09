part of '../library.dart';

/// Enum representing log levels for file-based logging.
///
/// The log level indicates the severity of a log message, ranging from
/// general debugging information to critical error messages.
enum FileLogLevel {
  /// Debug level.
  ///
  /// Used for detailed debugging information that is helpful during
  /// development.
  debug(0),

  /// Info level.
  ///
  /// Used for general informational messages about the application's execution.
  info(1),

  /// Warning level.
  ///
  /// Used for non-critical issues that might require attention.
  warning(2),

  /// Error level.
  ///
  /// Used for critical issues that affect the application's execution or
  /// reliability.
  error(3);

  /// Creates a [FileLogLevel] instance with the associated integer value.
  const FileLogLevel(this.value);

  /// The integer value associated with the log level.
  final int value;

  /// Returns the [FileLogLevel] corresponding to the given integer [value].
  ///
  /// Throws a [RangeError] if the value is out of the valid range.
  static FileLogLevel fromValue(int value) {
    if (value < 0 || value >= FileLogLevel.values.length) {
      throw RangeError('Invalid FileLogLevel value: $value');
    }

    return FileLogLevel.values[value];
  }
}
