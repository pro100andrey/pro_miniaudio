part of '../library.dart';

/// Represents a unique identifier for an audio device.
///
/// The `DeviceId` class encapsulates a unique identifier for an audio device.
/// This identifier is used to reference a specific device when configuring
/// audio pipelines or enumerating available devices.
final class DeviceId extends AutoFreePointer<device_id>
    with AutoFreePointerHash {
  DeviceId._(super.ptr) : super._();

  /// A prime number used in hash computation for the device identifier.
  @override
  int get prime => 1000;

  /// Precomputed hash value for this `DeviceId`.
  ///
  /// This field is used for comparing device identifiers for equality.
  /// The hash is generated based on the pointer value and the `prime` number.
  @override
  // ignore: unnecessary_overrides
  int get hash => super.hash;
}
