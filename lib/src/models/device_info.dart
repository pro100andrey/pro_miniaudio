part of '../library.dart';

/// Represents information about an audio device.
///
/// The `DeviceInfo` class provides metadata about an audio device,
/// including its unique identifier, name, default status, and supported
/// audio formats. This class is typically used to enumerate available
/// audio devices or configure audio pipelines.
final class DeviceInfo extends Equatable {
  /// Creates a new [DeviceInfo] instance with the given properties.
  ///
  /// - [id]: The unique identifier of the device.
  /// - [name]: The user-friendly name of the device.
  /// - [isDefault]: Indicates whether this device is the default audio device.
  const DeviceInfo({
    required this.id,
    required this.name,
    required this.isDefault,
  });

  /// The unique identifier of the device.
  ///
  /// This identifier is platform-specific and could be a string, integer,
  /// or other type depending on the implementation. It is used to select
  /// or reference a specific device programmatically.
  ///
  /// Example:
  /// ```dart
  /// print('Device ID: ${device.id}');
  /// ```
  final DeviceId id;

  /// The user-friendly name of the device.
  ///
  /// This is a human-readable name that can be displayed in a UI, such as
  /// "Built-in Audio" or "USB Audio Device".
  ///
  /// Example:
  /// ```dart
  /// print('Device Name: ${device.name}');
  /// ```
  final String name;

  /// Indicates whether this device is the default audio device.
  ///
  /// The default device is automatically selected for audio playback or
  /// recording unless explicitly overridden.
  ///
  /// Example:
  /// ```dart
  /// if (device.isDefault) {
  ///   print('This is the default audio device.');
  /// }
  /// ```
  final bool isDefault;

  /// A list of supported audio data formats for this device.
  ///
  /// Each entry in the list describes a specific format that the device can
  /// handle, including details like sample rate, number of channels, and
  /// sample format.
  ///
  /// Example:
  /// ```dart
  /// for (var format in device.audioFormats) {
  ///   print('Supported Format: ${format.sampleFormat.name}, '
  ///         '${format.channels} channels, ${format.sampleRate} Hz');
  /// }
  /// ```

  @override
  List<Object?> get props => [name, id.hash];
}
