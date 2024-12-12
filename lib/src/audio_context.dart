part of 'library.dart';

/// A class for managing audio contexts and devices.
///
/// The `AudioContext` class is the central manager for the audio system,
/// responsible for initializing resources, refreshing device lists, and
/// retrieving information about playback and capture devices. It provides an
/// easy-to-use interface for enumerating devices and querying their
/// capabilities.
///
/// ### Example Usage:
/// ```dart
/// final context = AudioContext();
///
/// // Refresh device list.
/// context.refreshDevices();
///
/// // Retrieve playback and capture devices.
/// final playbackDevices = context.playbackDeviceInfos;
/// final captureDevices = context.captureDeviceInfos;
///
/// print('Playback Devices: ${playbackDevices.length}');
/// print('Capture Devices: ${captureDevices.length}');
///
/// // Dispose of the context when done.
/// context.dispose();
/// ```
final class AudioContext extends NativeResource<Void> {
  /// Creates a new [AudioContext] instance.
  ///
  /// Initializes the audio system and allocates resources. Throws an
  /// exception if the initialization fails.
  factory AudioContext() => AudioContext._(_bindings.audio_context_create());

  /// Internal constructor.
  ///
  /// This constructor is used by the factory and should not be called
  /// directly.
  AudioContext._(super.ptr) : super._();

  @protected
  @override
  NativeFinalizer get finalizer => Library.contextFinalizer;

  @protected
  @override
  void releaseResource() => _bindings.audio_context_destroy(
        ensureIsNotFinalized(),
      );

  /// Refreshes the list of available audio devices.
  ///
  /// This method updates the internal state of the context by querying the
  /// system for the latest playback and capture devices. This should be called
  /// whenever there is a change in the system's audio configuration.
  ///
  /// Throws:
  /// - An exception if the context is finalized.
  /// - An exception if the refresh operation fails.
  ///
  /// ### Example:
  /// ```dart
  /// context.refreshDevices();
  /// ```
  void refreshDevices() => _bindings.audio_context_refresh_devices(
        ensureIsNotFinalized(),
      );

  /// Gets the number of available playback devices.
  ///
  /// Throws:
  /// - An exception if the context is finalized.
  ///
  /// Returns:
  /// - The count of playback devices available on the system.
  ///
  /// ### Example:
  /// ```dart
  /// print('Playback Devices: ${context.playbackDeviceCount}');
  /// ```
  int get playbackDeviceCount =>
      _bindings.audio_context_get_playback_device_count(
        ensureIsNotFinalized(),
      );

  /// Gets the number of available capture devices.
  ///
  /// Throws:
  /// - An exception if the context is finalized.
  ///
  /// Returns:
  /// - The count of capture devices available on the system.
  ///
  /// ### Example:
  /// ```dart
  /// print('Capture Devices: ${context.captureDeviceCount}');
  /// ```
  int get captureDeviceCount =>
      _bindings.audio_context_get_capture_device_count(
        ensureIsNotFinalized(),
      );

  /// Retrieves information about all available playback devices.
  ///
  /// Throws:
  /// - An exception if the context is finalized.
  ///
  /// Returns:
  /// - A list of [DeviceInfo] objects representing playback devices.
  ///
  /// ### Example:
  /// ```dart
  /// final playbackDevices = context.playbackDeviceInfos;
  /// for (var device in playbackDevices) {
  ///   print('Playback Device: ${device.name}');
  /// }
  /// ```
  List<DeviceInfo> get playbackDeviceInfos => _extractDeviceInfoList(
        _bindings.audio_context_get_playback_device_infos(
          ensureIsNotFinalized(),
        ),
        playbackDeviceCount,
      );

  /// Retrieves information about all available capture devices.
  ///
  /// Throws:
  /// - An exception if the context is finalized.
  ///
  /// Returns:
  /// - A list of [DeviceInfo] objects representing capture devices.
  ///
  /// ### Example:
  /// ```dart
  /// final captureDevices = context.captureDeviceInfos;
  /// for (var device in captureDevices) {
  ///   print('Capture Device: ${device.name}');
  /// }
  /// ```
  List<DeviceInfo> get captureDeviceInfos => _extractDeviceInfoList(
        _bindings.audio_context_get_capture_device_infos(
          ensureIsNotFinalized(),
        ),
        captureDeviceCount,
      );

  /// Extracts a list of [DeviceInfo] objects from a native device array.
  ///
  /// This helper method converts a pointer to a native device array into
  /// a list of Dart [DeviceInfo] objects.
  ///
  /// - [devicesPointer]: A pointer to the first element of the native device
  /// array.
  /// - [count]: The number of devices in the array.
  ///
  /// Returns:
  /// - A list of [DeviceInfo] objects.
  ///
  /// Throws:
  /// - An exception if the pointer is invalid.
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
          audioFormats: _extractSupportedFormats(nativeDeviceInfo),
        );
      });

  /// Converts supported formats from native to Dart.
  ///
  /// This helper method iterates over the formats of a native device and
  /// converts them into Dart [AudioFormat] objects.
  ///
  /// - [nativeDeviceInfo]: A native structure containing device information.
  ///
  /// Returns:
  /// - A list of [AudioFormat] objects.
  List<AudioFormat> _extractSupportedFormats(
    device_info_t nativeDeviceInfo,
  ) =>
      List.generate(
        nativeDeviceInfo.formatCount,
        (i) {
          final nativeAudioFormat = nativeDeviceInfo.audioFormats[i];

          return AudioFormat(
            pcmFormat: PcmFormat.fromValue(
              nativeAudioFormat.pcmFormatAsInt,
            ),
            channels: nativeAudioFormat.channels,
            sampleRate: nativeAudioFormat.sampleRate,
          );
        },
      );
}
