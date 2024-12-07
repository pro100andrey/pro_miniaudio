part of 'library.dart';


/// A helper class to manage the `Result` structure returned by native
/// functions.
extension type FFResult<T extends NativeType>(result_t result) {
  void throwIfError() {
    if (result.code != error_code_t.error_code_none) {
      final message = arrayCharToString(result.message);

      throw Exception('Error: ${result.code} - $message');
    }
  }

  Pointer<T> get data => result.data.pData.cast<T>();

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

extension SampleFormatExt on SampleFormat {
  sample_format_t toNative() => sample_format_t.values[index];
}

extension WaveformTypeExt on WaveformType {
  waveform_type_t toNative() => waveform_type_t.values[index];
}
