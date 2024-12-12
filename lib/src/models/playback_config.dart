part of '../library.dart';

class PlaybackConfig extends Equatable {
  const PlaybackConfig({
    required this.channels,
    required this.sampleRate,
    required this.sampleFormat,
    this.ringBufferMaxThreshold = 0,
    this.ringBufferMinThreshold = 0,
    this.ringBufferSizeInBytes = 0,
  });

  factory PlaybackConfig.withAudioFormat(AudioFormat format) {
    const sampleFormat = SampleFormat.s16;
    final frameCount = format.sampleRate * 100 ~/ 1000;
    final dataSizeInBytes = frameCount * sampleFormat.bps * format.channels;
    final bufferSizeInBytes = dataSizeInBytes * 5;
    // final bufferSizeInSeconds =
    //     bufferSizeInBytes / format.sampleRate / sampleFormat.bps;

    return PlaybackConfig(
      channels: format.channels,
      sampleRate: format.sampleRate,
      sampleFormat: sampleFormat,
      ringBufferMaxThreshold: bufferSizeInBytes - (frameCount * 2),
      ringBufferMinThreshold: frameCount * 2,
      ringBufferSizeInBytes: bufferSizeInBytes,
    );
  }

  final int channels;
  final int sampleRate;
  final SampleFormat sampleFormat;
  final int ringBufferMaxThreshold;
  final int ringBufferMinThreshold;
  final int ringBufferSizeInBytes;

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
