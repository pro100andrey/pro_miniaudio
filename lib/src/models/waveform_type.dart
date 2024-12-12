part of '../library.dart';

/// Enum representing different types of waveforms.
///
/// Waveforms are fundamental building blocks in audio synthesis, defining
/// the shape of an audio signal. This enum is used to select or identify
/// a waveform type in audio-related operations.
///
/// ### Available Waveform Types
/// - [sine]: A smooth periodic wave, commonly used for pure tones.
/// - [square]: A waveform with abrupt transitions, often used for
///   digital signals or sound effects.
/// - [triangle]: A periodic waveform with linear slopes, producing a
///   softer sound than square waves.
/// - [sawtooth]: A waveform with a ramp-like shape, used for complex
///   harmonic tones.
enum WaveformType {
  /// A sine wave.
  ///
  /// A smooth periodic wave, often used for generating pure tones.
  sine(0),

  /// A square wave.
  ///
  /// A waveform with abrupt transitions between high and low values,
  /// commonly used in digital sound effects.
  square(1),

  /// A triangle wave.
  ///
  /// A waveform with a linear slope, producing a softer sound than
  /// square waves, often used in synthesizers.
  triangle(2),

  /// A sawtooth wave.
  ///
  /// A waveform with a ramp-like shape, containing strong harmonic content,
  /// used in electronic music and sound synthesis.
  sawtooth(3);

  /// Creates a [WaveformType] with the associated integer value.
  ///
  /// Each waveform type corresponds to a unique integer value, making it
  /// compatible with native libraries or APIs.
  const WaveformType(this.value);

  /// The integer value representing the waveform type.
  ///
  /// This value is primarily used for serialization or communication
  /// with native APIs.
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

  /// Returns the name of the waveform type as a string.
  String get name {
    switch (this) {
      case sine:
        return 'sine';
      case square:
        return 'square';
      case triangle:
        return 'triangle';
      case sawtooth:
        return 'sawtooth';
    }
  }
}
