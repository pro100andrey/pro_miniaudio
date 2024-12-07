part of 'library.dart';

/// A class for generating and managing waveforms.
///
/// The `Waveform` class provides an interface for creating various types of
/// audio waveforms (sine, square, sawtooth, triangle) and reading PCM frames
/// generated by the waveform.
///
/// Example usage:
/// ```dart
/// final waveform = Waveform(
///   config: WaveformSineConfig(
///     format: SampleFormat.f32,
///     sampleRate: 44100,
///     channels: 2,
///     amplitude: 1.0,
///     frequency: 440.0,
///   ),
/// );
///
/// final pcmFrames = waveform.readWaveformPcmFrames(frameCount: 1024);
/// print('Read ${pcmFrames.framesRead} frames');
///
/// waveform.dispose();
/// ```
final class Waveform extends NativeResource<Void> {
  /// Creates a new waveform instance based on the provided configuration.
  ///
  /// - [config]: A configuration object that defines the waveform type,
  ///   format, sample rate, channels, amplitude, and frequency.
  ///
  /// Throws an exception if the waveform creation fails.
  ///
  /// Example:
  /// ```dart
  /// final waveform = Waveform(
  ///   config: WaveformSineConfig(
  ///     format: SampleFormat.f32,
  ///     sampleRate: 44100,
  ///     channels: 2,
  ///     amplitude: 1.0,
  ///     frequency: 440.0,
  ///   ),
  /// );
  /// ```
  factory Waveform({required BaseWaveformConfig config}) {
    final cfg = config;

    final rWaveform = switch (cfg) {
      WaveformSineConfig() => _bindings.waveform_create(
          cfg.format.toNative(),
          cfg.channels,
          cfg.sampleRate,
          cfg.type.toNative(),
          cfg.amplitude,
          cfg.frequency,
        ),
      WaveformSquareConfig() => _bindings.waveform_create(
          cfg.format.toNative(),
          cfg.channels,
          cfg.sampleRate,
          cfg.type.toNative(),
          cfg.amplitude,
          cfg.frequency,
        ),
      WaveformSawtoothConfig() => _bindings.waveform_create(
          cfg.format.toNative(),
          cfg.channels,
          cfg.sampleRate,
          cfg.type.toNative(),
          cfg.amplitude,
          cfg.frequency,
        ),
      WaveformTriangleConfig() => _bindings.waveform_create(
          cfg.format.toNative(),
          cfg.channels,
          cfg.sampleRate,
          cfg.type.toNative(),
          cfg.amplitude,
          cfg.frequency,
        ),
    };

    final result = FFResult<Void>(rWaveform)..throwIfError();

    final waveform = Waveform._(result.data)..config = cfg;

    return waveform;
  }

  /// Internal constructor.
  Waveform._(super.ptr) : super._();


  /// The waveform configuration used for generating PCM frames.
  late final BaseWaveformConfig config;

  /// The finalizer for the `Waveform` class.
  static final _finalizer = NativeFinalizer(
    _bindings.addresses.waveform_destroy.cast(),
  );

  @override
  NativeFinalizer get finalizer => _finalizer;

  @override
  void releaseResource() {
    _bindings.waveform_destroy(_resource);
  }

  /// Reads PCM frames from the waveform generator.
  ///
  /// - [frameCount]: The number of frames to read from the waveform.
  ///
  /// Returns:
  /// A Record containing the frames read and the number of
  /// frames successfully generated.
  ///
  /// Throws an exception if the waveform is not initialized or if the operation
  ///  fails.
  ///
  /// Example:
  /// ```dart
  /// final pcmFrames = waveform.readWaveformPcmFrames(frameCount: 1024);
  /// print('Read ${pcmFrames.framesRead} frames');
  /// ```
  ({Float32List frames, int framesRead}) readWaveformPcmFrames({
    required int frameCount,
  }) {
    final bytesPerFrame = _bindings.get_bytes_per_frame(
      config.format.toNative(),
      config.channels,
    );

    final pFramesOut = malloc.allocate<Void>(frameCount * bytesPerFrame);
    final pFramesRead = malloc.allocate<Uint64>(1);

    try {
      FFResult<Void>(
        _bindings.waveform_read_pcm_frames_with_buffer(
          _resource,
          pFramesOut,
          frameCount,
          pFramesRead,
        ),
      ).throwIfError();

      final framesRead = pFramesRead.value;
      final frames =
          pFramesOut.cast<Float>().asTypedList(framesRead * config.channels);
      final result = Float32List.fromList(frames);

      return (frames: result, framesRead: framesRead);
    } finally {
      malloc
        ..free(pFramesRead)
        ..free(pFramesOut);
    }
  }
}
