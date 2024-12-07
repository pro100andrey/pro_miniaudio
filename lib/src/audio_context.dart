part of 'library.dart';

/// A constant representing an empty result for device information.
///
/// This is used as a default or fallback value when no devices are available.
///
/// - playback: An empty list of playback devices.
/// - capture: An empty list of capture devices.
///
/// Example usage:
/// ```dart
/// final devices = kEmptyDevicesInfos;
/// print('Playback devices count: ${devices.playback.length}');
/// print('Capture devices count: ${devices.capture.length}');
/// ```
const kEmptyDevicesInfos = (playback: <DeviceInfo>[], capture: <DeviceInfo>[]);

/// A class for managing audio contexts and devices.
///
/// The `AudioContext` is responsible for initializing the audio system,
/// retrieving information about playback and capture devices, and managing
/// the resources associated with the audio context.
///
/// Example:
/// ```dart
/// final audioContext = AudioContext();
/// final devices = audioContext.refreshAndReturnDevices();
/// print('Playback Devices: ${devices.playback.length}');
/// print('Capture Devices: ${devices.capture.length}');
/// audioContext.dispose();
/// ```
final class AudioContext extends NativeResource<Void> {
  factory AudioContext() {
    final result = FFResult<Void>(_bindings.audio_context_create())
      ..throwIfError();

    return AudioContext._(result.data);
  }

  /// Internal constructor.
  AudioContext._(super.ptr) : super._();

  /// Checks if the audio context is initialized and valid.
  ///
  /// Returns `true` if the context is initialized and valid; otherwise,
  /// `false`.
  bool get contextIsInitialized => _bindings.audio_context_is_valid(_resource);

  /// The finalizer for the `AudioContext` class.
  static final _finalizer = NativeFinalizer(
    _bindings.addresses.audio_context_destroy.cast(),
  );

  @override
  NativeFinalizer get finalizer => _finalizer;

  @override
  void releaseResource() {
    _bindings.audio_context_destroy(_resource);
  }

  /// Refreshes the list of available audio devices.
  ///
  /// Queries the system for the latest playback and capture devices, updating
  /// the internal state of the `AudioContext`. Throws an exception if the
  /// operation fails.
  ///
  /// Example:
  /// ```dart
  /// audioContext.refreshDevices();
  /// ```
  void refreshDevices() {
    final context = _ensureContextInitialized();

    FFResult(_bindings.audio_context_refresh_devices(context)).throwIfError();
  }

  /// Refreshes the list of devices and retrieves updated information.
  ///
  /// This method calls [refreshDevices] to query the system for updated
  /// playback and capture devices. It then retrieves the device details
  /// as two separate lists.
  ///
  /// Returns:
  /// - A Record, which contains:
  ///   - playback: A list of playback devices.
  ///   - capture: A list of capture devices.
  ///
  /// Throws:
  /// - An exception if the refresh operation or data retrieval fails.
  ({List<DeviceInfo> playback, List<DeviceInfo> capture})
      refreshAndReturnDevices() {
    refreshDevices();

    final playbackDevices = playbackDevicesInfo;
    final captureDevices = captureDevicesInfo;

    return (playback: playbackDevices, capture: captureDevices);
  }

  /// Gets the number of available playback devices.
  ///
  /// Throws an exception if the context is not initialized.
  ///
  /// Returns:
  /// The count of playback devices available on the system.
  int get playbackDevicesCount {
    final context = _ensureContextInitialized();

    final result =
        FFResult(_bindings.audio_context_get_playback_device_count(context))
          ..throwIfError();

    return result.intData;
  }

  /// Gets the number of available capture devices.
  ///
  /// Throws an exception if the context is not initialized.
  ///
  /// Returns:
  /// The count of capture devices available on the system.
  int get captureDevicesCount {
    final context = _ensureContextInitialized();

    final result =
        FFResult(_bindings.audio_context_get_capture_device_count(context))
          ..throwIfError();

    return result.intData;
  }

