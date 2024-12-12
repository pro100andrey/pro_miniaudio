part of '../library.dart';

/// Enum representing the possible states of a playback or capture device.
///
/// The `DeviceState` enum describes the current operational state of a device,
/// such as whether it is initialized, actively processing audio, or stopped.
///
/// ### Device States
/// - [uninitialized]: The device has not been initialized or is in an undefined
/// state.
/// - [stopped]: The device is initialized but not actively processing audio.
/// - [started]: The device is actively processing or playing audio.
/// - [starting]: The device is transitioning from a stopped state to a started
/// state.
/// - [stopping]: The device is transitioning from a started state to a stopped
///  state.
/// ```
enum DeviceState {
  /// The device is uninitialized.
  ///
  /// The device has not been set up and cannot process audio in this state.
  uninitialized(0),

  /// The device is stopped.
  ///
  /// The device has been initialized but is not actively processing or
  /// playing audio.
  stopped(1),

  /// The device is started.
  ///
  /// The device is actively processing or playing audio.
  started(2),

  /// The device is starting.
  ///
  /// The device is transitioning from a stopped state to a started state.
  starting(3),

  /// The device is stopping.
  ///
  /// The device is transitioning from a started state to a stopped state.
  stopping(4);

  /// Creates a [DeviceState] instance with the associated integer value.
  ///
  /// Each state corresponds to a unique integer value, which can be used for
  /// interoperability with native libraries or APIs.
  const DeviceState(this.value);

  /// The integer value representing the device's state.
  ///
  /// This value is useful when interacting with native APIs or performing
  /// state comparisons programmatically.
  final int value;

  /// Returns the [DeviceState] corresponding to the given integer [value].
  ///
  /// Throws a [RangeError] if the value is not valid.
  static DeviceState fromValue(int value) {
    if (value < 0 || value >= DeviceState.values.length) {
      throw RangeError('Invalid DeviceState value: $value');
    }
    return DeviceState.values[value];
  }

  /// Returns `true` if the state is [DeviceState.uninitialized].
  ///
  /// ### Example
  /// ```dart
  /// if (state.isUninitialized) {
  ///   print('Device is not initialized.');
  /// }
  /// ```
  bool get isUninitialized => this == DeviceState.uninitialized;

  /// Returns `true` if the state is [DeviceState.stopped].
  ///
  /// ### Example
  /// ```dart
  /// if (state.isStopped) {
  ///   print('Device is stopped.');
  /// }
  /// ```
  bool get isStopped => this == DeviceState.stopped;

  /// Returns `true` if the state is [DeviceState.started].
  ///
  /// ### Example
  /// ```dart
  /// if (state.isStarted) {
  ///   print('Device is playing audio.');
  /// }
  /// ```
  bool get isStarted => this == DeviceState.started;

  /// Returns `true` if the state is [DeviceState.starting].
  ///
  /// ### Example
  /// ```dart
  /// if (state.isStarting) {
  ///   print('Device is starting up.');
  /// }
  /// ```
  bool get isStarting => this == DeviceState.starting;

  /// Returns `true` if the state is [DeviceState.stopping].
  ///
  /// ### Example
  /// ```dart
  /// if (state.isStopping) {
  ///   print('Device is shutting down.');
  /// }
  /// ```
  bool get isStopping => this == DeviceState.stopping;

  /// Returns the name of the current state as a string.
  ///
  /// ### Example
  /// ```dart
  /// print('Current state: ${state.name}');
  /// ```
  String get name {
    switch (this) {
      case uninitialized:
        return 'uninitialized';
      case stopped:
        return 'stopped';
      case started:
        return 'started';
      case starting:
        return 'starting';
      case stopping:
        return 'stopping';
    }
  }
}
