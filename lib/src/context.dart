part of 'library.dart';

/// A class for managing contexts and devices.
///
/// The `Context` is responsible for initializing the audio system,
/// retrieving information about playback and capture devices, and managing
/// the resources associated with the audio context.
///
/// Example:
/// ```dart
/// final context = Context();
/// final devices = context.refreshAndReturnDevices();
/// print('Playback Devices: ${devices.playback.length}');
/// print('Capture Devices: ${devices.capture.length}');
/// context.dispose();
/// ```
final class Context extends NativeResource<Void> {
  factory Context() => Context._(_bindings.context_create());

  /// Internal constructor.
  Context._(super.ptr) : super._();

  /// The finalizer for the `Context` class.
  static final _finalizer = NativeFinalizer(
    _bindings.addresses.context_destroy.cast(),
  );

  @override
  NativeFinalizer get finalizer => _finalizer;

  @override
  void releaseResource() => _bindings.context_destroy(
        ensureResourceIsNotFinalized(),
      );

  /// Refreshes the list of available audio devices.
  ///
  /// Queries the system for the latest playback and capture devices, updating
  /// the internal state of the `Context`. Throws an exception if the
  /// operation fails.
  ///
  /// Throws an exception if the context is finalized.
  ///
  /// Example:
  /// ```dart
  /// context.refreshDevices();
  /// ```
  void refreshDevices() => _bindings.context_refresh_devices(
        ensureResourceIsNotFinalized(),
      );

  /// Gets the number of available playback devices.
  ///
  /// Throws an exception if the context is finalized.
  ///
  /// Returns:
  /// The count of playback devices available on the system.
  int get playbackDeviceCount => _bindings.context_get_playback_device_count(
        ensureResourceIsNotFinalized(),
      );

  /// Gets the number of available capture devices.
  ///
  /// Throws an exception if the context is finalized.
  ///
  /// Returns:
  /// The count of capture devices available on the system.
  int get captureDeviceCount => _bindings.context_get_capture_device_count(
        ensureResourceIsNotFinalized(),
      );

  /// Retrieves information about all available playback devices.
  ///
  /// Throws an exception if the context is finalized.
  ///
  /// Returns:
  /// A list of [DeviceInfo] objects representing playback devices.
  List<DeviceInfo> get playbackDeviceInfos => _extractDeviceInfoList(
        _bindings.context_get_playback_device_infos(
          ensureResourceIsNotFinalized(),
        ),
        playbackDeviceCount,
      );

  /// Retrieves information about all available capture devices.
  ///
  /// Throws an exception if the context is finalized.
  ///
  /// Returns:
  /// A list of [DeviceInfo] objects representing capture devices.
  List<DeviceInfo> get captureDeviceInfos => _extractDeviceInfoList(
        _bindings.context_get_capture_device_infos(
          ensureResourceIsNotFinalized(),
        ),
        captureDeviceCount,
      );

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
          audioFormats: _extractSupportedFormats(nativeDeviceInfo),
        );
      });

  /// Converts native formats into a list of [AudioFormat].
  ///
  /// This method iterates over the supported formats of a native device and
  /// converts each format into a Dart [AudioFormat] object.
  ///
  /// - [nativeDeviceInfo]: A native structure containing device information,
  ///   including supported formats.
  ///
  /// Returns:
  /// A list of [AudioFormat] objects, each containing:
  /// - [format]: The sample format.
  /// - [channels]: The number of audio channels.
  /// - [sampleRate]: The sampling rate in Hz.
  /// - [flags]: Reserved for additional flags.
  /// - [nativeFormat]: The original native format.
  /// - [bytesPerSample]: The size of one sample in bytes.
  /// - [bytesPerFrame]: The size of one audio frame in bytes
  /// (sample size * channels).
  List<AudioFormat> _extractSupportedFormats(
    device_info_t nativeDeviceInfo,
  ) =>
      List.generate(
        nativeDeviceInfo.formatCount,
        (i) {
          final nativeAudioFormat = nativeDeviceInfo.audioFormats[i];

          return AudioFormat(
            sampleFormat: SampleFormat.fromValue(
              nativeAudioFormat.sampleFormatAsInt,
            ),
            channels: nativeAudioFormat.channels,
            sampleRate: nativeAudioFormat.sampleRate,
            flags: nativeAudioFormat.flags,
          );
        },
      );
}
