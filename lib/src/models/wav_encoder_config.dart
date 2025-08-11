part of '../library.dart';

class WavEncoderConfig extends Equatable {
  /// Creates a new [WavEncoderConfig] instance with the specified properties.
  ///
  /// - [channels]: The number of audio channels.
  /// - [sampleRate]: The sample rate in Hertz.
  /// - [pcmFormat]: The audio sample format.
  const WavEncoderConfig({
    required this.channels,
    required this.sampleRate,
    required this.pcmFormat,
  });

  /// Creates a new [WavEncoderConfig] instance from the specified
  /// [AudioFormat].
  factory WavEncoderConfig.fromAudioFormat(AudioFormat audioFormat) =>
      WavEncoderConfig(
        channels: audioFormat.channels,
        sampleRate: audioFormat.sampleRate,
        pcmFormat: audioFormat.pcmFormat,
      );

  /// The number of audio channels.
  ///
  /// Examples:
  /// - `1`: Mono
  /// - `2`: Stereo
  final int channels;

  /// The sample rate in Hertz.
  ///
  /// Examples:
  /// - `44100`: CD-quality audio.
  /// - `48000`: Professional audio production.
  final int sampleRate;

  /// The audio sample format.
  ///
  /// Examples:
  /// - `PcmFormat.s16`: 16-bit signed integer.
  /// - `PcmFormat.f32`: 32-bit floating point.
  final PcmFormat pcmFormat;

  @override
  List<Object?> get props => [channels, sampleRate, pcmFormat];
}
