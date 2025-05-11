import 'dart:ffi';
import 'dart:io';
import 'dart:typed_data';

import 'package:equatable/equatable.dart';
import 'package:ffi/ffi.dart';
import 'package:flutter/material.dart';
import 'package:meta/meta.dart';

import 'generated/bindings.dart';

part 'audio_context.dart';
part 'device_infos.dart';
part 'file_logger.dart';
part 'internal.dart';
part 'models/audio_device_type.dart';
part 'models/audio_format.dart';
part 'models/device_id.dart';
part 'models/device_info.dart';
part 'models/device_state.dart';
part 'models/log_level.dart';
part 'models/pcm_format.dart';
part 'models/playback_config.dart';
part 'models/wav_encoder_config.dart';
part 'models/waveform_config.dart';
part 'models/waveform_type.dart';
part 'native_resource.dart';
part 'playback_device.dart';
part 'wav_encoder.dart';
part 'waveform.dart';

ProMiniaudioBindings get _bindings => Library.instance.bindings;

final class Library {
  const Library._(this.bindings);

  /// Instance of the native FFI bindings.
  final ProMiniaudioBindings bindings;

  static Library instance = Library._(_loadBindings());

  /// The finalizer for the `Waveform` class.
  static final _waveformFinalizer = NativeFinalizer(
    _bindings.addresses.waveform_destroy.cast(),
  );

  /// The finalizer for the `Context` class.
  static final _contextFinalizer = NativeFinalizer(
    _bindings.addresses.audio_context_destroy.cast(),
  );

  /// The finalizer for the `DeviceInfos` class.
  static final _deviceInfosFinalizer = NativeFinalizer(
    _bindings.addresses.audio_context_device_infos_destroy.cast(),
  );

  /// The finalizer for the `WavEncoder` class.
  static final _wavEncoderFinalizer = NativeFinalizer(
    _bindings.addresses.encoder_destroy.cast(),
  );

  /// Loads the native `pro_miniaudio` bindings from the platform-specific
  /// dynamic library.
  ///
  /// Throws an `UnsupportedError` if the platform is not supported.
  static ProMiniaudioBindings _loadBindings() {
    const libName = 'pro_miniaudio';

    final DynamicLibrary dylib;
    if (Platform.isMacOS || Platform.isIOS) {
      dylib = DynamicLibrary.open('$libName.framework/$libName');
    } else if (Platform.isAndroid || Platform.isLinux) {
      dylib = DynamicLibrary.open('lib$libName.so');
    } else if (Platform.isWindows) {
      dylib = DynamicLibrary.open('$libName.dll');
    } else {
      throw UnsupportedError(
        'Unsupported platform: ${Platform.operatingSystem}',
      );
    }

    return ProMiniaudioBindings(dylib);
  }
}
