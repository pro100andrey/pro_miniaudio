part of '../library.dart';

/// Enum representing the possible states of a playback device.
///
/// These states indicate the current status of the playback device,
/// such as whether it is initialized, started, or stopped.
enum DeviceState {
  /// The playback device is uninitialized.
  ///
  /// The device has not yet been initialized or is in an undefined state.
  uninitialized(0),

  /// The playback device is stopped.
  ///
  /// The device is initialized but not actively processing or playing audio.
  stopped(1),

  /// The playback device is started.
  ///
  /// The device is actively processing or playing audio.
  started(2),

  /// The playback device is starting.
  ///
  /// The device is in the process of transitioning from a stopped state to a
  /// started state.
  starting(3),

  /// The playback device is stopping.
  ///
  /// The device is in the process of transitioning from a started state to a
  /// stopped state.
  stopping(4);

  /// Creates a [DeviceState] instance with the associated integer value.
  const DeviceState(this.value);

  /// The integer value representing the state of the playback device.
  final int value;

  /// Returns the [DeviceState] corresponding to the given integer [value].
  ///
  /// Throws a [RangeError] if the value is out of the valid range.
  static DeviceState fromValue(int value) {
    if (value < 0 || value >= DeviceState.values.length) {
      throw RangeError('Invalid DeviceState value: $value');
    }

    return DeviceState.values[value];
  }

  /// Returns the name of the device state as a string.
  ///
  /// For unknown states, it returns `'unknown'`.
  String get name {
    switch (this) {
      case DeviceState.uninitialized:
        return 'uninitialized';
      case DeviceState.stopped:
        return 'stopped';
      case DeviceState.started:
        return 'started';
      case DeviceState.starting:
        return 'starting';
      case DeviceState.stopping:
        return 'stopping';
    }
  }
}
