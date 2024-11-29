import 'package:equatable/equatable.dart';

import 'sample_format.dart';

/// Represents information about an audio device.
///
/// This class provides metadata about an audio device, such as its unique
/// identifier, name, whether it is the default device, and the list of
/// supported audio data formats.
final class DeviceInfo extends Equatable {
  /// Creates a new [DeviceInfo] instance with the given properties.
  ///
  /// - [id]: The unique identifier of the device.
  /// - [name]: The name of the device.
  /// - [isDefault]: Whether this device is the default audio device.
  /// - [supportedFormats]: A list of supported audio data formats.
  const DeviceInfo({
    required this.id,
    required this.name,
    required this.isDefault,
    required this.supportedFormats,
  });

  /// The unique identifier of the device.
  ///
  /// This could be a platform-specific identifier, such as a string or
  /// integer, depending on the implementation.
  final Object id;

  /// The user-friendly name of the device.
  final String name;

  /// Indicates whether this device is the default audio device.
  ///
  /// A default device is typically the one automatically selected for audio
  /// playback or recording.
  final bool isDefault;

  /// The list of supported audio data formats for this device.
  ///
  /// Each entry in the list describes a specific format that the device can
  /// handle, including its sample rate, channel configuration, and sample
  /// format.
  final List<SupportedFormat> supportedFormats;

  @override
  List<Object?> get props => [name, isDefault, supportedFormats];
}

/// Represents a supported audio data format.
///
/// This class provides detailed information about a specific audio data format
/// that a device supports, including sample format, channel count, sample rate,
/// and more.
final class SupportedFormat extends Equatable {
  /// Creates a new [SupportedFormat] instance with the given properties.
  ///
  /// - [format]: The audio sample format (e.g., 16-bit signed integer).
  /// - [channels]: The number of audio channels (e.g., 1 for mono, 2 for
  /// stereo).
  /// - [sampleRate]: The sample rate in Hertz.
  /// - [flags]: Format flags (reserved for future use).
  /// - [nativeFormat]: The native format representation (platform-specific).
  /// - [bytesPerSample]: The number of bytes per individual sample.
  /// - [bytesPerFrame]: The number of bytes per frame (sample × channels).
  const SupportedFormat({
    required this.format,
    required this.channels,
    required this.sampleRate,
    required this.flags,
    required this.nativeFormat,
    required this.bytesPerSample,
    required this.bytesPerFrame,
  });

  /// The audio sample format.
  ///
  /// Examples include 16-bit signed integer (`s16`) or 32-bit float (`f32`).
  final SampleFormat format;

  /// The number of audio channels.
  ///
  /// Examples:
  /// - `1`: Mono
  /// - `2`: Stereo
  /// - `6`: 5.1 Surround
  final int channels;

  /// The sample rate in Hertz.
  ///
  /// Examples:
  /// - `44100`: Standard CD-quality audio.
  /// - `48000`: Professional audio.
  final int sampleRate;

  /// Format flags (reserved for future use).
  ///
  /// This field is currently not used but is included for potential future
  /// extensions.
  final int flags;

  /// The native format representation.
  ///
  /// This is a platform-specific representation of the format. For example,
  /// on some platforms, this might be an integer code or a string.
  final dynamic nativeFormat;

  /// The number of bytes per individual sample.
  ///
  /// This depends on the sample format. For example:
  /// - `2`: For 16-bit signed integer samples.
  /// - `4`: For 32-bit float samples.
  final int bytesPerSample;

  /// The number of bytes per audio frame.
  ///
  /// This is calculated as `bytesPerSample × channels`.
  final int bytesPerFrame;

  @override
  List<Object?> get props =>
      [format, channels, sampleRate, flags, bytesPerSample, bytesPerFrame];
}
