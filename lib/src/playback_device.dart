import 'dart:ffi' as ffi;
import 'dart:typed_data';

import 'package:ffi/ffi.dart';

import '../pro_miniaudio.dart';
import 'generated/bindings.dart';
import 'library.dart';

/// A class for managing audio playback devices.
///
/// The `PlaybackDevice` class provides methods to control audio playback,
/// such as starting playback, pushing audio buffers, stopping playback,
/// and releasing resources.
final class PlaybackDevice {
  /// Creates a new playback device instance.
  ///
  /// - [deviceId]: The unique identifier of the playback device.
  /// - [context]: The [AudioContext] associated with this device.
  /// - [bufferSizeInBytes]: The size of the audio buffer in bytes.
  /// - [format]: The audio format that this device will use for playback.
  ///
  /// Throws an exception if the [AudioContext] is not initialized or
  /// if the device creation fails.
  PlaybackDevice({
    required Object deviceId,
    required AudioContext context,
    required int bufferSizeInBytes,
    required SupportedFormat format,
  }) : _supportedFormat = format {
    if (!context.contextIsInitialized) {
      throw Exception('Audio context is not initialized');
    }

    final result = FFResult<ffi.Void>(
      _bindings.playback_device_create(
        context.nativeContext,
        bufferSizeInBytes,
        deviceId as device_id_t,
        format.nativeFormat,
      ),
    )..throwIfError();

    _device = result.data;
  }

  /// The audio format supported by this playback device.
  ///
  /// This format defines the sample rate, channels, and data representation.
  final SupportedFormat _supportedFormat;

  /// Pointer to the native playback device.
  ///
  /// This is used internally for interacting with the underlying
  /// native playback device.
  late ffi.Pointer<ffi.Void>? _device;

  /// Provides access to the native bindings for playback device operations.
  ProMiniaudioBindings get _bindings => Library.instance.bindings;

  /// Starts audio playback on the device.
  ///
  /// Throws an exception if the operation fails or if the device is not
  /// properly initialized.
  void start() {
    FFResult<ffi.Void>(_bindings.playback_device_start(_device!))
        .throwIfError();
  }

  /// Pushes an audio buffer to the playback device for playback.
  ///
  /// - [buffer]: A [Float32List] containing the audio samples.
  /// - [framesCount]: The number of frames in the buffer.
  ///
  /// Each frame contains samples for all channels. For example, in a stereo
  /// configuration (2 channels), one frame includes 2 samples.
  ///
  /// Throws an exception if the operation fails or if the buffer cannot be
  /// processed.
  void pushBuffer({required Float32List buffer, required int framesCount}) {
    final data =
        malloc.allocate<playback_data_t>(ffi.sizeOf<playback_data_t>());

    final sizeInBytes = framesCount * _supportedFormat.bytesPerFrame;

    final pUserData = malloc<ffi.Float>(sizeInBytes);

    try {
      // Fill the allocated memory with audio buffer data.
      pUserData
          .asTypedList(framesCount * _supportedFormat.channels)
          .setAll(0, buffer);

      // Set data properties for the native structure.
      data.ref.pUserData = pUserData.cast();
      data.ref.sizeInBytes = sizeInBytes;
      data.ref.formatAsInt = _supportedFormat.format.value;

      // Push the buffer to the playback device.
      FFResult<ffi.Void>(
        _bindings.playback_device_push_buffer(_device!, data),
      ).throwIfError();
    } finally {
      // Free allocated memory to avoid leaks.
      malloc
        ..free(pUserData)
        ..free(data);
    }
  }

  /// Stops audio playback on the device.
  ///
  /// This method halts any ongoing playback and prepares the device
  /// for further operations if needed.
  ///
  /// Throws an exception if the operation fails or if the device is not
  /// properly initialized.
  void stop() {
    FFResult<ffi.Void>(_bindings.playback_device_stop(_device!)).throwIfError();
  }

  /// Releases the resources associated with this playback device.
  ///
  /// This method must be called when the playback device is no longer needed
  /// to free up native resources and prevent memory leaks.
  void dispose() {
    _bindings.playback_device_destroy(_device!);
  }
}
