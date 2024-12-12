part of 'library.dart';

/// A class for managing audio playback devices.
///
/// The `PlaybackDevice` class provides methods to control audio playback,
/// such as starting playback, pushing audio buffers, stopping playback,
/// and releasing resources.
final class PlaybackDevice extends NativeResource<Void> {
  /// Creates a new playback device instance.
  ///
  /// - [context]: The audio context that will manage this device.
  /// - [deviceInfo]: Information about the device to create.
  /// - [config]: The audio format configuration for the device's audio stream.
  ///
  /// Throws an exception if the [Context] is not initialized or
  /// if the device creation fails.
  factory PlaybackDevice({
    required Context context,
    required DeviceInfo deviceInfo,
    required PlaybackConfig config,
  }) {
    final nativeConfig = config.toNative();

    final pContext = context.ensureIsNotFinalized();

    final device = _bindings.playback_device_create(
      pContext,
      deviceInfo.id as Pointer<Void>,
      nativeConfig.ensureIsNotFinalized().ref,
    );

    if (device == nullptr) {
      throw Exception('Failed to create playback device');
    }

    return PlaybackDevice._(
      device,
      context: context,
      config: config,
      deviceInfo: deviceInfo,
    );
  }

  /// Internal constructor.
  PlaybackDevice._(
    super.ptr, {
    required this.context,
    required this.config,
    required this.deviceInfo,
  }) : super._();

  /// The audio format that this device will use for playback.
  final PlaybackConfig config;

  /// Device information.
  final DeviceInfo deviceInfo;

  /// The audio context that manages this device.
  final Context context;

  @protected
  @override
  NativeFinalizer get finalizer => Library.playbackDeviceFinalizer;

  @protected
  @override
  void releaseResource() =>_bindings.playback_device_destroy(
      ensureIsNotFinalized(),
    );
  /// Resets the playback device's internal buffer.
  ///
  /// Clears any audio data currently in the buffer and ensures that the buffer
  /// is ready for new data.
  ///
  /// Throws an exception if resource is not initialized.
  void resetBuffer() => _bindings.playback_device_reset_buffer(
        ensureIsNotFinalized(),
      );

  /// Starts audio playback on the device.
  ///
  /// Throws an exception if resource is not initialized.
  void start() => _bindings.playback_device_start(
        ensureIsNotFinalized(),
      );

  /// Gets the current state of the playback device.
  ///
  /// Throws an exception if resource is not initialized.
  DeviceState get state {
    final state = _bindings.playback_device_get_state(
      ensureIsNotFinalized(),
    );

    return DeviceState.values[state.index];
  }

  /// Pushes an audio buffer to the playback device for playback.
  ///
  /// - [buffer]: A [Float32List] or [Int16List] containing the audio samples.
  /// - [framesCount]: The number of frames in the buffer.
  ///
  /// Each frame contains samples for all channels. For example, in a stereo
  /// configuration (2 channels), one frame includes 2 samples.
  ///
  /// Throws an exception if resource is not initialized.
  void pushBuffer<T extends TypedData>({
    required TypedData buffer,
    required int framesCount,
  }) {
    assert(framesCount > 0, 'Frames count must be greater than 0');
    assert(
      buffer is Float32List ||
          buffer is Int32List ||
          buffer is Int16List ||
          buffer is Uint8List,
      'Unsupported buffer type',
    );

    final resource = ensureIsNotFinalized();
    final data = malloc.allocate<playback_data_t>(sizeOf<playback_data_t>());

    final sizeInBytes = framesCount * config.bpf;
    final pUserData = malloc.allocate(sizeInBytes);
    final length = framesCount * config.channels;

    // Fill the allocated memory with audio buffer data.
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

    // Set data properties for the native structure.
    data.ref.pUserData = pUserData.cast();
    data.ref.sizeInBytes = sizeInBytes;

    _bindings.playback_device_push_buffer(resource, data);

    // Free allocated memory to avoid leaks.
    malloc
      ..free(data)
      ..free(pUserData);
  }

  /// Stops audio playback on the device.
  ///
  /// This method halts any ongoing playback and prepares the device
  /// for further operations if needed.
  ///
  /// Throws an exception if resource is not initialized.
  void stop() => _bindings.playback_device_stop(
        ensureIsNotFinalized(),
      );
}
