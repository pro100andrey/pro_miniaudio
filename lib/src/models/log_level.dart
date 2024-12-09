part of '../library.dart';

enum FileLogLevel {
  debug(0),
  info(1),
  warning(2),
  error(3);

  const FileLogLevel(this.value);
  final int value;

  static FileLogLevel fromValue(int value) {
    if (value < 0 || value >= FileLogLevel.values.length) {
      throw RangeError('Invalid FileLogLevel value: $value');
    }

    return FileLogLevel.values[value];
  }
}
