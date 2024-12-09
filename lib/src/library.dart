import 'dart:ffi';
import 'dart:io';
import 'dart:typed_data';

import 'package:equatable/equatable.dart';
import 'package:ffi/ffi.dart';
import 'package:meta/meta.dart';

import 'generated/bindings.dart';

part 'context.dart';
part 'internal.dart';
part 'models/device_info.dart';
part 'models/sample_format.dart';
part 'models/waveform_config.dart';
part 'models/waveform_type.dart';
part 'models/log_level.dart';
part 'native_resource.dart';
part 'playback_device.dart';
part 'waveform.dart';
part 'file_logger.dart';

ProMiniaudioBindings get _bindings => Library.instance.bindings;

final class Library {
  const Library._(this.bindings);

  /// Instance of the native FFI bindings.
  final ProMiniaudioBindings bindings;

  static Library instance = Library._(_loadBindings());

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
