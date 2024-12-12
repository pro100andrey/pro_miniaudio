part of '../library.dart';

/// Configuration class for playback device settings.
///
/// The `PlaybackConfig` class encapsulates configuration details for
/// initializing and managing playback devices. It includes properties
/// like the number of channels, sample rate, sample format, and ring
/// buffer thresholds.
class PlaybackConfig extends Equatable {
  /// Creates a new [PlaybackConfig] instance with the specified properties.
  ///
  /// - [channels]: The number of audio channels (e.g., 1 for mono, 2 for
  /// stereo).
  /// - [sampleRate]: The sample rate in Hertz (e.g., 44100 for CD-quality).
  /// - [sampleFormat]: The format of audio samples (e.g., 16-bit signed
  /// integer).
  /// - [ringBufferMaxThreshold]: The maximum threshold for the ring buffer
  ///   before the playback device starts consuming data.
  /// - [ringBufferMinThreshold]: The minimum threshold for the ring buffer
  ///   before the playback device can safely consume data.
  /// - [ringBufferSizeInBytes]: The total size of the ring buffer in bytes.
  const PlaybackConfig({
    required this.channels,
    required this.sampleRate,
    required this.sampleFormat,
    this.ringBufferMaxThreshold = 0,
    this.ringBufferMinThreshold = 0,
    this.ringBufferSizeInBytes = 0,
  });

  /// Creates a [PlaybackConfig] instance from an [AudioFormat].
  ///
  /// This factory method generates a playback configuration based on the
  /// provided [AudioFormat]. It calculates the appropriate buffer sizes
  /// and thresholds using standard values.
  ///
  /// - [format]: The audio format to base the configuration on.
  factory PlaybackConfig.withAudioFormat(AudioFormat format) {
    const sampleFormat = PcmFormat.s16; // Default sample format.
    final frameCount = format.sampleRate * 100 ~/ 1000; // Frames for 100 ms.
    final dataSizeInBytes = frameCount * sampleFormat.bps * format.channels;
    final bufferSizeInBytes = dataSizeInBytes * 5; // Total buffer size.

    return PlaybackConfig(
      channels: format.channels,
      sampleRate: format.sampleRate,
      sampleFormat: sampleFormat,
      ringBufferMaxThreshold: bufferSizeInBytes - (frameCount * 2),
      ringBufferMinThreshold: frameCount * 2,
      ringBufferSizeInBytes: bufferSizeInBytes,
    );
  }

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
  final PcmFormat sampleFormat;

  /// The maximum threshold for the ring buffer.
  ///
  /// This value defines when the playback device should start consuming
  /// data from the buffer to avoid overflows.
  final int ringBufferMaxThreshold;

  /// The minimum threshold for the ring buffer.
  ///
  /// This value defines the minimum amount of data that should be available
  /// in the buffer before the playback device starts processing it.
  final int ringBufferMinThreshold;

  /// The total size of the ring buffer in bytes.
  ///
  /// This value determines how much audio data can be stored in the buffer
  /// at any given time.
  final int ringBufferSizeInBytes;

  /// Calculates the number of bytes per audio frame.
  ///
  /// An audio frame consists of one sample per channel. This property
  /// computes the size of one frame based on the sample format and the
  /// number of channels.
  ///
  /// ### Example:
  /// ```dart
  /// final config = PlaybackConfig(
  ///   channels: 2,
  ///   sampleRate: 44100,
  ///   sampleFormat: PcmFormat.s16,
  /// );
  /// print('Bytes Per Frame: ${config.bpf}');
  /// ```
  int get bpf => sampleFormat.bps * channels;

  @override
  List<Object?> get props => [
        channels,
        sampleRate,
        sampleFormat,
        ringBufferMaxThreshold,
        ringBufferMinThreshold,
        ringBufferSizeInBytes,
      ];
}
