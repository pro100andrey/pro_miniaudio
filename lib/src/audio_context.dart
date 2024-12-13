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
  NativeFinalizer get finalizer => Library._contextFinalizer;

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

  List<DeviceInfo> getDeviceInfos({required AudioDeviceType type}) {
    final devicesInfos = _bindings.audio_context_get_device_infos(
      ensureIsNotFinalized(),
      type.toNative(),
    );

    final deviceInfos = DeviceInfos._(devicesInfos);
    final deviceList = deviceInfos.getList();

    return deviceList;
  }

  // List<DeviceInfo> _extractDeviceInfoList(
  //   Pointer<device_info_t> devicesPointer,
  //   int count,
  // ) =>
  //     List.generate(count, (i) {
  //       final nativeDeviceInfo = devicesPointer[i];

  //       return DeviceInfo(
  //         id: nativeDeviceInfo.id,
  //         name: arrayCharToString(nativeDeviceInfo.name),
  //         isDefault: nativeDeviceInfo.isDefault,
  //       );
  //     });

  // List<AudioFormat> _extractSupportedFormats(
  //   device_info_t nativeDeviceInfo,
  // ) =>
  //     List.generate(
  //       nativeDeviceInfo.formatCount,
  //       (i) {
  //         final nativeAudioFormat = nativeDeviceInfo.audioFormats[i];

  //         return AudioFormat(
  //           pcmFormat: PcmFormat.fromValue(
  //             nativeAudioFormat.pcmFormatAsInt,
  //           ),
  //           channels: nativeAudioFormat.channels,
  //           sampleRate: nativeAudioFormat.sampleRate,
  //         );
  //       },
  //     );
}
