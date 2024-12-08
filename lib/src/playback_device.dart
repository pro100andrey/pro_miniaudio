part of 'library.dart';

/// A class for managing audio playback devices.
///
/// The `PlaybackDevice` class provides methods to control audio playback,
/// such as starting playback, pushing audio buffers, stopping playback,
/// and releasing resources.
final class PlaybackDevice extends NativeResource<Void> {
  /// Creates a new playback device instance.
  ///
  /// - [deviceId]: The unique identifier of the playback device.
  /// - [context]: The [Context] associated with this device.
  /// - [bufferSizeInBytes]: The size of the audio buffer in bytes.
  /// - [format]: The audio format that this device will use for playback.
  ///
  /// Throws an exception if the [Context] is not initialized or
  /// if the device creation fails.
  factory PlaybackDevice({
    required Object deviceId,
    required Context context,
    required int bufferSizeInBytes,
    required SupportedFormat format,
  }) {
    if (!context.isFinalized) {
      throw Exception('Audio context is finalized');
    }

    final result = _bindings.playback_device_create(
      context._resource,
      bufferSizeInBytes,
      deviceId as device_id_t,
      format.nativeFormat,
    );

    final playbackDevice = PlaybackDevice._(result) //
      .._supportedFormat = format;

    return playbackDevice;
  }

  /// Internal constructor.
  PlaybackDevice._(super.ptr) : super._();

  /// The audio format supported by this playback device.
  ///
  /// This format defines the sample rate, channels, and data representation.
  late SupportedFormat _supportedFormat;

  static final _finalizer = NativeFinalizer(
    _bindings.addresses.playback_device_destroy.cast(),
  );

  @override
  NativeFinalizer get finalizer => _finalizer;

  @override
  void releaseResource() {
    _bindings.playback_device_destroy(_resource);
  }

  /// Starts audio playback on the device.
  ///
  /// Throws an exception if the operation fails or if the device is not
  /// properly initialized.
  void start() {
    _bindings.playback_device_start(_resource);
  }

  /// Pushes an audio buffer to the playback device for playback.
  ///
  /// - [buffer]: A [Float32List] containing the audio samples.
  /// - [framesCount]: The number of frames in the buffer.
  ///
  /// Each frame contains samples for all channels. For example, in a stereo
  /// configuration (2 channels), one frame includes 2 samples.
  ///
  /// Throws an exception if the operation fails or if the buffer cannot be
  /// processed.
  void pushBuffer({required Float32List buffer, required int framesCount}) {
    final data = malloc.allocate<playback_data_t>(sizeOf<playback_data_t>());

    final sizeInBytes = framesCount * _supportedFormat.bytesPerFrame;

    final pUserData = malloc<Float>(sizeInBytes);
    // Fill the allocated memory with audio buffer data.
    pUserData
        .asTypedList(framesCount * _supportedFormat.channels)
        .setAll(0, buffer);

    // Set data properties for the native structure.
    data.ref.pUserData = pUserData.cast();
    data.ref.sizeInBytes = sizeInBytes;
    data.ref.formatAsInt = _supportedFormat.format.value;

    _bindings.playback_device_push_buffer(_resource, data);

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
  /// Throws an exception if the operation fails or if the device is not
  /// properly initialized.
  void stop() {
    _bindings.playback_device_stop(_resource);
  }
}
