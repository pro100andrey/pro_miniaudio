part of '../library.dart';

/// Enum representing different audio sample formats.
///
/// Sample formats define the data type and bit depth of individual audio
/// samples. They are used to describe how audio data is encoded at the
/// sample level.
enum PcmFormat {
  /// Unknown sample format.
  ///
  /// This format is used when the sample format is not specified or
  /// unsupported.
  unknown(0),

  /// Unsigned 8-bit sample format.
  ///
  /// Each sample is represented as an 8-bit unsigned integer.
  /// Commonly used in low-quality or legacy audio systems.
  u8(1),

  /// Signed 16-bit sample format.
  ///
  /// Each sample is represented as a 16-bit signed integer.
  /// This is one of the most widely supported formats in modern audio.
  s16(2),

  /// Signed 24-bit sample format.
  ///
  /// Each sample is represented as a 24-bit signed integer.
  /// Provides higher precision than 16-bit formats, often used in
  /// professional audio recording.
  s24(3),

  /// Signed 32-bit sample format.
  ///
  /// Each sample is represented as a 32-bit signed integer.
  /// Suitable for applications requiring high dynamic range.
  s32(4),

  /// 32-bit floating-point sample format.
  ///
  /// Each sample is represented as a 32-bit IEEE 754 floating-point number.
  /// Ideal for audio processing and mixing due to its high precision and
  /// support for very low or high amplitudes without clipping.
  f32(5),

  /// Total count of sample formats.
  ///
  /// This is primarily used for internal purposes, such as iterating
  /// through the available formats.
  count(6);

  /// The integer value associated with the sample format.
  ///
  /// This value corresponds to the representation used in native libraries
  /// or APIs.
  const PcmFormat(this.value);

  /// The integer value representing the sample format.
  final int value;

  /// Returns the [PcmFormat] corresponding to the given integer [value].
  ///
  /// Throws a [RangeError] if the [value] is not valid.
  static PcmFormat fromValue(int value) {
    if (value < 0 || value >= PcmFormat.values.length) {
      throw RangeError('Invalid PcmFormat value: $value');
    }

    return PcmFormat.values[value];
  }

  /// Returns the name of the sample format as a string.
  ///
  /// For known formats, it returns their shorthand names (e.g., 's16', 'f32').
  /// For unknown formats, it returns `'unknown'`.
  String get name {
    switch (this) {
      case u8:
        return 'u8';
      case s16:
        return 's16';
      case s24:
        return 's24';
      case s32:
        return 's32';
      case f32:
        return 'f32';
      case count:
        return 'count';
      case _:
        return 'unknown';
    }
  }

  /// Returns the number of bytes per sample for this format.
  ///
  /// The value corresponds to the size of a single audio sample in bytes.
  /// - `u8`: 1 byte
  /// - `s16`: 2 bytes
  /// - `s24`: 3 bytes
  /// - `s32` and `f32`: 4 bytes
  /// - `unknown`: 0 bytes
  int get bps {
    switch (this) {
      case u8:
        return 1;
      case s16:
        return 2;
      case s24:
        return 3;
      case s32 || f32:
        return 4;
      case _:
        return 0;
    }
  }
}
