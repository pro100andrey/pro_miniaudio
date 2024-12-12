part of '../library.dart';

/// Base class for waveform configuration.
///
/// This class defines the common properties and behavior for all waveform
/// types, such as the audio sample format, sample rate, number of channels,
/// and amplitude.
///
/// ### Example Usage:
/// ```dart
/// final config = WaveformSineConfig(
///   sampleFormat: PcmFormat.f32,
///   sampleRate: 44100,
///   channels: 2,
///   amplitude: 1.0,
///   frequency: 440.0,
/// );
///
/// print('Waveform type: ${config.type.name}');
/// print('Bytes per frame: ${config.bpf}');
/// ```
sealed class BaseWaveformConfig extends Equatable {
  /// Creates a waveform configuration with the given properties.
  ///
  /// - [sampleFormat]: The audio sample format.
  /// - [sampleRate]: The sample rate in Hertz.
  /// - [channels]: The number of audio channels.
  /// - [amplitude]: The amplitude of the waveform.
  const BaseWaveformConfig({
    required this.sampleFormat,
    required this.sampleRate,
    required this.channels,
    required this.amplitude,
  });

  /// The audio sample format.
  ///
  /// Examples:
  /// - `PcmFormat.s16`: 16-bit signed integer.
  /// - `PcmFormat.f32`: 32-bit floating-point.
  final PcmFormat sampleFormat;

  /// The sample rate in Hertz.
  ///
  /// Examples:
  /// - `44100`: CD-quality audio.
  /// - `48000`: Professional audio.
  final int sampleRate;

  /// The number of audio channels.
  ///
  /// Examples:
  /// - `1`: Mono
  /// - `2`: Stereo
  /// - `6`: Surround sound (5.1).
  final int channels;

  /// The amplitude of the waveform.
  ///
  /// This value determines the loudness of the waveform. It typically ranges
  /// between `0.0` (silent) and `1.0` (full volume).
  final double amplitude;

  /// The type of waveform.
  ///
  /// This property is overridden in derived classes to specify the waveform
  /// type.
  WaveformType get type;

  /// The number of bytes per audio frame.
  ///
  /// An audio frame consists of one sample per channel. This property
  /// calculates the size of one frame based on the sample format and
  /// the number of channels.
  ///
  /// ### Example:
  /// ```dart
  /// print('Bytes per frame: ${config.bpf}');
  /// ```
  int get bpf => sampleFormat.bps * channels;

  @override
  List<Object?> get props => [
        sampleFormat,
        amplitude,
        sampleRate,
        channels,
        type,
      ];
}

/// Configuration for a sine waveform.
///
/// This class defines the properties of a sine wave, including its frequency.
///
/// ### Example Usage:
/// ```dart
/// final sineConfig = WaveformSineConfig(
///   sampleFormat: PcmFormat.f32,
///   sampleRate: 44100,
///   channels: 2,
///   amplitude: 1.0,
///   frequency: 440.0,
/// );
/// print('Sine wave frequency: ${sineConfig.frequency} Hz');
/// ```
final class WaveformSineConfig extends BaseWaveformConfig {
  /// Creates a sine waveform configuration with the given properties.
  const WaveformSineConfig({
    required super.sampleFormat,
    required super.sampleRate,
    required super.channels,
    required super.amplitude,
    required this.frequency,
  });

  /// The frequency of the sine wave in Hertz.
  ///
  /// Examples:
  /// - `440.0`: Standard tuning pitch (A4).
  /// - `880.0`: One octave higher.
  final double frequency;

  @override
  WaveformType get type => WaveformType.sine;

  @override
  List<Object?> get props => super.props + [frequency];
}

/// Configuration for a square waveform.
///
/// This class defines the properties of a square wave, including its frequency.
///
/// ### Example Usage:
/// ```dart
/// final squareConfig = WaveformSquareConfig(
///   sampleFormat: PcmFormat.s16,
///   sampleRate: 48000,
///   channels: 1,
///   amplitude: 0.8,
///   frequency: 440.0,
/// );
/// print('Square wave frequency: ${squareConfig.frequency} Hz');
/// ```
final class WaveformSquareConfig extends BaseWaveformConfig {
  /// Creates a square waveform configuration with the given properties.
  const WaveformSquareConfig({
    required super.sampleFormat,
    required super.sampleRate,
    required super.channels,
    required super.amplitude,
    required this.frequency,
  });

  /// The frequency of the square wave in Hertz.
  final double frequency;

  @override
  WaveformType get type => WaveformType.square;

  @override
  List<Object?> get props => super.props + [frequency];
}

/// Configuration for a triangle waveform.
///
/// This class defines the properties of a triangle wave, including its f
/// requency.
///
/// ### Example Usage:
/// ```dart
/// final triangleConfig = WaveformTriangleConfig(
///   sampleFormat: PcmFormat.s16,
///   sampleRate: 44100,
///   channels: 2,
///   amplitude: 0.5,
///   frequency: 660.0,
/// );
/// print('Triangle wave frequency: ${triangleConfig.frequency} Hz');
/// ```
final class WaveformTriangleConfig extends BaseWaveformConfig {
  /// Creates a triangle waveform configuration with the given properties.
  const WaveformTriangleConfig({
    required super.sampleFormat,
    required super.sampleRate,
    required super.channels,
    required super.amplitude,
    required this.frequency,
  });

  /// The frequency of the triangle wave in Hertz.
  final double frequency;

  @override
  WaveformType get type => WaveformType.triangle;

  @override
  List<Object?> get props => super.props + [frequency];
}

/// Configuration for a sawtooth waveform.
///
/// This class defines the properties of a sawtooth wave, including its
/// frequency.
///
/// ### Example Usage:
/// ```dart
/// final sawtoothConfig = WaveformSawtoothConfig(
///   sampleFormat: PcmFormat.f32,
///   sampleRate: 48000,
///   channels: 1,
///   amplitude: 1.0,
///   frequency: 550.0,
/// );
/// print('Sawtooth wave frequency: ${sawtoothConfig.frequency} Hz');
/// ```
final class WaveformSawtoothConfig extends BaseWaveformConfig {
  /// Creates a sawtooth waveform configuration with the given properties.
  const WaveformSawtoothConfig({
    required super.sampleFormat,
    required super.sampleRate,
    required super.channels,
    required super.amplitude,
    required this.frequency,
  });

  /// The frequency of the sawtooth wave in Hertz.
  final double frequency;

  @override
  WaveformType get type => WaveformType.sawtooth;

  @override
  List<Object?> get props => super.props + [frequency];
}
