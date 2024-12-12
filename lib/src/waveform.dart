part of 'library.dart';

/// A class for generating and managing audio waveforms.
///
/// The `Waveform` class allows you to create and manipulate various types of
/// audio waveforms (sine, square, sawtooth, triangle) using a flexible API.
/// It is particularly useful for testing audio pipelines or generating
/// procedural audio signals.
///
/// ### Example Usage:
/// ```dart
/// final waveform = Waveform(
///   config: WaveformSineConfig(
///     sampleFormat: PcmFormat.f32,
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
  /// - [config]: A configuration object (e.g., [WaveformSineConfig]) that
  ///   defines the waveform's properties, such as type, format, sample rate,
  ///   channels, amplitude, and frequency.
  ///
  /// Throws:
  /// - [Exception] if the waveform creation fails.
  ///
  /// ### Example:
  /// ```dart
  /// final waveform = Waveform(
  ///   config: WaveformSineConfig(
  ///     sampleFormat: PcmFormat.f32,
  ///     sampleRate: 44100,
  ///     channels: 2,
  ///     amplitude: 1.0,
  ///     frequency: 440.0,
  ///   ),
  /// );
  /// ```
  factory Waveform({required BaseWaveformConfig config}) {
    final rWaveform = switch (config) {
      WaveformSineConfig() => _bindings.waveform_create(
          config.sampleFormat.toNative(),
          config.channels,
          config.sampleRate,
          config.type.toNative(),
          config.amplitude,
          config.frequency,
        ),
      WaveformSquareConfig() => _bindings.waveform_create(
          config.sampleFormat.toNative(),
          config.channels,
          config.sampleRate,
          config.type.toNative(),
          config.amplitude,
          config.frequency,
        ),
      WaveformSawtoothConfig() => _bindings.waveform_create(
          config.sampleFormat.toNative(),
          config.channels,
          config.sampleRate,
          config.type.toNative(),
          config.amplitude,
          config.frequency,
        ),
      WaveformTriangleConfig() => _bindings.waveform_create(
          config.sampleFormat.toNative(),
          config.channels,
          config.sampleRate,
          config.type.toNative(),
          config.amplitude,
          config.frequency,
        ),
    };

    if (rWaveform == nullptr) {
      throw Exception('Failed to create waveform');
    }

    return Waveform._(rWaveform, config);
  }

  /// Internal constructor.
  ///
  /// This is used internally by the factory constructor and should not
  /// be called directly.
  Waveform._(super.ptr, this.config) : super._();

  /// The waveform configuration used for generating PCM frames.
  final BaseWaveformConfig config;

  @protected
  @override
  NativeFinalizer get finalizer => Library.waveformFinalizer;

  @protected
  @override
  void releaseResource() => _bindings.waveform_destroy(
        ensureIsNotFinalized(),
      );

  /// Reads PCM frames from the waveform generator.
  ///
  /// This method retrieves audio samples generated by the waveform and
  /// returns the PCM data along with the number of frames read.
  ///
  /// - [frameCount]: The number of frames to read from the waveform generator.
  ///
  /// Returns:
  /// - A record containing:
  ///   - `frames`: The PCM audio data as a [TypedData].
  ///   - `framesRead`: The number of frames successfully read.
  ///
  /// Throws:
  /// - [StateError] if the waveform is finalized or uninitialized.
  /// - [Exception] if the sample format is unsupported.
  ///
  /// ### Example:
  /// ```dart
  /// final pcmFrames = waveform.readWaveformPcmFrames(frameCount: 1024);
  /// print('Read ${pcmFrames.framesRead} frames');
  /// ```
  ({TypedData frames, int framesRead}) readWaveformPcmFrames({
    required int frameCount,
  }) {
    final resource = ensureIsNotFinalized();

    // Allocate memory for PCM output and frames read count.
    final pFramesOut = malloc.allocate(frameCount * config.bpf);
    final pFramesRead = malloc<Uint64>();

    try {
      _bindings.waveform_read_pcm_frames_with_buffer(
        resource,
        pFramesOut.cast(),
        frameCount,
        pFramesRead,
      );

      // Retrieve the number of frames read and calculate buffer size.
      final framesRead = pFramesRead.value;
      final size = framesRead * config.channels;

      // Convert native buffer to Dart TypedData based on sample format.
      final frames = switch (config.sampleFormat) {
        PcmFormat.f32 => pFramesOut
            .cast<Float>()
            .asTypedList(size, finalizer: malloc.nativeFree),
        PcmFormat.s16 => pFramesOut
            .cast<Int16>()
            .asTypedList(size, finalizer: malloc.nativeFree),
        PcmFormat.s32 || PcmFormat.s24 => pFramesOut
            .cast<Int32>()
            .asTypedList(size, finalizer: malloc.nativeFree),
        PcmFormat.u8 => pFramesOut
            .cast<Uint8>()
            .asTypedList(size, finalizer: malloc.nativeFree),
        _ =>
          throw Exception('Unsupported sample format: ${config.sampleFormat}'),
      };

      return (frames: frames, framesRead: framesRead);
    } finally {
      // Free memory allocated for the frames count.
      malloc.free(pFramesRead);
    }
  }
}
