part of '../library.dart';

/// Calculates the buffer size in bytes for a given duration and audio format.
int bufferSizeWith(int ms, AudioFormat format) {
  final frameCount = format.sampleRate * ms ~/ 1000;
  final dataSizeInBytes = frameCount * format.pcmFormat.bps * format.channels;

  return dataSizeInBytes;
}

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
  /// - [pcmFormat]: The format of audio samples (e.g., 16-bit signed
  /// integer).
  /// - [ringBufferMaxThreshold]: The maximum threshold for the ring buffer
  ///   before the playback device starts consuming data.
  /// - [ringBufferMinThreshold]: The minimum threshold for the ring buffer
  ///   before the playback device can safely consume data.
  /// - [ringBufferSizeInBytes]: The total size of the ring buffer in bytes.
  const PlaybackConfig({
    required this.channels,
    required this.sampleRate,
    required this.pcmFormat,
    required this.ringBufferMaxThreshold,
    required this.ringBufferMinThreshold,
    required this.ringBufferSizeInBytes,
  });

  /// Creates a [PlaybackConfig] instance from an [AudioFormat] based data
  /// estimated chunk duration.
  ///
  /// This factory method generates a playback configuration based on the
  /// provided [AudioFormat] and estimated chunk duration. It calculates the
  /// buffer size based on the format and chunk duration.
  ///
  /// - [format]: The audio format to base the configuration on.
  /// - [chunkMs]: The estimated duration of each chunk in milliseconds.
  factory PlaybackConfig.basedChunkDuration({
    required AudioFormat format,
    required int chunkMs,
  }) {
    final bufferSizeInBytes = bufferSizeWith(chunkMs, format) * 5;
    final chunkInFrames = format.sampleRate * chunkMs ~/ 1000;
    final framesInBytes =
        chunkInFrames * format.pcmFormat.bps * format.channels;

    return PlaybackConfig(
      channels: format.channels,
      sampleRate: format.sampleRate,
      pcmFormat: format.pcmFormat,
      ringBufferMaxThreshold: bufferSizeInBytes ~/ 2,
      ringBufferMinThreshold: framesInBytes,
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
  final PcmFormat pcmFormat;

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
  int get bpf => pcmFormat.bps * channels;

  @override
  List<Object?> get props => [
        channels,
        sampleRate,
        pcmFormat,
        ringBufferMaxThreshold,
        ringBufferMinThreshold,
        ringBufferSizeInBytes,
      ];
}
