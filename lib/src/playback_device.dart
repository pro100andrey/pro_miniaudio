part of 'library.dart';

/// A class for managing audio playback devices.
///
/// The `PlaybackDevice` class provides methods for controlling audio playback,
/// such as starting and stopping playback, resetting the internal buffer,
/// and pushing audio buffers to the device. It integrates with the native
/// audio system to ensure efficient playback.
///
/// ### Example Usage:
/// ```dart
/// final context = AudioContext();
/// final devices = context.playbackDeviceInfos;
/// final device = devices.first; // Select the first playback device.
///
/// final config = PlaybackConfig.withAudioFormat(
///   AudioFormat(
///     sampleFormat: PcmFormat.s16,
///     channels: 2,
///     sampleRate: 44100,
///   ),
/// );
///
/// final playbackDevice = PlaybackDevice(
///   context: context,
///   deviceInfo: device,
///   config: config,
/// );
///
/// playbackDevice.start();
/// playbackDevice.pushBuffer(
///   buffer: Float32List.fromList([0.0, 0.5, -0.5, 1.0]),
///   framesCount: 2,
/// );
/// playbackDevice.stop();
/// playbackDevice.dispose();
/// ```
final class PlaybackDevice extends ManagedResource<Void> {
  /// Creates a new playback device instance.
  ///
  /// - [context]: The [AudioContext] that manages this device.
  /// - [id]: The unique identifier of the playback device. If `null`,
  ///   the default playback device will be used.
  /// - [config]: The playback configuration specifying sample format,
  ///   channels, sample rate, and buffer properties.
  ///
  /// Throws:
  /// - [StateError] if the [AudioContext] is not initialized.
  /// - [Exception] if the device creation fails.
  factory PlaybackDevice({
    required AudioContext context,
    required DeviceId? id,
    required PlaybackConfig config,
  }) {
    final nativeConfig = config.toNative();
    final pContext = context.ensureIsNotFinalized();

    final device = _bindings.playback_device_create(
      pContext,
      id == null ? nullptr : id._resource,
      nativeConfig.ensureIsNotFinalized().ref,
    );

    if (device == nullptr) {
      throw Exception('Failed to create playback device');
    }

    return PlaybackDevice._(
      device,
      context: context,
      config: config,
      id: id,
    );
  }

  /// Internal constructor.
  PlaybackDevice._(
    super.ptr, {
    required this.context,
    required this.config,
    required this.id,
  }) : super._();

  /// The playback configuration for this device.
  final PlaybackConfig config;

  /// Information about the playback device.
  final DeviceId? id;

  /// The [AudioContext] that manages this device.
  final AudioContext context;

  @protected
  @override
  void releaseResource() => _bindings.playback_device_destroy(
        ensureIsNotFinalized(),
      );

  /// Resets the internal buffer of the playback device.
  ///
  /// This clears any audio data currently in the buffer and prepares the device
  /// for new data.
  ///
  /// Throws:
  /// - [StateError] if the device is finalized or not initialized.
  void resetBuffer() => _bindings.playback_device_reset_buffer(
        ensureIsNotFinalized(),
      );

  /// Starts audio playback.
  ///
  /// Throws:
  /// - [StateError] if the device is finalized or not initialized.
  void start() => _bindings.playback_device_start(
        ensureIsNotFinalized(),
      );

  /// Stops audio playback.
  ///
  /// This halts ongoing playback and prepares the device for further
  /// operations.
  ///
  /// Throws:
  /// - [StateError] if the device is finalized or not initialized.
  void stop() => _bindings.playback_device_stop(
        ensureIsNotFinalized(),
      );

  /// Gets the current state of the playback device.
  ///
  /// Returns:
  /// - The current [DeviceState].
  ///
  /// Throws:
  /// - [StateError] if the device is finalized or not initialized.
  DeviceState get state {
    final state = _bindings.playback_device_get_state(
      ensureIsNotFinalized(),
    );

    return DeviceState.values[state.index];
  }

  /// Pushes an audio buffer to the playback device.
  ///
  /// - [buffer]: A [TypedData] containing the audio samples. Supported types
  ///   include [Float32List], [Int16List], [Uint8List], and [Int32List].
  /// - [framesCount]: The number of frames in the buffer. Each frame contains
  ///   samples for all channels.
  ///
  /// Throws:
  /// - [StateError] if the device is finalized or not initialized.
  /// - [AssertionError] if the buffer type or frame count is invalid.
  ///
  /// Example:
  /// ```dart
  /// playbackDevice.pushBuffer(
  ///   buffer: Float32List.fromList([0.0, 0.5, -0.5, 1.0]),
  ///   framesCount: 2,
  /// );
  /// ```
  void pushBuffer<T extends TypedData>({
    required TypedData buffer,
    required int framesCount,
  }) {
    assert(framesCount > 0, 'Frames count must be greater than 0');
    assert(
      buffer is Float32List ||
          buffer is Int16List ||
          buffer is Uint8List ||
          buffer is Int32List,
      'Unsupported buffer type',
    );

    final resource = ensureIsNotFinalized();
    final data = malloc.allocate<playback_data_t>(sizeOf<playback_data_t>());
    final sizeInBytes = framesCount * config.bpf;
    final pUserData = malloc.allocate(sizeInBytes);
    final length = framesCount * config.channels;

    // Copy audio data into the allocated memory.
    switch (buffer) {
      case Uint8List():
        pUserData.cast<Uint8>().asTypedList(length).setAll(0, buffer);
      case Int16List():
        pUserData.cast<Int16>().asTypedList(length).setAll(0, buffer);
      case Float32List():
        pUserData.cast<Float>().asTypedList(length).setAll(0, buffer);
      case Int32List():
        pUserData.cast<Int32>().asTypedList(length).setAll(0, buffer);
      case _:
        throw UnsupportedError('Unsupported buffer type');
    }

    // Set playback data properties.
    data.ref.pUserData = pUserData.cast();
    data.ref.sizeInBytes = sizeInBytes;

    _bindings.playback_device_push_buffer(resource, data);

    // Free allocated memory.
    malloc
      ..free(data)
      ..free(pUserData);
  }
}
