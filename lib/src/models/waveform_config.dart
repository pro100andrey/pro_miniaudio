part of '../library.dart';

/// Base class for waveform configuration.
///
/// This class provides common properties and behavior for all waveform types,
/// including format, sample rate, channels, and amplitude.
sealed class BaseWaveformConfig extends Equatable {
  /// Creates a waveform configuration with the given properties.
  ///
  /// - [format]: The audio sample format.
  /// - [sampleRate]: The sample rate in Hertz.
  /// - [channels]: The number of audio channels.
  /// - [amplitude]: The amplitude of the waveform.
  const BaseWaveformConfig({
    required this.format,
    required this.sampleRate,
    required this.channels,
    required this.amplitude,
  });

  /// The audio sample format.
  final SampleFormat format;

  /// The sample rate in Hertz.
  final int sampleRate;

  /// The number of audio channels.
  final int channels;

  /// The amplitude of the waveform.
  final double amplitude;

  /// The type of waveform.
  WaveformType get type;

  @override
  List<Object?> get props => [format, amplitude, sampleRate, channels];
}

/// Configuration for a sine waveform.
///
/// This class defines the properties of a sine wave, including frequency.
final class WaveformSineConfig extends BaseWaveformConfig {
  /// Creates a sine waveform configuration with the given properties.
  ///
  /// - [format]: The audio sample format.
  /// - [sampleRate]: The sample rate in Hertz.
  /// - [channels]: The number of audio channels.
  /// - [amplitude]: The amplitude of the sine wave.
  /// - [frequency]: The frequency of the sine wave in Hertz.
  const WaveformSineConfig({
    required super.format,
    required super.sampleRate,
    required super.channels,
    required super.amplitude,
    required this.frequency,
  });

  /// The frequency of the sine wave in Hertz.
  final double frequency;

  @override
  WaveformType get type => WaveformType.sine;

  @override
  List<Object?> get props => super.props + [frequency];
}

/// Configuration for a square waveform.
///
/// This class defines the properties of a square wave, including frequency.
final class WaveformSquareConfig extends BaseWaveformConfig {
  /// Creates a square waveform configuration with the given properties.
  ///
  /// - [format]: The audio sample format.
  /// - [sampleRate]: The sample rate in Hertz.
  /// - [channels]: The number of audio channels.
  /// - [amplitude]: The amplitude of the square wave.
  /// - [frequency]: The frequency of the square wave in Hertz.
  const WaveformSquareConfig({
    required super.format,
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
/// This class defines the properties of a triangle wave, including frequency.
final class WaveformTriangleConfig extends BaseWaveformConfig {
  /// Creates a triangle waveform configuration with the given properties.
  ///
  /// - [format]: The audio sample format.
  /// - [sampleRate]: The sample rate in Hertz.
  /// - [channels]: The number of audio channels.
  /// - [amplitude]: The amplitude of the triangle wave.
  /// - [frequency]: The frequency of the triangle wave in Hertz.
  const WaveformTriangleConfig({
    required super.format,
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
/// This class defines the properties of a sawtooth wave, including frequency.
final class WaveformSawtoothConfig extends BaseWaveformConfig {
  /// Creates a sawtooth waveform configuration with the given properties.
  ///
  /// - [format]: The audio sample format.
  /// - [sampleRate]: The sample rate in Hertz.
  /// - [channels]: The number of audio channels.
  /// - [amplitude]: The amplitude of the sawtooth wave.
  /// - [frequency]: The frequency of the sawtooth wave in Hertz.
  const WaveformSawtoothConfig({
    required super.format,
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
