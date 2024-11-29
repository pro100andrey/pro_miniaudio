import 'dart:ffi' as ffi;
import 'dart:io';

import '../pro_miniaudio.dart';
import 'generated/bindings.dart';

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

    final ffi.DynamicLibrary dylib;
    if (Platform.isMacOS || Platform.isIOS) {
      dylib = ffi.DynamicLibrary.open('$libName.framework/$libName');
    } else if (Platform.isAndroid || Platform.isLinux) {
      dylib = ffi.DynamicLibrary.open('lib$libName.so');
    } else if (Platform.isWindows) {
      dylib = ffi.DynamicLibrary.open('$libName.dll');
    } else {
      throw UnsupportedError(
        'Unsupported platform: ${Platform.operatingSystem}',
      );
    }

    final bindings = ProMiniaudioBindings(dylib)..resource_manager_free_all();

    return bindings;
  }
}

/// A helper class to manage the `Result` structure returned by native
/// functions.
extension type FFResult<T extends ffi.NativeType>(result_t result) {
  void throwIfError() {
    if (result.code != error_code_t.error_code_none) {
      final message = arrayCharToString(result.message);

      throw Exception('Error: ${result.code} - $message');
    }
  }

  ffi.Pointer<T> get data => result.data.pData.cast<T>();

  int get intData {
    if (result.type != data_type_t.data_type_int) {
      throw StateError('Expected integer data but got ${result.type}.');
    }

    return result.data.intData;
  }
}

/// Converts a native `Array<Char>` to a Dart string.
///
/// - [array]: The native array of characters.
/// - [maxLength]: The maximum length to process (default: 256). This ensures
///   safety in case of missing null terminator.
///
/// Returns a Dart string.
String arrayCharToString(ffi.Array<ffi.Char> array, {int maxLength = 256}) {
  // Ensure maxLength is positive
  if (maxLength <= 0) {
    throw ArgumentError('maxLength must be greater than zero');
  }

  final charCodes = <int>[];

  for (var i = 0; i < maxLength; i++) {
    final char = array[i];

    if (char == 0) {
      break; // Stop at null terminator
    }

    charCodes.add(char);
  }

  return String.fromCharCodes(charCodes);
}

extension SampleFormatExt on SampleFormat {
  sample_format_t toNative() => sample_format_t.values[index];
}

extension WaveformTypeExt on WaveformType {
  waveform_type_t toNative() => waveform_type_t.values[index];
}
