part of '../library.dart';

/// Represents a supported audio data format.
///
/// The `AudioFormat` class encapsulates information about the properties
/// of a specific audio data format, such as the sample format, the number
/// of channels, and the sample rate.
///
/// This class is typically used to describe the capabilities of audio devices
/// or to configure audio processing pipelines.
final class AudioFormat extends Equatable {
  /// Creates a new [AudioFormat] instance with the given properties.
  ///
  /// - [pcmFormat]: The audio sample format (e.g., 16-bit signed integer).
  /// - [channels]: The number of audio channels (e.g., 1 for mono, 2 for
  /// stereo).
  /// - [sampleRate]: The sample rate in Hertz (e.g., 44100 for CD-quality).
  const AudioFormat({
    required this.pcmFormat,
    required this.channels,
    required this.sampleRate,
  });

  /// The audio sample format.
  ///
  /// This property defines how each audio sample is represented in memory.
  /// Examples include:
  /// - `PcmFormat.s16`: 16-bit signed integer.
  /// - `PcmFormat.f32`: 32-bit floating-point.
  final PcmFormat pcmFormat;

  /// The number of audio channels.
  ///
  /// Common configurations include:
  /// - `1`: Mono
  /// - `2`: Stereo
  /// - `6`: 5.1 Surround
  final int channels;

  /// The sample rate in Hertz.
  ///
  /// This property defines the number of audio samples per second for each
  /// channel.
  /// Common sample rates include:
  /// - `44100`: CD-quality audio.
  /// - `48000`: Used in professional audio and video production.
  /// - `96000`: High-resolution audio.
  final int sampleRate;

  /// The number of bytes per audio frame.
  ///
  /// An audio frame is a single sample across all channels. This property
  /// calculates the size of one frame based on the sample format and the
  /// number of channels.
  int get bytesPerFrame => pcmFormat.bps * channels;

  @override
  List<Object?> get props => [
        pcmFormat,
        channels,
        sampleRate,
      ];
}
