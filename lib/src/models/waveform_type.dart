part of '../library.dart';
/// Enum representing different types of waveforms.
///
/// Each waveform type corresponds to a unique integer value.
enum WaveformType {
  /// A sine wave.
  sine(0),

  /// A square wave.
  square(1),

  /// A triangle wave.
  triangle(2),

  /// A sawtooth wave.
  sawtooth(3);

  /// Creates a [WaveformType] with the associated integer value.
  const WaveformType(this.value);

  /// The integer value representing the waveform type.
  final int value;

  /// Returns the [WaveformType] corresponding to the given integer [value].
  ///
  /// Throws an [ArgumentError] if the value is invalid.
  static WaveformType fromValue(int value) {
    if (value < 0 || value >= WaveformType.values.length) {
      throw ArgumentError('Invalid WaveformType value: $value');
    }
    return WaveformType.values[value];
  }
}
