part of 'library.dart';

/// A class for managing audio playback devices.
///
/// The `PlaybackDevice` class provides methods to control audio playback,
/// such as starting playback, pushing audio buffers, stopping playback,
/// and releasing resources.
final class PlaybackDevice extends NativeResource<Void> with EquatableMixin {
  /// Creates a new playback device instance.
  ///
  /// - [deviceInfo]: The device information for the playback device.
  /// - [audioFormat]: The audio format that this device will use for
  /// - [bufferSizeInBytes]: The size of the audio buffer in bytes.
  /// playback.
  ///
  /// Throws an exception if the [Context] is not initialized or
  /// if the device creation fails.
  factory PlaybackDevice({
    required DeviceInfo deviceInfo,
    required AudioFormat audioFormat,
    required int bufferSizeInBytes,
  }) {

    final nativeAudioFormat = audioFormat.toNative();

    final device = _bindings.playback_device_create(
      deviceInfo.id as device_id_t,
      nativeAudioFormat.ref,
      bufferSizeInBytes,
    );

    malloc.free(nativeAudioFormat);

    return PlaybackDevice._(
      device,
      deviceInfo: deviceInfo,
      audioFormat: audioFormat,
      bufferSizeInBytes: bufferSizeInBytes,
    );
  }

  /// Internal constructor.
  PlaybackDevice._(
    super.ptr, {
    required this.deviceInfo,
    required this.audioFormat,
    required this.bufferSizeInBytes,
  }) : super._();

  /// The audio format supported by this playback device.
  ///
  /// This format defines the sample rate, channels, and data representation.
  final AudioFormat audioFormat;

  /// The size of the audio buffer in bytes.
  final int bufferSizeInBytes;

  /// Device information.
  final DeviceInfo deviceInfo;

  /// The finalizer for the playback device.
  ///
  /// Releases the native resource when the Dart object is garbage collected.
  static final _finalizer = NativeFinalizer(
    _bindings.addresses.playback_device_destroy.cast(),
  );

  @override
  List<Object?> get props => [
        audioFormat,
        bufferSizeInBytes,
        deviceInfo,
      ];

  @override
  NativeFinalizer get finalizer => _finalizer;

  @override
  void releaseResource() => _bindings.playback_device_destroy(
        ensureResourceIsNotFinalized(),
      );

  /// Resets the playback device's internal buffer.
  ///
  /// Clears any audio data currently in the buffer and ensures that the buffer
  /// is ready for new data.
  ///
  /// Throws an exception if resource is not initialized.
  void resetBuffer() => _bindings.playback_device_reset_buffer(
        ensureResourceIsNotFinalized(),
      );

  /// Starts audio playback on the device.
  ///
  /// Throws an exception if resource is not initialized.
  void start() => _bindings.playback_device_start(
        ensureResourceIsNotFinalized(),
      );

  /// Gets the current state of the playback device.
  ///
  /// Throws an exception if resource is not initialized.
  DeviceState get state {
    final state = _bindings.playback_device_get_state(
      ensureResourceIsNotFinalized(),
    );

    return DeviceState.values[state.index];
  }

  /// Pushes an audio buffer to the playback device for playback.
  ///
  /// - [buffer]: A [Float32List] containing the audio samples.
  /// - [framesCount]: The number of frames in the buffer.
  ///
  /// Each frame contains samples for all channels. For example, in a stereo
  /// configuration (2 channels), one frame includes 2 samples.
  ///
  /// Throws an exception if resource is not initialized.
  void pushBuffer({required Float32List buffer, required int framesCount}) {
    final resource = ensureResourceIsNotFinalized();
    final data = malloc.allocate<playback_data_t>(sizeOf<playback_data_t>());
    
    final sizeInBytes = framesCount * audioFormat.bytesPerFrame;

    final pUserData = malloc<Float>(sizeInBytes);
    // Fill the allocated memory with audio buffer data.
    pUserData
        .asTypedList(framesCount * audioFormat.channels)
        .setAll(0, buffer);

    // Set data properties for the native structure.
    data.ref.pUserData = pUserData.cast();
    data.ref.sizeInBytes = sizeInBytes;

    _bindings.playback_device_push_buffer(resource, data);

    // Free allocated memory to avoid leaks.
    malloc
      ..free(pUserData)
      ..free(data);
  }

  /// Stops audio playback on the device.
  ///
  /// This method halts any ongoing playback and prepares the device
  /// for further operations if needed.
  ///
  /// Throws an exception if resource is not initialized.
  void stop() => _bindings.playback_device_stop(
        ensureResourceIsNotFinalized(),
      );
}
