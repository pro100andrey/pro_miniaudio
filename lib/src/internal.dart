part of 'library.dart';

/// Converts a native `Array<Char>` to a Dart string.
///
/// - [array]: The native array of characters.
/// - [maxLength]: The maximum length to process (default: 256). This ensures
///   safety in case of missing null terminator.
///
/// Returns a Dart string.
String arrayCharToString(Array<Char> array, {int maxLength = 256}) {
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

/// Converts a Dart string to a native `Pointer<Char>`.
/// The returned pointer must be freed using `malloc.free(pointer)`.

/// - [string]: The Dart string to convert.
/// Returns a native `Pointer<Char>`.
Pointer<Char> stringToCharPointer(String string) {
  final pointer = malloc<Char>(string.length + 1);

  for (var i = 0; i < string.length; i++) {
    (pointer + i).value = string.codeUnitAt(i);
  }

  (pointer + string.length).value = 0;

  return pointer;
}

extension SampleFormatExt on SampleFormat {
  sample_format_t toNative() => sample_format_t.values[index];
}

extension WaveformTypeExt on WaveformType {
  waveform_type_t toNative() => waveform_type_t.values[index];
}

extension LogLevelExt on FileLogLevel {
  LogLevel toNative() => LogLevel.values[index];
}