  /// Retrieves information about all available playback devices.
  ///
  /// Throws an exception if the context is not initialized.
  ///
  /// Returns:
  /// A list of [DeviceInfo] objects representing playback devices.
  List<DeviceInfo> get playbackDevicesInfo {
    final context = _ensureContextInitialized();
    final result = FFResult<device_info_t>(
      _bindings.audio_context_get_playback_devices_info(context),
    )..throwIfError();

    return _extractDeviceInfoList(result.data, playbackDevicesCount);
  }

  /// Retrieves information about all available capture devices.
  ///
  /// Throws an exception if the context is not initialized.
  ///
  /// Returns:
  /// A list of [DeviceInfo] objects representing capture devices.
  List<DeviceInfo> get captureDevicesInfo {
    final context = _ensureContextInitialized();
    final result = FFResult<device_info_t>(
      _bindings.audio_context_get_capture_devices_info(context),
    )..throwIfError();

    return _extractDeviceInfoList(result.data, captureDevicesCount);
  }

  /// Ensures that the context is initialized and returns its pointer.
  ///
  /// Throws a [StateError] if the context is not initialized.
  ///
  /// Example:
  /// ```dart
  /// final contextPointer = audioContext._requireContext();
  /// ```
  @pragma('vm:prefer-inline')
  Pointer<Void> _ensureContextInitialized() {
    if (_isFinalized) {
      throw StateError('AudioContext is finalized');
    }

    return _resource;
  }

  /// Extracts a list of [DeviceInfo] objects from a pointer to an array of
  /// native devices.
  ///
  /// This method iterates over the native device array, converting each device
  /// into a Dart [DeviceInfo] object, including supported formats.
  ///
  /// - [devicesPointer]: A pointer to the first element in the native device
  /// array.
  /// - [count]: The number of devices in the array.
  ///
  /// Returns:
  /// A list of [DeviceInfo] objects, each containing:
  /// - [id]: The device's unique identifier.
  /// - [name]: The device's name as a string.
  /// - [isDefault]: Whether the device is the default device.
  /// - [supportedFormats]: A list of formats supported by the device.
  List<DeviceInfo> _extractDeviceInfoList(
    Pointer<device_info_t> devicesPointer,
    int count,
  ) =>
      List.generate(count, (i) {
        final nativeDeviceInfo = devicesPointer[i];

        return DeviceInfo(
          id: nativeDeviceInfo.id,
          name: arrayCharToString(nativeDeviceInfo.name),
          isDefault: nativeDeviceInfo.isDefault,
          supportedFormats: _extractSupportedFormats(nativeDeviceInfo),
        );
      });

  /// Converts native formats into a list of [SupportedFormat].
  ///
  /// This method iterates over the supported formats of a native device and
  /// converts each format into a Dart [SupportedFormat] object.
  ///
  /// - [nativeDeviceInfo]: A native structure containing device information,
  ///   including supported formats.
  ///
  /// Returns:
  /// A list of [SupportedFormat] objects, each containing:
  /// - [format]: The sample format.
  /// - [channels]: The number of audio channels.
  /// - [sampleRate]: The sampling rate in Hz.
  /// - [flags]: Reserved for additional flags.
  /// - [nativeFormat]: The original native format.
  /// - [bytesPerSample]: The size of one sample in bytes.
  /// - [bytesPerFrame]: The size of one audio frame in bytes
  /// (sample size * channels).
  List<SupportedFormat> _extractSupportedFormats(
    device_info_t nativeDeviceInfo,
  ) =>
      List.generate(
        nativeDeviceInfo.dataFormatCount,
        (i) {
          final nativeFormat = nativeDeviceInfo.dataFormats[i];
          final bps = _bindings.get_bytes_per_sample(nativeFormat.format);
          final bpf = bps * nativeFormat.channels;

          return SupportedFormat(
            format: SampleFormat.fromValue(nativeFormat.formatAsInt),
            channels: nativeFormat.channels,
            sampleRate: nativeFormat.sampleRate,
            flags: nativeFormat.flags,
            nativeFormat: nativeFormat,
            bytesPerSample: bps,
            bytesPerFrame: bpf,
          );
        },
      );
}
