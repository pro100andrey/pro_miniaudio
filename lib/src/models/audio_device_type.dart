part of '../library.dart';

/// Defines the type of the audio device.
enum AudioDeviceType {
  /// Playback device.
  playback(1),

  /// Capture device.
  capture(2),

  /// Duplex device.
  duplex(3),

  /// Loopback device.
  loopback(4);

  const AudioDeviceType(this.value);

  final int value;

  /// Returns the [AudioDeviceType] corresponding to the given integer [value].
  ///
  /// Throws a [RangeError] if the [value] is not valid.
  static AudioDeviceType fromValue(int value) {
    if (value < 0 || value >= AudioDeviceType.values.length) {
      throw RangeError('Invalid AudioDeviceType value: $value');
    }

    return AudioDeviceType.values[value];
  }
}
