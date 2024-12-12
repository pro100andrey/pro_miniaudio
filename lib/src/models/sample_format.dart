part of '../library.dart';

/// Enum representing different audio sample formats.
///
/// Sample formats define the data type and bit depth of individual audio
/// samples.
enum SampleFormat {
  /// Unknown sample format.
  unknown(0),

  /// Unsigned 8-bit sample format.
  ///
  /// Each sample is represented as an 8-bit unsigned integer.
  u8(1),

  /// Signed 16-bit sample format.
  ///
  /// Each sample is represented as a 16-bit signed integer.
  s16(2),

  /// Signed 24-bit sample format.
  ///
  /// Each sample is represented as a 24-bit signed integer.
  s24(3),

  /// Signed 32-bit sample format.
  ///
  /// Each sample is represented as a 32-bit signed integer.
  s32(4),

  /// 32-bit floating-point sample format.
  ///
  /// Each sample is represented as a 32-bit IEEE floating-point number.
  f32(5),

  /// Total count of sample formats.
  ///
  /// This is used internally to determine the number of sample formats.
  count(6);

  /// The integer value associated with the sample format.
  const SampleFormat(this.value);

  /// The integer value representing the sample format.
  final int value;

  /// Returns the [SampleFormat] corresponding to the given integer [value].
  ///
  /// Throws a [RangeError] if the value is out of range.
  static SampleFormat fromValue(int value) {
    if (value < 0 || value >= SampleFormat.values.length) {
      throw RangeError('Invalid SampleFormat value: $value');
    }

    return SampleFormat.values[value];
  }

  /// Returns the name of the sample format as a string.
  ///
  /// For unknown formats, it returns `'unknown'`.
  String get name {
    switch (this) {
      case SampleFormat.u8:
        return 'u8';
      case SampleFormat.s16:
        return 's16';
      case SampleFormat.s24:
        return 's24';
      case SampleFormat.s32:
        return 's32';
      case SampleFormat.f32:
        return 'f32';
      case SampleFormat.count:
        return 'count $value';
      case _:
        return 'unknown';
    }
  }

  int get bps {
    switch (this) {
      case SampleFormat.u8:
        return 1;
      case SampleFormat.s16:
        return 2;
      case SampleFormat.s24:
        return 3;
      case SampleFormat.s32 || SampleFormat.f32:
        return 4;
      case _:
        return 0;
    }
  }
}
